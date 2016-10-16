#include "mbed.h"
#include "limits.h"
#include "MMA8451Q.h"
#include "gyrointerface.h"
#include "nRF24L01P.h"
#include "nRF24L01P_PTX.h"
#include "common/matvec/matvec.h"
#include <cstdint>
#include <vector>

//
// packet format for sensor readings:
//
// delay (14 bits) seq (2 bits) |flags | seq | payload (up to 28 bytes)
// delay is a 14-bit unsigned in units of 4 microseconds - maximum value
// 16383 represents 65.532 miliseconds.
///
// flags is 8 bits: 0 AAA GGGG
// seq is a 1-byte sequentially increasing number
// delay/seq is the number of microseconds that this packet has been delayed (unsigned 16 bit)
// AAA is number of 6-byte accelerometer readings (first in the payload)
// GGGG is the number of 2-byte z-axis gyro readings
//
// Since we are not using the nRF retransmit functionality,
// on a lost ACK packet we will be unaware that the receive was successful,
// and thefore we will send the packet again.  The packet will have a different CRC,
// hence will be picked up by the receiver as a valid packet, rather than
// intercepted as a duplicate.  Hence we must use a sequence number on all packets
// (this wouldn't need to be 8 bits, it could be smaller).
// packet format after lost sensor packet:
// delay (2 bytes) | 11000000 | acc lost count (2 bytes) | gyro lost count (2 bytes) | gyro sum (4 bytes)

// but we need to indicate the start of a stream.

// packet format for status information:
// delay/seq (2 bytes) |10TCDBSX | acc ODR (2 bytes) | gyro ODR (2 bytes) | gyro bandwidth (1 byte) | payload
// if T is set, then the payload conains a 1-byte gyro temperature reading
// if C is set, the charging circuit is active, otherwise it is not active
// if D is set, the payload contains a 2-byte charging current reading
// if B is set, the payload contains a 2-byte battery current reading
// if S is set, then the sensor is preparing for sleep mode
// X = 0 is reserved for future expansion
//
// TODO: a better approach is to ditch the seq number and track sample loss due to lost packets.
//
// possible improved packet format:
// dump the seq in the sensor packet.
// measure the delay in units of 10 microseconds
// Have a new packet type for when we have packet loss.
// flags, 8 bits 0STMNXXX
// S - start of a new sequence, discard previous state
// T - a one-byte gyro temperature reading is included at the start of the payload
// M - accelerometer measurements are missing due to a lost packet,
// the playload includes a 2-byte count and a 12-byte (4 bytes per sample) cumulative sum
// N - gyro measurements are missing due to a lost packet,
// the playload includes an 2-byte count and a 4-byte cumulative sum


// a simple LCG for random number generation
uint32_t Seed = time(NULL);

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

int RandomSlot(int n)
{
   int Max = (1 << std::max(n,5));
   return random(0, Max);
}

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

      // 0 of there is no packet in flight.  1 if there is a high priority packet in flight.
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
      char HiBuf[32];
      int HiBufSz;
      char LoBuf[32];
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
      std::memcpy(HiBuf, &d, 2);
      PTX.TransmitPacketNB(HiBuf, HiBufSz);
      PacketInFlight = 1;
      NewHighPriorityPacket = false;
   }
   else if (HaveLowPriorityPacket)
   {
      uint16_t d = LowPriorityTimer.read_us();
      d = (d & 0xFFFC) | Seq;
      std::memcpy(LoBuf, &d, 2);
      PTX.TransmitPacketNB(LoBuf, LoBufSz);
      PacketInFlight = 2;
      NewLowPriorityPacket = false;
   }
}

void
PacketScheduler::SendHighPriority(char const* buf, int Size)
{
   std::memcpy(HiBuf+2, buf, Size);
   HiBufSz = Size+2;
   HaveHighPriorityPacket = true;
   NewHighPriorityPacket = true;
   HighPriorityTimer.reset();
   HighPriorityTimer.start();
   this->Poll();
}

void
PacketScheduler::SendLowPriority(char const* buf, int Size)
{
   std::memcpy(LoBuf+2, buf, Size);
   LoBufSz = Size+2;
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
   char buf[30];
   int BufSz = 2 + AccelBuffer.size()*6 + GyroBuffer.size()*2;

   buf[0] = uint8_t(AccelBuffer.size() << 4) + uint8_t(GyroBuffer.size());
   buf[1] = SeqNum++;

   std::memcpy(buf+2, AccelBuffer.data(), AccelBuffer.size()*6);
   std::memcpy(buf+2+AccelBuffer.size()*6, GyroBuffer.data(), GyroBuffer.size()*2);

   Scheduler.SendHighPriority(buf, BufSz);

   AccelBuffer.clear();
   GyroBuffer.clear();
}

void WriteStatusPacket(PacketScheduler& Scheduler)
{
   char buf[30];
   buf[0] = 0x94;
   *static_cast<uint16_t*>(static_cast<void*>(buf+1)) = 100;  // accel ODR
   *static_cast<uint16_t*>(static_cast<void*>(buf+3)) = 760;  // gyro ODR
   *static_cast<uint8_t*>(static_cast<void*>(buf+5)) = 100;   // gyro BW
   uint16_t ChV = 0;
   *static_cast<uint16_t*>(static_cast<void*>(buf+7)) = ChV;
   uint16_t BattV = 0;
   *static_cast<uint16_t*>(static_cast<void*>(buf+6)) = BattV;
   Scheduler.SendLowPriority(buf, 8);
}

// write a status packet, including temperature information and charging info
void WriteStatusPacket(PacketScheduler& Scheduler, int8_t Temp, bool CoilDetect, bool Charging)
{
   char buf[30];
   buf[0] = 0xB4;
   *static_cast<uint16_t*>(static_cast<void*>(buf+1)) = 100;  // accel ODR
   *static_cast<uint16_t*>(static_cast<void*>(buf+3)) = 760;  // gyro ODR
   *static_cast<uint8_t*>(static_cast<void*>(buf+5)) = 100;   // gyro BW
   *static_cast<int8_t*>(static_cast<void*>(buf+6)) = Temp;   // gyro temperature
   uint16_t ChV = Charging; //ChargeV.read_u16();
   *static_cast<uint16_t*>(static_cast<void*>(buf+7)) = ChV;
   uint16_t BattV = CoilDetect; //BatteryV.read_u16();
   *static_cast<uint16_t*>(static_cast<void*>(buf+9)) = BattV;
   Scheduler.SendLowPriority(buf, 11);
}

void SleepMode(nRF24L01_PTX& PTX,

// buffers for accelerometer and gyro data

std::vector<vector3<int16_t>> AccelBuffer;
std::vector<int16_t> GyroBuffer;

int const MaxAccelSamples = 4;
int const MaxGyroSamples = 14;

int const PacketWatermark = 23;
//int const PacketWatermark = 1;

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

   uint64_t PipeAddrs[4] = {0xe7e7e7e7e7ULL, 0x0f0f0f0f0fULL, 0x0f0f0f0f17ULL, 0x0f0f0f0f2cULL};

   unsigned Channel = (Ch1.read()<<1) + Ch0.read();

   printf("Initializing.\r\n");

   Timer LedTimer;
   LedTimer.start();

   Timer GyroTempTimer;
   GyroTempTimer.start();

   I2C i2c0(PTE25, PTE24);
   i2c0.frequency(1000000);
   MMA8451Q acc(i2c0);


   //GyroInterface<I2C> Gyro(PTE0, PTE1, 0, PTA5);
   GyroInterface<SPI> Gyro(PTE1, PTE3, PTE2, PTE4, PTA5);
   //   Gyro.device().SetFrequency(1000000);
   //   Gyro.SetRateBandwidth(L3G_H_ODR_200_70);
   //   Gyro.SetRateBandwidth(L3G_H_ODR_800_30);
   Gyro.SetRateBandwidth(L3G_H_ODR_800_110);


   nRF24L01P Device(PTD2, PTD3, PTD1, PTD5, 8000000);
   nRF24L01P_PTX PTX(Device, PTA13, PTD0);

   PTX.Initialize();
   PTX.SetDataRate(2000);
   PTX.SetDestinationAddress(PipeAddrs[Addr]);
   PTX.SetChannel(76 + Channel*2);
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
   acc.set_oversampling_mode(MMA8451Q::OSMODE_HighResolution);
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

      if ((AccelBuffer.size()*6 + GyroBuffer.size()*2) >= PacketWatermark)
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

      if ((AccelBuffer.size()*6 + GyroBuffer.size()*2) >= PacketWatermark)
      {
         WriteSamplePacket(Scheduler, AccelBuffer, GyroBuffer);
      }

      if (GyroTempTimer.read_ms() > 1100)
      {
         WriteStatusPacket(Scheduler, Gyro.device().TempRaw(), CoilDetectBar == 0, CoilDetectOK);
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
