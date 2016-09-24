#include "l3g.h"
#include <math.h>

#define L3G4200D_ADDRESS_SA0_LOW  0xD0
#define L3G4200D_ADDRESS_SA0_HIGH 0xD2
#define L3GD20_ADDRESS_SA0_LOW    0xD4
#define L3GD20_ADDRESS_SA0_HIGH   0xD6

L3G::L3G(I2CType& i2c_)
: i2c(i2c_),
  device(0),
  address(0)
{
}

bool L3G::init(int Device, int sa0)
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
        return autoDetectAddress();

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
        return autoDetectAddress();

    default:
      return autoDetectAddress();
  }
}

void
L3G::soft_reset()
{
    if (device == L3GD20_DEVICE)
    {
        this->writeReg(L3G_CTRL_REG1, 0x03);
    }
    if (device == L3GD20H_DEVICE)
    {
        this->writeReg(L3G_CTRL_REG1, 0x07);
        this->writeReg(L3G_LOW_ODR, 0x00);
    }
    if (device == L3GD20_DEVICE || device == L3GD20H_DEVICE)
    {
        this->writeReg(L3G_CTRL_REG2, 0x00);
        this->writeReg(L3G_CTRL_REG3, 0x00);
        this->writeReg(L3G_CTRL_REG4, 0x00);
        this->writeReg(L3G_CTRL_REG5, 0x00);
        this->writeReg(L3G_REFERENCE, 0x00);
        this->writeReg(L3G_FIFO_CTRL_REG, 0x00);
        this->writeReg(L3G_INT1_CFG, 0x00);
        this->writeReg(L3G_INT1_THS_XH, 0x00);
        this->writeReg(L3G_INT1_THS_XL, 0x00);
        this->writeReg(L3G_INT1_THS_YH, 0x00);
        this->writeReg(L3G_INT1_THS_YL, 0x00);
        this->writeReg(L3G_INT1_THS_ZH, 0x00);
        this->writeReg(L3G_INT1_THS_ZL, 0x00);
        this->writeReg(L3G_INT1_DURATION, 0x00);
    }
}

void
L3G::hard_reset()
{
   if (device == L3GD20H_DEVICE)
   {
       this->writeReg(L3G_LOW_ODR, 0x04);
   }
}

bool
L3G::OK()
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

// Turns on the L3G's gyro and places it in normal mode.
void L3G::EnableAll()
{
  // 0x0F = 0b00001111
  // Normal power mode, all axes enabled
   char Reg1 = this->readReg(L3G_CTRL_REG1);
   this->writeReg(L3G_CTRL_REG1, Reg1 | 0x0F);
}

void L3G::SetScale(int dps)
{
   char FS = 0;
   switch (dps)
   {
       case 250  : FS = 0; break;
       case 500  : FS = 1; break;
       case 2000 : FS = 3; break;
       default : FS = 0;
   }
   char Reg4 = this->readReg(L3G_CTRL_REG4);
   this->writeReg(L3G_CTRL_REG4, (Reg4 & 0xCF) | (FS << 4));
}

void L3G::SetBlockDataUpdate(bool Enable)
{
   char Reg4 = this->readReg(L3G_CTRL_REG4);
   this->writeReg(L3G_CTRL_REG4, (Reg4 & 0x7F) | (char(Enable) << 7));
}

void L3G::SetRateBandwidth(unsigned Setting)
{
   char Reg1 = this->readReg(L3G_CTRL_REG1);
   // no need to shift the Setting to the left, the defined constants already use the correct bits
   this->writeReg(L3G_CTRL_REG1, (Reg1 & 0x0F) | (char(Setting & 0xF0)));
   if (device == L3GD20H_DEVICE)
   {
       char Reg2 = this->readReg(L3G_LOW_ODR);
       this->writeReg(L3G_LOW_ODR, (Reg2 & 0xFE) | ((Setting >> 8) & 0x01));
    }
}

void L3G::SetInt2DataReady(bool Enable)
{
   char Reg3 = this->readReg(L3G_CTRL_REG3);
   this->writeReg(L3G_CTRL_REG3, (Reg3 & 0xF7) | (char(Enable) << 3));
}

void L3G::SetInt2FTH(bool Enable)
{
   char Reg3 = this->readReg(L3G_CTRL_REG3);
   this->writeReg(L3G_CTRL_REG3, (Reg3 & 0xF7) | (char(Enable) << 2));
}

void L3G::SetFIFOMode(FIFO_Modes Mode)
{
   char Reg = this->readReg(L3G_FIFO_CTRL_REG);
   this->writeReg(L3G_FIFO_CTRL_REG, (Reg & 0x1F) | ((char(Mode) & 0x03) << 5));
}

void L3G::SetFIFOThreshold(int Threshold)
{
   char Reg = this->readReg(L3G_FIFO_CTRL_REG);
   this->writeReg(L3G_FIFO_CTRL_REG, (Reg & 0x1F) | (char(Threshold & 0x1F)));
}

void L3G::EnableFIFO(bool Enable)
{
   char Reg5 = this->readReg(L3G_CTRL_REG5);
   this->writeReg(L3G_CTRL_REG5, (Reg5 & 0xBF) | (char(Enable) << 6));
}

L3G::FIFOStatus L3G::GetFIFOStatus()
{
   return FIFOStatus(char(this->readReg(L3G_FIFO_SRC_REG)));
}

// Writes a gyro register
void L3G::writeReg(int reg, int value)
{
   char data[2];
   data[0] = reg;
   data[1] = value;
   i2c.write(address, &data[0], 2);
}

// Reads a gyro register
int L3G::readReg(int reg)
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

short int
L3G::ReadX()
{
   short int Result;
   char reg = char(L3G_OUT_X_L) | (1 << 7);
   i2c.write(address, &reg, 1, true);
   i2c.read(address, (char*) &Result, 2);
   return Result;
}

short int
L3G::ReadY()
{
   short int Result;
   char reg = char(L3G_OUT_Y_L) | (1 << 7);
   i2c.write(address, &reg, 1, true);
   i2c.read(address, (char*) &Result, 2);
   return Result;
}

short int
L3G::ReadZ()
{
   short int Result;
   char reg = char(L3G_OUT_Z_L) | (1 << 7);
   i2c.write(address, &reg, 1, true);
   i2c.read(address, (char*) &Result, 2);
   return Result;
}



// Reads the 3 gyro channels and stores them in vector g
int
L3G::Read(vector& g)
{
   // assert the MSB of the address to get the gyro
   // to do slave-transmit subaddress updating.
   char reg = char(L3G_OUT_X_L) | (1 << 7);
   //__disable_irq();
   int err = i2c.write(address, &reg, 1, true);
   if (err == 0)
   {
      err = i2c.read(address, (char*) &g, 6);
   }
   //__enable_irq();
   return err;
}

// Private Methods //////////////////////////////////////////////////////////////

bool L3G::autoDetectAddress(void)
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
   return false;
}
