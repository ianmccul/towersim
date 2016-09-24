#include "mbed.h"
#include "limits.h"
#include "MMA8451Q.h"
#include "gyrointerface.h"
#include "nRF24L01P.h"
#include "nRF24L01P_PTX.h"
#include "matvec.h"

#define MMA8451_I2C_ADDRESS (0x1d<<1)

// gyro calibration parameters

bool GyroHasZeroCalibration = false;

// maximum swing values that are still considered to be zero
int const GyroZeroMaxDeviation = 120;

// number of samples required that satisfy GyroZeroMaxDeviation
// to count as a successful zero calibration
int const GyroZeroRequiredSamples = 800;

unsigned char SeqNum = 0;

void WriteAccelPacket(nRF24L01P_PTX& s, MMA8451Q::vector const& v)
{
   int const PacketSize = 8;
   char Buffer[PacketSize];
   Buffer[0] = 0x21;
   Buffer[1] = SeqNum++;
   memcpy(Buffer+2, static_cast<void const*>(&v), 6);
   //   s.TransmitWait();
   //s.TransmitPacketNB(Buffer, PacketSize);
   s.StreamPacket(Buffer, PacketSize);
}

void WriteGyroPacket(nRF24L01P_PTX& s, L3G::vector const& v)
{
   int const PacketSize = 8;
   char Buffer[PacketSize];
   Buffer[0] = 0x42;
   Buffer[1] = SeqNum++;
   memcpy(Buffer+2, static_cast<void const*>(&v), 6);
   //   s.TransmitWait();
   //   s.TransmitPacketNB(Buffer, PacketSize);
   s.StreamPacket(Buffer, PacketSize);
}

void WriteGyroCalibrationPacket(nRF24L01P_PTX& s, vector3<float> const& v)
{
   int const PacketSize = sizeof(v) + 2;
   char Buffer[PacketSize];
   Buffer[0] = 0x83;
   Buffer[1] = SeqNum++;
   memcpy(Buffer+2, static_cast<void const*>(&v), sizeof(v));
   //s.TransmitWait();
   //   s.TransmitPacketNB(Buffer, PacketSize);
   s.StreamPacket(Buffer, PacketSize);
}

void WriteTimerPacket(nRF24L01P_PTX& s, Timer& t, char Type)
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
   s.StreamPacket(Buffer, PacketSize);
}

vector3<short> GyroMin(SHRT_MAX), GyroMax(SHRT_MIN);
vector3<int>   GyroAccumulator(0);
int GyroCount = 0;

// Zero offset calibration of the gyro
vector3<float> GyroOffset(0,0,0);
vector3<short> GyroOffsetInt(0,0,0);

bool ProcessGyroZeroOffset(nRF24L01P_PTX& s, L3G::vector const& v)
{
   GyroMin = min(GyroMin, v);
   GyroMax = max(GyroMax, v);
   if (norm_inf(GyroMax-GyroMin) > GyroZeroMaxDeviation)
   {
      // reset the counter
      GyroMin.fill(SHRT_MAX);
      GyroMax.fill(SHRT_MIN);
      GyroAccumulator.fill(0);
      GyroCount = 0;
      return false;
   }

   GyroAccumulator += v;
   ++GyroCount;
   if (GyroCount >= GyroZeroRequiredSamples)
   {
      // we have calibration, set the GyroOffset
      if (GyroHasZeroCalibration)
      {
         // filter step
         GyroOffset += GyroAccumulator /float(GyroCount);
         GyroOffset = GyroOffset / 2;
         GyroOffsetInt = vector3<short>(round(GyroOffset));
         WriteGyroCalibrationPacket(s, GyroOffset);
      }
      else
      {
         GyroOffset = GyroAccumulator / float(GyroCount);
         GyroOffsetInt = vector3<short>(round(GyroOffset));
         GyroHasZeroCalibration = true;
         WriteGyroCalibrationPacket(s, GyroOffset);
      }
      // reset the counter
      GyroMin.fill(SHRT_MAX);
      GyroMax.fill(SHRT_MIN);
      GyroAccumulator.fill(0);
      GyroCount = 0;
      return true;
   }

   return false;
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

   I2C i2c0(PTE25, PTE24);
   i2c0.frequency(1000000);
   MMA8451Q acc(i2c0);


   I2C i2c1(PTE0, PTE1);
   i2c1.frequency(1000000);
   GyroInterface Gyro(i2c1, 0, PTA5);
   //   Gyro.SetRateBandwidth(L3G_H_ODR_200_70);
   //   Gyro.SetRateBandwidth(L3G_H_ODR_800_30);
   Gyro.SetRateBandwidth(L3G_H_ODR_800_110);


   nRF24L01P Device(PTD2, PTD3, PTD1, PTD5, 8000000);
   nRF24L01P_PTX PTX(Device, PTA13, PTD0);

   PTX.Initialize();
   PTX.SetDataRate(2000);
   PTX.SetChannel(76);
   Device.set_retransmit_attempts(2);
   Device.set_crc_width(2);
   Device.set_tx_power(0);
   PTX.EnableDynamicPayload(0);  // enable dynamic payload on pipe 0
   PTX.PowerUp();
   PTX.EnableStreamMode();

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
         WriteAccelPacket(PTX, Data);
      }

      if (Gyro.DataAvailable())
      {
         //L3G::FIFOStatus Stat = Gyro.device().GetFIFOStatus();
         //WriteTimerPacket(PTX, timer, Stat.Reg);
         L3G::vector v;
         int r = Gyro.Read(v);
         if (r == 0)
         {
            if (!GyroHasZeroCalibration)
            {
               ProcessGyroZeroOffset(PTX, v);
               if (GyroHasZeroCalibration)
               {
                  rled = 1.0;
                  gled = 0.0;
                  bled = 1.0;
               }
            }
            else
               ProcessGyroZeroOffset(PTX, v);

            WriteGyroPacket(PTX, v-GyroOffsetInt);
            //rled = 0.0;
            //gled = 0.0;
            //bled = 0.0;
         }
         else
            printf("Gyro read failed!\r\n");
      }
   }
}
