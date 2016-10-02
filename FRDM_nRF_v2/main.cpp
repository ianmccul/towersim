#include "mbed.h"
#include "limits.h"
#include "MMA8451Q.h"
#include "gyrointerface.h"
#include "nRF24L01P.h"
#include "nRF24L01P_PTX.h"
#include "matvec.h"
#include <cstdint>

//
// packet format for sensor readings:
//
// flags | seq | delay (2 bytes) | payload (up to 28 bytes)
// flags is 8 bits: 0 AAA GGGG
// seq is a 1-byte sequentially increasing number
// delay is the number of microseconds that this packet has been delayed (unsigned 16 bit)
// AAA is number of 6-byte accelerometer readings (first in the payload)
// GGGG is the number of 2-byte z-axis gyro readings
//
// packet format for status information:
// 01TCDBXX | acc ODR (2 bytes) | gyro ODR (2 bytes) | gyro bandwidth (1 byte) | payload
// if T is set, then the payload conains a 1-byte gyro temperature reading
// if C is set, the charging circuit is active, otherwise it is not active
// if D is set, the payload contains a 2-byte charging current reading
// if B is set, the payload contains a 2-byte battery current reading
// XX = 00 is reserved for future expansion


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

      // true if the low priority packet in the buffer is new (we haven't attempted a transmit yet)
      bool NewLowPriorityPacket;

      // time in microseconds for each slot
      static uint16_t const SlotTime = 250;

      // number of retransmits since the last successful packet
      int NumRetransmits;

      Timer RetransmitTimer;
      int32_t RetransmitPoint;

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
     RetransmitPoint(0)
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
         if (PacketInFlight == 1 && !NewHighPriorityPacket)
         {
            HaveHighPriorityPacket = false;
            HighPriorityTimer.stop();
         }
         else if (PacketInFlight == 2 && !NewLowPriorityPacket)
         {
            HaveLowPriorityPacket = false;
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
      PTX.TransmitPacketNB(HiBuf, HiBufSz);
      PacketInFlight = 1;
      NewHighPriorityPacket = false;
   }
   else if (HaveLowPriorityPacket)
   {
      PTX.TransmitPacketNB(LoBuf, LoBufSz);
      PacketInFlight = 2;
      NewLowPriorityPacket = false;
   }
}

void
PacketScheduler::SendHighPriority(char const* buf, int Size)
{
   std::memcpy(HiBuf, buf, Size);
   HiBufSz = Size;
   HaveHighPriorityPacket = true;
   NewHighPriorityPacket = true;
   HighPriorityTimer.reset();
   HighPriorityTimer.start();
   this->Poll();
}

void
PacketScheduler::SendLowPriority(char const* buf, int Size)
{
   std::memcpy(LoBuf, buf, Size);
   LoBufSz = Size;
   HaveLowPriorityPacket = true;
   NewLowPriorityPacket = true;
   this->Poll();
}


#define MMA8451_I2C_ADDRESS (0x1d<<1)

// gyro calibration parameters

bool GyroHasZeroCalibration = false;

// maximum swing values that are still considered to be zero
int const GyroZeroMaxDeviation = 120;

// number of samples required that satisfy GyroZeroMaxDeviation
// to count as a successful zero calibration
int const GyroZeroRequiredSamples = 800;

unsigned char SeqNum = 0;

void WriteAccelPacket(PacketScheduler& s, MMA8451Q::vector const& v)
{
   int const PacketSize = 8;
   char Buffer[PacketSize];
   Buffer[0] = 0x21;
   Buffer[1] = SeqNum++;
   memcpy(Buffer+2, static_cast<void const*>(&v), 6);
   //   s.TransmitWait();
   //s.TransmitPacketNB(Buffer, PacketSize);
   s.SendHighPriority(Buffer, PacketSize);
}

void WriteGyroPacket(PacketScheduler& s, L3GTypes::vector const& v)
{
   int const PacketSize = 8;
   char Buffer[PacketSize];
   Buffer[0] = 0x42;
   Buffer[1] = SeqNum++;
   memcpy(Buffer+2, static_cast<void const*>(&v), 6);
   //   s.TransmitWait();
   //   s.TransmitPacketNB(Buffer, PacketSize);
   s.SendHighPriority(Buffer, PacketSize);
}

void WriteGyroTemp(PacketScheduler& s, int8_t T)
{
   int const PacketSize = sizeof(T)+1;
   char Buffer[PacketSize];
   Buffer[0] = 0x84;
   memcpy(Buffer+1, static_cast<void const*>(&T), sizeof(T));
   s.SendLowPriority(Buffer, PacketSize);
}

void WriteTimerPacket(PacketScheduler& s, Timer& t, char Type)
{
   int us = t.read_us();
   t.reset();
   int const PacketSize = sizeof(us)+2;
   char Buffer[PacketSize];
   Buffer[0] = 0x99;
   Buffer[1] = Type;
   memcpy(Buffer+2, static_cast<void const*>(&us), sizeof(us));
   //s.TransmitWait();
   //s.TransmitPacketNB(Buffer, PacketSize);
   s.SendHighPriority(Buffer, PacketSize);
}

int main()
{
   PwmOut rled(LED_RED);
   PwmOut gled(LED_GREEN);
   PwmOut bled(LED_BLUE);

   rled = 0.0;
   gled = 1.0;
   bled = 1.0;

   Timer timer;
   timer.start();

   wait_ms(500);   // wait a bit so we initialize the gyro properly

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
   PTX.SetChannel(76);
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

   while (true)
   {
      if (AccINT2)
      {
         //WriteTimerPacket(PTX, timer, 0x21);
         MMA8451Q::vector Data;
         acc.readAll(Data);
#if 0
         if (LedTimer.read() > 1)
         {
            rled = 1.0 - abs(Data.x)/32768.0;
            gled = 1.0 - abs(Data.y)/32768.0;
            bled = 1.0 - abs(Data.z)/32768.0;
            LedTimer.reset();
         }
#endif

         //printf(".");
         WriteAccelPacket(Scheduler, Data);
      }

      if (Gyro.DataAvailable())
      {
         //L3G::FIFOStatus Stat = Gyro.device().GetFIFOStatus();
         //WriteTimerPacket(PTX, timer, Stat.Reg);
         L3GTypes::vector v;
         int r = Gyro.Read(v);
         if (r == 0)
         {
            WriteGyroPacket(Scheduler, v);
            //rled = 0.0;
            //gled = 0.0;
            //bled = 0.0;
         }
         else
            printf("Gyro read failed!\r\n");
      }

      if (GyroTempTimer.read() > 10)
      {
         WriteGyroTemp(Scheduler, Gyro.device().TempRaw());
         GyroTempTimer.reset();
      }

      Scheduler.Poll();
   }
}
