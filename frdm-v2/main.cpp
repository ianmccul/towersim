#include "mbed.h"
#include "icm20602_spi.h"

Serial pc(USBTX, USBRX);

// SPI interface
PinName mosi_pin = PTD3;
PinName miso_pin = PTD2;
PinName sck_pin = PTD1;
PinName csn_pin = PTD0;

uint8_t fifo[1008];

int main()
{
   pc.baud(19200);
   pc.printf("Initialising SPI\r\n");

   ICM20602 Device(mosi_pin, miso_pin, sck_pin, csn_pin);

   pc.printf("OK?\r\n");
   bool b = Device.OK();

   pc.printf("OK returned %d\r\n", b);

   Device.init();

   Device.SetGyroRateBandwidth(ICM20602::ODR_1K_BW_20);
   Device.SetGyroScale(500);

   pc.printf("Turning on gyro Z\r\n");
   Device.EnableGyroZ();

   Device.SetSampleRateDivider(99);

   Device.SetFIFOMode(false);  // throw away FIFO data once it is full
   Device.EnableGyroFIFO();
   Device.EnableFIFO();

   int PacketSize = 8;  // 3 gyro readings + temp

   pc.printf("turning off sleep mode\r\n");
   Device.sleep(false);

   while(1)
   {
      int FifoCount = Device.FIFOCount();
      if (FifoCount >= PacketSize)
      {
         int Packets = FifoCount / PacketSize;
         Device.ReadFIFO(fifo, Packets*PacketSize);
         int n = 0;
         for (int i = 0; i < Packets; ++i)
         {
            pc.printf("got a packet 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\r\n",
                       fifo[n], fifo[n+1], fifo[n+2], fifo[n+3], fifo[n+4], fifo[n+5], fifo[n+6], fifo[n+7]);
            n += PacketSize;
         }
      }
      wait(0.5);
//      int16_t z = Device.ReadGyroZ();
//      float t = Device.Temp();
//      float zz = z / GyroSensitivityScale[1];
//      pc.printf("gyro z = %9.5f temp = %8.5f\r\n", zz, t);
   }
}
