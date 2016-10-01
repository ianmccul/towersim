#include "l3g.h"
#include <math.h>

//
// I2C mode
//


L3G_base<I2C>::L3G_base(PinName sda, PinName scl)
   : device(L3G_DEVICE_AUTO),
     i2c(sda, scl),
     address(L3G_ADDRESS_AUTO)
{
   this->AutoDetectAddress();
}

bool
L3G_base<I2C>::AutoDetectAddress(void)
{
   // try each possible address and stop if reading WHO_AM_I returns the expected response
   address = L3G4200D_ADDRESS_SA0_LOW;
   if (readReg(L3G_WHO_AM_I) == 0xD3)
   {
      device = L3G4200D_DEVICE;
      return true;
   }
   address = L3G4200D_ADDRESS_SA0_HIGH;
   if (readReg(L3G_WHO_AM_I) == 0xD3)
   {
      device = L3G4200D_DEVICE;
      return true;
   }
   address = L3GD20_ADDRESS_SA0_LOW;
   if (readReg(L3G_WHO_AM_I) == 0xD4)
   {
      device = L3GD20_DEVICE;
      return true;
   }
   if (readReg(L3G_WHO_AM_I) == 0xD7)
   {
      device = L3GD20H_DEVICE;
      return true;
   }
   address = L3GD20_ADDRESS_SA0_HIGH;
   if (readReg(L3G_WHO_AM_I) == 0xD4)
   {
      device = L3GD20_DEVICE;
      return true;
   }
   if (readReg(L3G_WHO_AM_I) == 0xD7)
   {
      device = L3GD20H_DEVICE;
      return true;
   }
   device = L3G_DEVICE_AUTO;
   address = L3G_ADDRESS_AUTO;
   return false;
}

bool
L3G_base<I2C>::init(int Device, int sa0)
{
   device = Device;
   switch (device)
   {
   case L3G4200D_DEVICE:
      if (sa0 == L3G_SA0_LOW)
      {
         address = L3G4200D_ADDRESS_SA0_LOW;
         return true;
      }
      else if (sa0 == L3G_SA0_HIGH)
      {
         address = L3G4200D_ADDRESS_SA0_HIGH;
         return true;
      }
      else
         return this->AutoDetectAddress();

   case L3GD20_DEVICE:
   case L3GD20H_DEVICE:
      if (sa0 == L3G_SA0_LOW)
      {
         address = L3GD20_ADDRESS_SA0_LOW;
         return true;
      }
      else if (sa0 == L3G_SA0_HIGH)
      {
         address = L3GD20_ADDRESS_SA0_HIGH;
         return true;
      }
      else
         return this->AutoDetectAddress();

   default:
      return this->AutoDetectAddress();
   }
#if 0
   else
   {
      if (device == L3GD20_DEVICE && this->readReg(L3G_WHO_AM_I) == 0xD4) return true;
      if (device == L3GD20H_DEVICE && this->readReg(L3G_WHO_AM_I) == 0xD7) return true;
   }
#endif
   return false;
}

// Writes a gyro register
void
L3G_base<I2C>::writeReg(int reg, int value)
{
   char data[2];
   data[0] = reg;
   data[1] = value;
   i2c.write(address, &data[0], 2);
}

// Reads a gyro register
int
L3G_base<I2C>::readReg(int reg)
{
   char regc = reg;
   char result;
   int err = i2c.write(address, &regc, 1, true);
   if (err == 0)
      err = i2c.read(address, &result, 1);
   if (err < 0)
      return err;
   return result;
}

uint16_t
L3G_base<I2C>::read16(int reg)
{
   // assert the MSB of the address to get the gyro
   // to do slave-transmit subaddress updating.
   char regc = reg | 0x80;
   uint16_t Result;
   i2c.write(address, &regc, 1, true);
   i2c.read(address, static_cast<char*>(static_cast<void*>(&Result)), 2);
   return Result;
}

int
L3G_base<I2C>::Read(vector& g)
{
   char reg = char(L3G_OUT_X_L) | 0x80;
   int err = i2c.write(address, &reg, 1, true);
   if (err == 0)
   {
      err = i2c.read(address, (char*) &g, 6);
   }
   return err;
}

bool
L3G_base<I2C>::OK()
{
   if (address == L3G4200D_ADDRESS_SA0_LOW || address == L3G4200D_ADDRESS_SA0_HIGH)
   {
      if (this->readReg(L3G_WHO_AM_I) == 0xD3) return true;
   }
   else
   {
      if (device == L3GD20_DEVICE && this->readReg(L3G_WHO_AM_I) == 0xD4) return true;
      if (device == L3GD20H_DEVICE && this->readReg(L3G_WHO_AM_I) == 0xD7) return true;
   }
   return false;
}

//
// SPI mode
//
