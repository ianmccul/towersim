//

#include "icm20602_spi.h"

ICM20602::ICM20602(PinName mosi, PinName miso, PinName sck, PinName csn, long SPIFrequency)
   : spi_(mosi, miso, sck), csn_(csn)
{
   spi_.frequency(SPIFrequency);
   spi_.format(8,3);
   this->writeReg(ICM20602_I2C_IF, 0x40); // disable I2C mode
}

void ICM20602::writeReg(int reg, int value)
{
   csn_ = 0;
   spi_.write(reg);
   spi_.write(value);
   csn_ = 1;

}

int ICM20602::readReg(int reg)
{
   csn_ = 0;
   spi_.write(reg | 0x80);
   int Result = spi_.write(0);
   csn_ = 1;
   return Result;
}

uint16_t ICM20602::read16(int reg)
{
   uint16_t DataH = this->readReg(reg);
   uint16_t DataL = this->readReg(reg+1);
   return (DataH << 8) | DataL;
}

void ICM20602::setBit(int reg, int bit)
{
   int x = this->readReg(reg);
   this->writeReg(reg, x | (0x1<<bit));
}

void ICM20602::clearBit(int reg, int bit)
{
   int x = this->readReg(reg);
   this->writeReg(reg, x & ~(0x1<<bit));
}

bool ICM20602::OK()
{
   return this->readReg(ICM20602_WHO_AM_I) == 0x12;
}

void ICM20602::soft_reset()
{

}

void ICM20602::hard_reset()
{
   int x = 0x01;
   this->writeReg(ICM20602_PWR_MGMT_1, x);
   // wait for the reset to complete
   while (x & 0x01)
   {
      x = this->readReg(ICM20602_PWR_MGMT_1);
   }
   this->writeReg(ICM20602_I2C_IF, 0x40); // disable I2C mode
}

void ICM20602::init()
{
   // clear bit 7 of CONFIG
   this->clearBit(ICM20602_CONFIG, 7);

   // set bit 1 of ACCEL_INTEL_CTRL
   this->setBit(ICM20602_ACCEL_INTEL_CTRL, 1);

   // set CLKSEL of PWR_MGMT_1 to 1, so that we choose the best clock source
   int x = this->readReg(ICM20602_PWR_MGMT_1);
   this->writeReg(ICM20602_PWR_MGMT_1, (x & 0xF4) | 0x01);
}

void ICM20602::sleep(bool state)
{
   if (state)
      this->setBit(ICM20602_PWR_MGMT_1, 6);
   else
   this->clearBit(ICM20602_PWR_MGMT_1, 6);
   }

void ICM20602::GyroStandby(bool state)
{
   if (state)
   {
      // make sure at least one gyro is on
      int x = this->readReg(ICM20602_PWR_MGMT_2) & 0xF4;
      if (x == 0x3)
         return;
      this->setBit(ICM20602_PWR_MGMT_1, 4);
   }
   else
   {
      this->clearBit(ICM20602_PWR_MGMT_1, 4);
   }
}

void ICM20602::SetGyroRateBandwidth(GyroRateBandwidth g)
{

}

void ICM20602::SetGyroScale(int DegreesPerSecond)
{

}

void ICM20602::EnableGyroX(bool state)
{
   if (state)
   {
      this->clearBit(ICM20602_PWR_MGMT_2, 2);
   }
   else
   {
      // if we are disabling the last gryo, make sure that we are not in standby mode
      int x = this->readReg(ICM20602_PWR_MGMT_2) & 0xF4;
      if ((x | 0x04) == 0x07)
         this->clearBit(ICM20602_PWR_MGMT_1, 4);
      // now disable the gyro
      this->setBit(ICM20602_PWR_MGMT_2, 2);
   }
}

void ICM20602::EnableGyroY(bool state)
{
   if (state)
   {
      this->clearBit(ICM20602_PWR_MGMT_2, 1);
   }
   else
   {
      // if we are disabling the last gryo, make sure that we are not in standby mode
      int x = this->readReg(ICM20602_PWR_MGMT_2) & 0xF4;
      if ((x | 0x02) == 0x07)
         this->clearBit(ICM20602_PWR_MGMT_1, 4);
      // now disable the gyro
      this->setBit(ICM20602_PWR_MGMT_2, 1);
   }
}

void ICM20602::EnableGyroZ(bool state)
{
   if (state)
   {
      this->clearBit(ICM20602_PWR_MGMT_2, 0);
   }
   else
   {
      // if we are disabling the last gryo, make sure that we are not in standby mode
      int x = this->readReg(ICM20602_PWR_MGMT_2) & 0xF4;
      if ((x | 0x01) == 0x07)
         this->clearBit(ICM20602_PWR_MGMT_1, 4);
      // now disable the gyro
      this->setBit(ICM20602_PWR_MGMT_2, 0);
   }
}

int16_t ICM20602::ReadGyroX()
{
   return this->read16(ICM20602_GYRO_XOUT_H);
}

int16_t ICM20602::ReadGyroY()
{
   return this->read16(ICM20602_GYRO_YOUT_H);
}

int16_t ICM20602::ReadGyroZ()
{
   return this->read16(ICM20602_GYRO_ZOUT_H);
}

float ICM20602::Temp()
{
   int16_t x = this->read16(ICM20602_TEMP_OUT_H);
   return (x * 0.00305997552f) + 25.0f;
}
