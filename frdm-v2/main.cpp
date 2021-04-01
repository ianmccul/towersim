#include "mbed.h"
#include "icm20602_spi.h"

Serial pc(USBTX, USBRX);

// SPI interface
PinName mosi_pin = PTD3;
PinName miso_pin = PTD2;
PinName sck_pin = PTD1;
PinName csn_pin = PTD0;

int main()
{
   pc.printf("Initialising SPI\r\n");

   ICM20602 Device(mosi_pin, miso_pin, sck_pin, csn_pin);

   pc.printf("OK?\r\n");
   bool b = Device.OK();

   pc.printf("OK returned %d\r\n", b);

   Device.init();

   pc.printf("Turning on gyro X\r\n");
   Device.EnableGyroX();

   pc.printf("turning off sleep mode\r\n");
   Device.sleep(false);

   while(1)
   {
      int16_t x = Device.ReadGyroX();
      pc.printf("gyro x = %d\r\n", x);
   }
}
