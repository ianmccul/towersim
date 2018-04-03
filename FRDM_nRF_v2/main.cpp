#include "mbed.h"
#include "limits.h"
#include "MMA8451Q.h"
#include "gyrointerface.h"
#include "nRF24L01P.h"
#include "nRF24L01P_PTX.h"
#include "common/matvec/matvec.h"
#include "uid.h"
#include <cstdint>
#include <vector>

//
// packet format for sensor readings:
// Multi-byte sequences are in little-endian format, which is the default
// for the KL25z and also the raspberry pi.
//
// hash (4 bytes) | delay (14 bits) seq (2 bits) |flags | seq1 | payload (up to 28 bytes)
// hash is the FNV-1a-32 hash of the remaining bytes in the packet (with extra bytes set to zero to make
// 32 bytes in total, even though we don't transmit extra bytes)
// delay is a 14-bit unsigned in units of 4 microseconds - maximum value
// 16383 represents 65.532 miliseconds.
//
// flags is 8 bits: 0 AAA GGGG
// seq1 is a 1-byte sequentially increasing number
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

class PacketScheduler
{
   public:
      PacketScheduler(nRF24L01P_PTX& PTX_);

      // Turns on the device and initializes it
      //void PowerOn();

      // Turns off the device
      //void PowerOff();

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
                       std::vector<int16_t>& GyroBuffer)
{
   // assemble our packet
   char buf[32-ReservedBufSize];
   int BufSz = 2 + AccelBuffer.size()*6 + GyroBuffer.size()*2;

   buf[0] = uint8_t(AccelBuffer.size() << 4) + uint8_t(GyroBuffer.size());
   buf[1] = SeqNum++;

   std::memcpy(buf+2, AccelBuffer.data(), AccelBuffer.size()*6);
   std::memcpy(buf+2+AccelBuffer.size()*6, GyroBuffer.data(), GyroBuffer.size()*2);

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
   *static_cast<uint16_t*>(static_cast<void*>(buf+3)) = 100;  // accel ODR
   *static_cast<uint16_t*>(static_cast<void*>(buf+5)) = 760;  // gyro ODR
   *static_cast<uint8_t*>(static_cast<void*>(buf+7)) = 100;   // gyro BW
   *static_cast<int8_t*>(static_cast<void*>(buf+8)) = Temp;   // gyro temperature
   Scheduler.SendLowPriority(buf, 9);
}

// write a status packet, including temperature information and charging info
void WriteStatusPacket(PacketScheduler& Scheduler, bool CoilDetect, bool Charging, int8_t Temp,
                       uint16_t BatteryCharge)
{
   char buf[32-ReservedBufSize];
   buf[0] = 0x8C | (uint8_t(CoilDetect) << 5) | (uint8_t(Charging) << 4);
   *static_cast<uint16_t*>(static_cast<void*>(buf+1)) = UniqueID16;
   *static_cast<uint16_t*>(static_cast<void*>(buf+3)) = 100;  // accel ODR
   *static_cast<uint16_t*>(static_cast<void*>(buf+5)) = 760;  // gyro ODR
   *static_cast<uint8_t*>(static_cast<void*>(buf+7)) = 100;   // gyro BW
   *static_cast<int8_t*>(static_cast<void*>(buf+8)) = Temp;   // gyro temperature
   *static_cast<uint16_t*>(static_cast<void*>(buf+9)) = BatteryCharge;
   Scheduler.SendLowPriority(buf, 11);
}

void SleepMode(nRF24L01P_PTX& PTX)
{
}

// buffers for accelerometer and gyro data

std::vector<vector3<int16_t>> AccelBuffer;
std::vector<int16_t> GyroBuffer;

// maximum sizes that we can send in a single packet - only used to reserve buffer sizes
int const MaxAccelSamples = 4;
int const MaxGyroSamples = 14;

// once we've written this many bytes, send the packet.  This is obtained from the maximum
// buffer size (32 - ReservedBufSize) minus the overhead (2 bytes) minus the size of an accelerometer sample (6 bytes)
int const PacketWatermark = 32 - ReservedBufSize - 2 - 6;
//int const PacketWatermark = 1;

uint16_t MeasureBatteryCharge(DigitalOut& BatteryDetectEnable, AnalogIn& AnalogBattery)
{
   BatteryDetectEnable = 1;
   wait_us(20);
   uint16_t x = AnalogBattery.read_u16();
   BatteryDetectEnable = 0;
   return x;
}

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

   BatteryDetectEnable = 0;
   ChargeEnable = 0;
   int const CoilDetectTimeDelay = 20000;  // milliseconds
   bool CoilDetectOK = false;  // true if the coil is enabled for more than 20 seconds
   Timer CoilDetectTimer;
   CoilDetectTimer.reset();
   CoilDetectTimer.start();

   PwmOut rled(LED_RED);
   PwmOut gled(LED_GREEN);
   PwmOut bled(LED_BLUE);

   rled = 0.0;
   gled = 1.0;
   bled = 1.0;

   Timer timer;
   timer.start();

   wait_ms(500);   // wait a bit so we initialize the gyro properly

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


   nRF24L01P Device(PTD2, PTD3, PTD1, PTD5, 8000000);
   nRF24L01P_PTX PTX(Device, PTA13, PTD0);

   PTX.Initialize();
   PTX.SetDataRate(2000);
   PTX.SetDestinationAddress(PipeAddrs[Addr][Channel]);
   PTX.SetChannel(Channels[Channel]);
   Device.set_retransmit_attempts(0);
   Device.set_crc_width(2);
   Device.set_tx_power(0);
   PTX.EnableDynamicPayload(0);  // enable dynamic payload on pipe 0
   PTX.PowerUp();

   PacketScheduler Scheduler(PTX);

   //PTX.EnableStreamMode();

   DigitalIn AccINT2(PTA15);

   acc.reset();
   acc.set_range(2);
   acc.set_low_noise_mode(true);
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
      printf("Accelerometer initialization failed!\r\n");
   }

   if (Gyro.Initialize())
   {
      printf("Gyro initialization successful.\r\n");
   }
   else
   {
      printf("Gyro initialization failed!\r\n");
   }

   rled = 1.0;
   gled = 0.0;
   bled = 1.0;

   while (true)
   {
      if (AccINT2)
      {
         MMA8451Q::vector Data;
         acc.readAll(Data);

         AccelBuffer.push_back(Data);
      }

      if ((AccelBuffer.size()*6 + GyroBuffer.size()*2) > PacketWatermark)
      {
         WriteSamplePacket(Scheduler, AccelBuffer, GyroBuffer);
      }

      if (Gyro.DataAvailable())
      {
         L3GTypes::vector v;
         int r = Gyro.Read(v);
         if (r == 0)
         {
            GyroBuffer.push_back(v[2]);
         }
         else
            printf("Gyro read failed!\r\n");
      }

      if ((AccelBuffer.size()*6 + GyroBuffer.size()*2) > PacketWatermark)
      {
         WriteSamplePacket(Scheduler, AccelBuffer, GyroBuffer);
      }

      if (GyroTempTimer.read_ms() > 1100)
      {
         uint16_t Charge = MeasureBatteryCharge(BatteryDetectEnable, AnalogBattery);
         WriteStatusPacket(Scheduler, CoilDetectBar == 0, CoilDetectOK, Gyro.device().TempRaw(),
                           Charge);
         GyroTempTimer.reset();
      }

      Scheduler.Poll();

      // CoilDetectBar active low
      if (CoilDetectBar == 0)
      {
         if (!CoilDetectOK && CoilDetectTimer.read_ms() > CoilDetectTimeDelay)
         {
            // start the charging circuit
            CoilDetectOK = true;
            ChargeEnable = true;
         }
      }
      else
      {
         CoilDetectTimer.reset();
         if (CoilDetectOK)
         {
            CoilDetectOK = false;
            ChargeEnable = false;
         }
      }

   }
}
