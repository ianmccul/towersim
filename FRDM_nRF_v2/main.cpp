#include "mbed.h"
#include "WakeUp.h"
#include "limits.h"
#include "MMA8451Q.h"
#include "gyrointerface.h"
#include "nRF24L01P.h"
#include "nRF24L01P_PTX.h"
#include "common/matvec/matvec.h"
#include "uid.h"
#include "rgbled.h"
#include <cstdint>
#include <vector>

RGBLED* MyLed;

//
// packet format for sensor readings:
// Multi-byte sequences are in little-endian format, which is the default
// for the KL25z and also the raspberry pi.
//
// hash (4 bytes) | delay (14 bits) seq (2 bits) |flags | seq1 | seq2 (2 bytes) | payload (up to 22 bytes)
// hash is the FNV-1a-32 hash of the remaining bytes in the packet (with extra bytes set to zero to make
// 32 bytes in total, even though we don't transmit extra bytes)
// delay is a 14-bit unsigned in units of 4 microseconds - maximum value
// 16383 represents 65.532 miliseconds.
//
// flags is 8 bits: 0 AAA GGGG
// seq1 is a 1-byte sequentially increasing number
// seq2 is a 16-bit unsigned sequentially increasing record number that counts the record number
// of gyro measurements
// delay/seq is the number of microseconds that this packet has been delayed (unsigned 16 bit
// delay shifted left 2 bits, with 2-bit sequence number).
// AAA is number of 6-byte accelerometer readings (first in the payload)
// GGGG is the number of 2-byte z-axis gyro readings

// packet format for status information:
// hash (4 bytes) | delay/seq (2 bytes) | 10DCTBSX | UID (2 bytes) | acc ODR (2 bytes) | gyro ODR (2 bytes)
// | gyro bandwidth (1 byte) | payload
// UID is a 16-bit hash of the microcontroller unique ID
// if D is set, the device is receiving power
// if C is set, the charging circuit is active and charging the battery, otherwise it is not active
// if T is set, then the payload conains a 1-byte gyro temperature reading
// if B is set, the payload contains a 2-byte battery current reading
// if S is set, then the sensor is preparing for sleep mode
// X = 0 is reserved for future expansion

// Since we are not using the nRF retransmit functionality,
// on a lost ACK packet we will be unaware that the receive was successful,
// and thefore we will send the packet again.  This is the purpose of the 2-bit
// seq number.  If the receiver gets two packets with the same seq number then
// it will check the entire packet, and assume that identical packets with the same
// seq number are duplicates.  Packets that have a different seq number but are otherwise
// identical are allowed.

constexpr float pi = 3.141592653589323;

// a simple LCG for random number generation
uint32_t Seed = UniqueID32;

uint32_t random()
{
   Seed = Seed*1664525 + 1013904223;
   return Seed;
}

int random(int Min, int Max)
{
   uint32_t x = random() >> 16;
   return x%(Max-Min) + Min;
}

// random slot number for exponential backoff retransmit.
int RandomSlot(int n)
{
   int Max = (1 << std::max(n,5));
   return random(0, Max);
}

// we reserve 6 bytes, the 4-byte hash and 2 bites for delay and seq number
constexpr int ReservedBufSize = 6;
constexpr int HashOffset = 0;
constexpr int DelayOffset = 4;

// current charge reading on the battery
// Standard scaling for the battery: divider is 4.7 : 1, max input is 65535 = 3.3v nominal,
// but the actual supply voltage on the kl25z is closer to 2.95v on the rev D board.
// Also 5 ohm drain-source resistance in the mosfet, makes the divider 475 : 100
// analog_batt = (batt / 5.7), output is (batt / 5.7) * (65535 / 3.3) = batt * 3484 nominal,
// analog_batt = (batt / 5.7), output is (batt / 5.7) * (65535 / 2.95) = batt * 3897.4 actual
// analog_batt = (batt / 5.75), output is (batt / 5.75) * (65535 / 2.95) = batt * 3863.5 actual
// analog_batt = (batt / 5.75), output is (batt / 5.75) * (65535 / 2.93) = batt * 3889.9 actual
// 3863.5 agrees reasonably well with measurements.
// So the analog in threshold is 15067.
// HOWEVER, we measure closer to 3.01v with led, transmitter and gyro turned off.
// Hence we should do the voltage conversion ourselves.  With all off the conversion is
// about 3786
constexpr float ChargeScaling = 3863.5;

// Automatic recharge on the battery circuit happens at V = 96.5% of full (spec >= 94%),
// which is 4.2V.  We want to follow the same, and not initiate a recharge until V falls
// to at least 3.948v.  We choose Vthreshold = 3.9v.
constexpr float Vthreshold = 3.9;

float MeasureBatteryCharge(DigitalOut& BatteryDetectEnable, AnalogIn& AnalogBattery)
{
   BatteryDetectEnable = 1;
   wait_us(40);
   uint16_t x = AnalogBattery.read_u16();
   wait_us(40);
   uint16_t y = AnalogBattery.read_u16();
   BatteryDetectEnable = 0;
   return ((x+y) / 2) / ChargeScaling;
}

class PacketScheduler
{
   public:
      PacketScheduler(nRF24L01P_PTX& PTX_);

      // Turns on the device and initializes it
      //void PowerOn();

      void Sleep(int WaitRetransmits = 0);
      void Wakeup();

      // polls the scheduler, to see if we need to do anything.  This should be called
      // at least once every SlotTime microseconds, if possible
      void Poll();

      // send a high priority packet.  These packets have a 2-byte 'delay' field
      // at bytes 2,3 of the packet.  This is zero if the packet is transmitted immediately,
      // but will be non-zero if there is already a packet in progress or if there is
      // a retransmit.
      void SendHighPriority(char const* buf, int Size);

      // send a low priority packet.  There is no packet delay field.  We attempt
      // to transmit the packet whenever the radio is available, and keep trying until
      // the packet is sent (or a new low-priority packet it sent).
      void SendLowPriority(char const* buf, int Size);

   private:
      // our radio device
      nRF24L01P_PTX& PTX;

      // 0 of there is no packet in flight.
      // 1 if there is a high priority packet in flight.
      // 2 if there is a low priority packet in flight.
      int PacketInFlight;

      // true if we have a high priority packet waiting to be sent
      bool HaveHighPriorityPacket;
      Timer HighPriorityTimer;

      // true if the high priority packet in the buffer is new (we haven't attempted a transmit yet)
      bool NewHighPriorityPacket;

      // true if we have a low priority packet waiting to be sent
      bool HaveLowPriorityPacket;
      Timer LowPriorityTimer;

      // true if the low priority packet in the buffer is new (we haven't attempted a transmit yet)
      bool NewLowPriorityPacket;

      // time in microseconds for each slot
      static uint16_t const SlotTime = 250;

      // number of retransmits since the last successful packet
      int NumRetransmits;

      Timer RetransmitTimer;
      int32_t RetransmitPoint;

      uint16_t Seq;  // 2-bit sequence number

      // high and low priority buffers
      uint32_t Pad1;    // make sure we have 32-bit alignment
      uint8_t HiBuf[32];
      int HiBufSz;
      uint32_t Pad2;    // make sure we have 32-bit alignment
      uint8_t LoBuf[32];
      int LoBufSz;
};

PacketScheduler::PacketScheduler(nRF24L01P_PTX& PTX_)
   : PTX(PTX_),
     PacketInFlight(false),
     HaveHighPriorityPacket(false),
     NewHighPriorityPacket(false),
     HaveLowPriorityPacket(false),
     NewLowPriorityPacket(false),
     NumRetransmits(0),
     RetransmitPoint(0),
     Seq(0)
{
}

void
PacketScheduler::Sleep(int WaitRetransmits)
{
   float c = 1.0;
   // Keep sending the packet until at most WaitRetransmits attempts
   while (PacketInFlight != 0 && NumRetransmits < WaitRetransmits)
   {
      MyLed->yellow(c);
      c *= 0.99;
      this->Poll();
   }
   if (PacketInFlight != 0)
   {
      // Finish the packet, we don't care if it didn't make it
      PTX.TransmitWait();
   }

   PTX.PowerDown();
   PacketInFlight = false;
   HaveHighPriorityPacket = false;
   NewHighPriorityPacket = false;
   HaveLowPriorityPacket = false;
   NewLowPriorityPacket = false;
   NumRetransmits = 0;
   RetransmitPoint = 0;
}

void PacketScheduler::Wakeup()
{
   PTX.PowerUp();
   // We must wait until ready to transmit here, or we cause problems later on
   // if the transmit fails unexpectedly (which Poll() doesn't handle)
   while (!PTX.IsReadyTransmit())
   {
      wait_us(10);
   }
}

void
PacketScheduler::Poll()
{
   if (PacketInFlight != 0)
   {
      if (!PTX.IsTransmitFinished())
         return;

      int Result = PTX.TransmitWait();
      if (Result == -1)
      {
         // packet failed, do a retransmit.
         ++NumRetransmits;
         RetransmitTimer.reset();
         RetransmitTimer.start();
         int Slot = RandomSlot(NumRetransmits);
         RetransmitPoint = SlotTime * Slot;
      }
      else if (Result == 0)
      {
         // successful packet transmission
         NumRetransmits = 0;
         RetransmitTimer.stop();
         Seq = (Seq + 1u) & 0x03;
         if (PacketInFlight == 1 && !NewHighPriorityPacket)
         {
            HaveHighPriorityPacket = false;
            HighPriorityTimer.stop();
         }
         else if (PacketInFlight == 2 && !NewLowPriorityPacket)
         {
            HaveLowPriorityPacket = false;
            LowPriorityTimer.stop();
         }
      }
      PacketInFlight = 0;
   }

   // Do we need to wait before transmitting?
   if (NumRetransmits > 0 && RetransmitTimer.read_us() < RetransmitPoint)
      return;

   // Transmit a packet, if we have one ready to go
   if (HaveHighPriorityPacket)
   {
      uint16_t d = HighPriorityTimer.read_us();
      d = (d & 0xFFFC) | Seq;
      std::memcpy(HiBuf+4, &d, 2);
      uint32_t hash = hash_fnv32(HiBuf+4, HiBuf+32);
      std::memcpy(HiBuf, &hash, 4);
      PTX.TransmitPacketNB(HiBuf, HiBufSz);
      PacketInFlight = 1;
      NewHighPriorityPacket = false;
   }
   else if (HaveLowPriorityPacket)
   {
      uint16_t d = LowPriorityTimer.read_us();
      d = (d & 0xFFFC) | Seq;
      std::memcpy(LoBuf+4, &d, 2);
      uint32_t hash = hash_fnv32(LoBuf+4, LoBuf+32);
      std::memcpy(LoBuf, &hash, 4);
      PTX.TransmitPacketNB(LoBuf, LoBufSz);
      PacketInFlight = 2;
      NewLowPriorityPacket = false;
   }
}

void
PacketScheduler::SendHighPriority(char const* buf, int Size)
{
   std::memcpy(HiBuf+ReservedBufSize, buf, Size);
   // zero out the remaining
   HiBufSz = Size+ReservedBufSize;
   std::memset(HiBuf+HiBufSz, 0, 32-HiBufSz);
   HaveHighPriorityPacket = true;
   NewHighPriorityPacket = true;
   HighPriorityTimer.reset();
   HighPriorityTimer.start();
   this->Poll();
}

void
PacketScheduler::SendLowPriority(char const* buf, int Size)
{
   std::memcpy(LoBuf+ReservedBufSize, buf, Size);
   LoBufSz = Size+ReservedBufSize;
   std::memset(LoBuf+LoBufSz, 0, 32-LoBufSz);
   HaveLowPriorityPacket = true;
   NewLowPriorityPacket = true;
   LowPriorityTimer.reset();
   LowPriorityTimer.start();
   this->Poll();
}

#define MMA8451_I2C_ADDRESS (0x1d<<1)

// gyro calibration parameters

unsigned char SeqNum = 0;

void WriteSamplePacket(PacketScheduler& Scheduler,
                       std::vector<vector3<int16_t>>& AccelBuffer,
                       std::vector<int16_t>& GyroBuffer,
                       uint16_t GyroSampleCount)
{
   // assemble our packet
   char buf[32-ReservedBufSize];
   int BufSz = 4 + AccelBuffer.size()*4 + GyroBuffer.size()*2;

   buf[0] = uint8_t(AccelBuffer.size() << 4) + uint8_t(GyroBuffer.size());
   buf[1] = SeqNum++;
   std::memcpy(buf+2, &GyroSampleCount, 2);

   for (unsigned i = 0; i < AccelBuffer.size(); ++i)
   {
      std::memcpy(buf+4+i*4, &AccelBuffer[i], 4);
   }
   std::memcpy(buf+4+AccelBuffer.size()*4, GyroBuffer.data(), GyroBuffer.size()*2);

   Scheduler.SendHighPriority(buf, BufSz);

   AccelBuffer.clear();
   GyroBuffer.clear();
}

// write a status packet, including temperature information and charging info
void WriteStatusPacket(PacketScheduler& Scheduler, bool CoilDetect, bool Charging, int8_t Temp)
{
   char buf[32-ReservedBufSize];
   buf[0] = 0x88 | (uint8_t(CoilDetect) << 5) | (uint8_t(Charging) << 4);
   *static_cast<uint16_t*>(static_cast<void*>(buf+1)) = UniqueID16;
   *static_cast<uint16_t*>(static_cast<void*>(buf+3)) = 100;   // accel ODR
   *static_cast<uint16_t*>(static_cast<void*>(buf+5)) = 760;  // gyro ODR
   *static_cast<uint8_t*>(static_cast<void*>(buf+7)) = 100;   // gyro BW
   *static_cast<int8_t*>(static_cast<void*>(buf+8)) = Temp;   // gyro temperature
   Scheduler.SendLowPriority(buf, 9);
}

// write a status packet, including temperature information and charging info
void WriteStatusPacket(PacketScheduler& Scheduler, bool CoilDetect, bool Charging, int8_t Temp,
                       float BatteryCharge, bool PrepareSleep)
{
   char buf[32-ReservedBufSize];
   buf[0] = 0x8C | (uint8_t(CoilDetect) << 5) | (uint8_t(Charging) << 4) | (uint8_t(PrepareSleep) << 1);
   *static_cast<uint16_t*>(static_cast<void*>(buf+1)) = UniqueID16;
   *static_cast<uint16_t*>(static_cast<void*>(buf+3)) = 100;   // accel ODR
   *static_cast<uint16_t*>(static_cast<void*>(buf+5)) = 760;  // gyro ODR
   *static_cast<uint8_t*>(static_cast<void*>(buf+7)) = 100;   // gyro BW
   *static_cast<int8_t*>(static_cast<void*>(buf+8)) = Temp;   // gyro temperature
   *static_cast<uint16_t*>(static_cast<void*>(buf+9)) = uint16_t(BatteryCharge * 1000);
   Scheduler.SendLowPriority(buf, 11);
}

// write a status packet when we are currently in a sleep state
void WriteSleepingStatusPacket(PacketScheduler& Scheduler, bool CoilDetect, bool Charging, float BatteryCharge)
{
   char buf[32-ReservedBufSize];
   buf[0] = 0x86 | (uint8_t(CoilDetect) << 5) | (uint8_t(Charging) << 4);
   *static_cast<uint16_t*>(static_cast<void*>(buf+1)) = UniqueID16;
   *static_cast<uint16_t*>(static_cast<void*>(buf+3)) = 100;   // accel ODR
   *static_cast<uint16_t*>(static_cast<void*>(buf+5)) = 760;  // gyro ODR
   *static_cast<uint8_t*>(static_cast<void*>(buf+7)) = 100;   // gyro BW
   *static_cast<uint16_t*>(static_cast<void*>(buf+8)) = uint16_t(BatteryCharge*1000);
   Scheduler.Wakeup();
   Scheduler.SendLowPriority(buf, 10);
   Scheduler.Sleep(5);  // a few attempts at sending the packet
}

void SetupAccelerometer(MMA8451Q& acc)
{
   acc.reset();
   acc.set_range(2);
   acc.set_low_noise_mode(true);
   acc.set_sampling_rate(MMA8451Q::RATE_50);
   acc.set_sampling_rate(MMA8451Q::RATE_100);
   //   acc.set_oversampling_mode(MMA8451Q::OSMODE_HighResolution);
   acc.set_oversampling_mode(MMA8451Q::OSMODE_Normal);
   acc.set_int_data_ready_pin(2);
   acc.set_int_polarity(true);
   acc.set_int_data_ready(true);
   acc.set_active(true);
   //acc.set_active(false);
   if (acc.OK())
   {
      printf("Accelerometer initialized.\r\n");
   }
   else
   {
      MyLed->magenta();
      printf("Accelerometer initialization failed!\r\n");
   }
}

volatile bool WakeOnMotion = false;

// minimum battery charge; below this, we force deep sleep mode
constexpr float BattMinimum = 3.1;
constexpr float BattMinimumHysteresis = 3.2;

// if we measure the battery charge < BattMinimum for BatMinumumCountThreshold times in a row,
// then force emergency sleep mode
constexpr int BatMinimumCountThreshold = 10;
int BattMinimumCount = 0;

// interrupt function when we wake from sleep.  This doesn't need to do anything
void WakeFromSleepINT()
{
   MyLed->blue();
   WakeOnMotion = true;
}

void SleepMode(PacketScheduler& Scheduler, MMA8451Q& Acc, GyroInterface<SPI>& Gyro,
               RGBLED& led, bool AboveThreshold,
               DigitalOut& BatteryDetectEnable, AnalogIn& AnalogBattery,
               DigitalIn& CoilDetectBar, DigitalOut& ChargeEnable, bool Emergency = false)
{
   // Basic strategy for sleep mode:
   // We can seep when charging, or not charging.  We assume this is set up before
   // calling this function, so we don't attempt to manipulate the charging state, just
   // leave it as is.  But we can use CoilDetect as an interrput status, so we wake up if this changes.
   // (In fact, the only situation we care about is if power is attached, that is, CoilDetectBar
   // transitions from high to low.).

   // There are two approaches to using the accelerometer to wake up.  if the bell is set, then
   // the X axis is going to be slightly off zero, ie at sin(stay_position).  Hence we can do a zero-detect
   // (well, below threshold, AboveThreshold parameter == false)
   // on the X axis to wake up when the bell gets near the balance point.
   // On the other hand, if the bell is down, then the X axis should be reading very close to zero, and we can
   // use an above-threshold reading to detect that the bell is being raised (AboveThreshold == true).
   // The threshold is measured in units of acceleration in meters per second^2.

   // flash the led to indicate that we are sleeping
   // Turn off the nRF24L01
   // Turn off the gyro
   // Set the accelerometer to low power mode with interrupts enabled
   // turn off the led

   // An improvement would be to set an interrupt on the CoilDetectBar.
   // But in the current circuit it is on GPIO block E, and only blocks A and D can be used
   // for interrupts. So instead we just detect it when we periodically wake up.

   // TODO: put the accelerometer into sleep mode

   WakeOnMotion = false;
   InterruptIn AccINT1(PTA14);
   AccINT1.mode(PullNone);

   // Turn off the nRF24L01
   Scheduler.Sleep();

   // Turn off the gyro
   Gyro.Sleep();

   // ** Need to deactivate the accelerometer before changing settings
   Acc.set_active(false);

   // Set the accelerometer to low power mode with interrupts enabled
   Acc.set_sleep_oversampling_mode(MMA8451Q::OSMODE_LowPower);
   Acc.set_int_data_ready(false);
   // Motion threshold 1.  Each unit corresponds to (roughly) 0.063 radians = 3.6 degrees
   Acc.set_motion_detect_threshold(AboveThreshold ? 2 : 1);
   Acc.enable_int_motion_sleep(true);
   Acc.set_debounce_time(1);
   Acc.enable_motion_detect(AboveThreshold, true, false, false);

   // if we're in emergency sleep mode, don't set the accelerometer interrupt
   if (!Emergency)
   {
      AccINT1.rise(&WakeFromSleepINT);
      Acc.set_int_motion_detect(true);
   }
   Acc.set_active(true);

   // turn off the led
   led.off();

   bool Charging = false;

   while (!WakeOnMotion)
   {

      // See if we want to turn on or off the charger
      // and flash the led
      if (Emergency)
      {
         if (Charging)
            led.magenta();
         else
            led.red();
      }
      else if (Charging)
         led.green();
      else
         led.blue();

//      led.green();
      float BatteryCharge = MeasureBatteryCharge(BatteryDetectEnable, AnalogBattery);
      // see if we want to enter charging mode
      if (CoilDetectBar == 0 && BatteryCharge <= Vthreshold)
      {
         Charging = true;
         ChargeEnable = 1;
      }
      if (CoilDetectBar == 1)
      {
         Charging = false;
         ChargeEnable = 0;
      }
      WriteSleepingStatusPacket(Scheduler, CoilDetectBar == 0, Charging, BatteryCharge);
      led.off();

      // if the battery charge is above the emergency threshold then turn on the interrupt
      if (BatteryCharge > BattMinimumHysteresis)
      {
         Emergency = false;
         Acc.set_active(false);
         AccINT1.rise(&WakeFromSleepINT);
         Acc.set_int_motion_detect(true);
         Acc.set_active(true);
      }
      else if (BatteryCharge < BattMinimum)
      {
         Emergency = true;
         WakeOnMotion = false;
         Acc.set_active(false);
         AccINT1.rise(NULL);
         Acc.set_int_motion_detect(false);
         Acc.set_active(true);
      }

      // Go to sleep
      WakeUp::set(30);
      if (!WakeOnMotion)
         deepsleep();
      WakeUp::set(0);

      // for debugging
      //      if (SleepCycles > 1)
      //         WakeFlag = true;
   }

   // Turn on the led
   led.white();

   // Disable charging
   ChargeEnable = 0;

   // turn on the gyro
   Gyro.Wakeup();

   led.cyan();

   // turn off acceleromter interrupt
   AccINT1.rise(NULL);

   // turn on the nRF24L01
   Scheduler.Wakeup();

   led.magenta();

   // reset the accelerometer settings
   SetupAccelerometer(Acc);

   led.yellow();

   // delay for the gyro to turn on,
   // it will probably take longer than this
   wait_ms(100);
   Gyro.ClearData();
}

// buffers for accelerometer and gyro data

std::vector<vector3<int16_t>> AccelBuffer;
std::vector<int16_t> GyroBuffer;

// maximum sizes that we can send in a single packet - only used to reserve buffer sizes
int const MaxAccelSamples = 4;
int const MaxGyroSamples = 14;

// once we've written this many bytes, send the packet.  This is obtained from the maximum
// buffer size (32 - ReservedBufSize) minus the overhead (2 bytes) minus the size of an accelerometer sample (4 bytes)
// minus 2-byte sequence number
int const PacketWatermark = 32 - ReservedBufSize - 2 - 4 - 2;
//int const PacketWatermark = 1;

// Thresholds for going to sleep.  These should correspond with the
// accelerometer thresholds in the SleepMode() function
// TODO: they probably do not
// if the bell is down, then treat motion larger than 2 degrees
// enough to wake up the sensor
float ZeroMotionThreshold = 2 * pi / 180;

// if the bell is up, then treat motion closer to zero than 4 degrees
// as enough to keep the sensor awake. This is in units of g.
float StayMotionThreshold = 4 * pi / 180;

float SleepDelaySeconds = 900; // start going to sleep after this many seconds
float SleepExtraTime = 10; // go to sleep after this many additional seconds

// For controlling the led via the gyro, we need to scale to the range [0,1],
// hence need the maximum deflection.  Pick a sensible starting value.
int16_t GyroMaxDeflection = 5000;

// for a fancy LED output, we do our own zero calibration of the gyro
constexpr int16_t GyroZeroMaxDeviation = 100;
constexpr int16_t GyroZeroRequiredSamples = 800;
int16_t GyroMin = INT16_MAX;
int16_t GyroMax = INT16_MIN;
int16_t GyroOffset = 0;
int GyroZeroCount = 0;
int32_t GyroAccumulator = 0;
float LedColor;

int main()
{
   AccelBuffer.reserve(MaxAccelSamples);
   GyroBuffer.reserve(MaxGyroSamples);

   // Digital inputs for address and channel selection
   DigitalIn Addr0(PTE22, PullUp);
   DigitalIn Addr1(PTE23, PullUp);

   DigitalIn Ch0(PTE29, PullUp);
   DigitalIn Ch1(PTE30, PullUp);

   AnalogIn AnalogBattery(PTB0);
   DigitalIn CoilDetectBar(PTE5, PullNone);
   DigitalOut BatteryDetectEnable(PTE21);
   DigitalOut ChargeEnable(PTE20);

   // Sleep inhibit
   DigitalOut SleepInhibitSupply(PTC3);
   DigitalIn SleepInhibitBar(PTC0, PullUp);
   SleepInhibitSupply = 10;  // put 0 on the supply, and if there is a jumper then
   // we will detect 0 on SleepInhibitBar

   BatteryDetectEnable = 0;

   RGBLED led(LED_RED, LED_GREEN, LED_BLUE);
   MyLed = &led;


   unsigned Addr = (Addr1.read()<<1) + Addr0.read();

   uint64_t PipeAddrs[4][4] = {{0xe7e7e7e7e7ULL, 0xe7e7e7e70fULL, 0xe7e7e7e717ULL, 0xe7e7e7e72cULL},
                               {0x0f0f0f0fe7ULL, 0x0f0f0f0f0fULL, 0x0f0f0f0f17ULL, 0x0f0f0f0f2cULL},
                               {0x7e7e7ef0e7ULL, 0x7e7e7ef00fULL, 0x7e7e7ef017ULL, 0x7e7e7ef02cULL},
                               {0xf0f0f0f0e7ULL, 0xf0f0f0f00fULL, 0xf0f0f0f017ULL, 0xf0f0f0f02cULL}};


   unsigned Channel = (Ch1.read()<<1) + Ch0.read();

   unsigned Channels[4] = {76, 82, 70, 0};

   printf("Initializing.\r\n");

   Timer LedTimer;
   LedTimer.start();

   Timer GyroTempTimer;
   GyroTempTimer.start();

   I2C i2c0(PTE25, PTE24);
   i2c0.frequency(1000000);
   MMA8451Q acc(i2c0);

   //GyroInterface<I2C> Gyro(PTE0, PTE1, 0, PTA5);

   GyroInterface<SPI> Gyro(PTE1, PTE3, PTE2, PTE4, PTA5, PTA4);
   //   Gyro.device().SetFrequency(1000000);
   //   Gyro.SetRateBandwidth(L3G_H_ODR_200_70);
   //   Gyro.SetRateBandwidth(L3G_H_ODR_800_30);
   Gyro.SetRateBandwidth(L3G_H_ODR_800_110);

   uint16_t GyroSampleCount = 0;


   nRF24L01P Device(PTD2, PTD3, PTC5, PTD5, 8000000);
   nRF24L01P_PTX PTX(Device, PTA13, PTD0);

   PTX.Initialize();
   PTX.SetDataRate(2000);
   PTX.SetDestinationAddress(PipeAddrs[Channel][Addr]);
   PTX.SetChannel(Channels[Channel]);
   Device.set_retransmit_attempts(0);
   Device.set_crc_width(2);
   Device.set_tx_power(0);
   PTX.EnableDynamicPayload(0);  // enable dynamic payload on pipe 0
   PTX.PowerUp();

   PacketScheduler Scheduler(PTX);

   //PTX.EnableStreamMode();

   DigitalIn AccINT1(PTA14);
   DigitalIn AccINT2(PTA15);
   SetupAccelerometer(acc);
   bool AccActive = true;

   if (Gyro.Initialize())
   {
      printf("Gyro initialization successful.\r\n");
      Gyro.EnableZ();
      led.green();
   }
   else
   {
      printf("Gyro initialization failed!\r\n");
      led.red();
   }

   // we need to wait a bit so we initialize the gyro properly anyway,
   // so put on a light show

   led.red();     wait_ms(500);
   led.green();   wait_ms(500);
   led.blue();    wait_ms(500);
   led.cyan();    wait_ms(500);
   led.yellow();  wait_ms(500);
   led.magenta(); wait_ms(500);
   led.white();   wait_ms(500);
   WakeUp::calibrate();

   led.off();

   // To detect if we should go to sleep, we have two possibilities.  If the
   // bell is down, then the X component of the acceleration should be zero.
   // So we can detect for a non-zero X, via the ZeroMotionThreshold.
   // If the bell spends more than ??? minutes within the ZeroMotionThreshold, then
   // sleep.
   Timer ZeroMotionDetectTimer;
   ZeroMotionDetectTimer.start();

   // Alternatively, if the bell is set then the X component of the acceleration will
   // be non-zero and stable.  If the X acceleration hasn't changed sign and is
   // consistently larger than StayMotionThreshold then sleep.
   Timer StayMotionDetectTimer;
   StayMotionDetectTimer.start();
  int AccLastSign = 1;

   bool CurrentlyPreparingSleep = false;

   // for flashing the led in preparation for sleep
   bool BlinkState = false;

   // Clear the gyro data to make sure we don't have strange readings.
   // If the gyro FIFO is full, then we end up sending several packets
   // in quick succession, which confuses the dejitter algorithm.
   Gyro.ClearData();

   while (true)
   {
      if (AccINT2)
      {
         MMA8451Q::vector Data;
         acc.readAll(Data);

         AccelBuffer.push_back(Data);

         // Get X acceleration in units of g
         float Xaccel = Data[0] * (2.0 / 32768.0);
         if (std::abs(Xaccel) > ZeroMotionThreshold)
         {
            ZeroMotionDetectTimer.reset();
         }
         int Sign = (Xaccel > 0) ? 1 : -1;
         if ((Sign != AccLastSign) || (std::abs(Xaccel) < StayMotionThreshold))
         {
            StayMotionDetectTimer.reset();
         }
         AccLastSign = Sign;
      }

      // Are we preparing to sleep?
      bool ZeroMotionTimedOut = ZeroMotionDetectTimer.read() > SleepDelaySeconds;
      bool StayMotionTimedOut = StayMotionDetectTimer.read() > SleepDelaySeconds;

      if (SleepInhibitBar == 0)
      {
         ZeroMotionTimedOut = false;
         StayMotionTimedOut = false;
      }

      CurrentlyPreparingSleep = ZeroMotionTimedOut || StayMotionTimedOut;

      if (CurrentlyPreparingSleep)
      {
         if (ZeroMotionTimedOut)
         {
            bool LedOn = fmod(ZeroMotionDetectTimer.read() - SleepDelaySeconds, 0.4) < 0.2;
            if (LedOn && !BlinkState)
            {
               led.yellow();
               BlinkState = true;
            }
            if (!LedOn && BlinkState)
            {
               led.off();
               BlinkState = false;
            }
         }
         else if (StayMotionTimedOut)
         {
            bool LedOn = fmod(StayMotionDetectTimer.read() - SleepDelaySeconds, 0.4) < 0.2;
            if (LedOn && !BlinkState)
            {
               led.magenta();
               BlinkState = true;
            }
            if (!LedOn && BlinkState)
            {
               led.off();
               BlinkState = false;
            }
         }
      }

      // do we nede to write the packet?
      if ((AccelBuffer.size()*4 + GyroBuffer.size()*2) > PacketWatermark)
      {
         WriteSamplePacket(Scheduler, AccelBuffer, GyroBuffer, GyroSampleCount);
      }

      if (Gyro.DataAvailable())
      {
         int16_t z;
         int r = Gyro.ReadZ(z);
         if (r == 0)
         {
            GyroBuffer.push_back(z);
            ++GyroSampleCount;

            GyroMin = std::min(GyroMin, z);
            GyroMax = std::max(GyroMax, z);

            if (std::abs(GyroMax-GyroMin) <= GyroZeroMaxDeviation)
            {
               GyroAccumulator += z;
               ++GyroZeroCount;

               if (GyroZeroCount >= GyroZeroRequiredSamples)
               {
                  GyroOffset =  GyroAccumulator / GyroZeroCount;
                  //SetupAccelerometer(acc);
                  acc.set_active(true);
                  AccActive = true;
               }
            }
            else if (AccActive)
            {
               acc.set_active(false);
               AccActive = false;
            }

            if (std::abs(GyroMax-GyroMin) > GyroZeroMaxDeviation || GyroZeroCount >= GyroZeroRequiredSamples)
            {
               // reset the counter
               GyroMin = INT16_MAX;
               GyroMax = INT16_MIN;
               GyroAccumulator = 0;
               GyroZeroCount = 0;
            }

            if (!CurrentlyPreparingSleep)
            {
               float g = z - GyroOffset;
               g = 1e-6 * std::sqrt(std::abs(g));

               LedColor += g;
               while (LedColor > 1)
                  LedColor -= 1;
               led.write_hsv(LedColor, 1.0, 1.0);
            }
         }
         else
         {
            printf("Gyro read failed!\r\n");
            led.white();
         }
      }

      // do we nede to write the packet?
      if ((AccelBuffer.size()*4 + GyroBuffer.size()*2) > PacketWatermark)
      {
         WriteSamplePacket(Scheduler, AccelBuffer, GyroBuffer, GyroSampleCount);
      }

      if (GyroTempTimer.read_ms() > 1100)
      {
         float Charge = MeasureBatteryCharge(BatteryDetectEnable, AnalogBattery);
         WriteStatusPacket(Scheduler, CoilDetectBar == 0, false, Gyro.device().TempRaw(),
                           Charge, CurrentlyPreparingSleep);
         if (Charge < BattMinimum)
         {
            if (++BattMinimumCount > BatMinimumCountThreshold)
            {
               SleepMode(Scheduler, acc, Gyro,led, false,
                         BatteryDetectEnable, AnalogBattery, CoilDetectBar, ChargeEnable, true);
            }
         }
         else
            BattMinimumCount = 0;
         GyroTempTimer.reset();
      }

      Scheduler.Poll();

      // should we sleep?
      if ((ZeroMotionTimedOut && ZeroMotionDetectTimer.read() > SleepDelaySeconds+SleepExtraTime)
          || (StayMotionTimedOut && StayMotionDetectTimer.read() > SleepDelaySeconds+SleepExtraTime))
      {
         // why are we sleeping?  If the bell is down, and we've satisfied
         // the zero motion condition, then detect motion that is above threshold.
         bool AboveThreshold = ZeroMotionDetectTimer.read() > (SleepDelaySeconds+SleepExtraTime);
         SleepMode(Scheduler, acc, Gyro,led, AboveThreshold,
                   BatteryDetectEnable, AnalogBattery, CoilDetectBar, ChargeEnable);

         ZeroMotionDetectTimer.reset();
         StayMotionDetectTimer.reset();
         GyroTempTimer.reset();
         CurrentlyPreparingSleep = false;
      }

   }
}
