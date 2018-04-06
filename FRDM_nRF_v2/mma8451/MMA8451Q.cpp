
#include "MMA8451Q.h"

inline
short bswap(short s)
{
   uint16_t su = s;
   return short((su >> 8) | (su << 8));
}

MMA8451Q::MMA8451Q(I2C& i2c_, int addr)
  : I2CRegisterDevice(i2c_, addr)
{
}

uint8_t
MMA8451Q::readWhoAmI()
{
   return this->read_reg(MMA8451_WHO_AM_I);
}

bool
MMA8451Q::OK()
{
   return this->readWhoAmI() == 0x1A;
}

void
MMA8451Q::set_active(bool Active)
{
   this->write_bit(MMA8451_CTRL_REG1, 0, Active);
}

void
MMA8451Q::set_range(int Range)
{
   unsigned char x=0;
   if (Range == 4)
   {
      x = 1;
   }
   else if (Range == 8)
   {
      x = 2;
   }
   this->write_bits(MMA8451_XYZ_DATA_CFG, 0, 2, x);
}

void
MMA8451Q::set_int_od(bool od)
{
   this->write_bit(MMA8451_CTRL_REG3, 0, od);
}

void
MMA8451Q::set_int_polarity(bool polarity)
{
   this->write_bit(MMA8451_CTRL_REG3, 1, polarity);
}

void
MMA8451Q::set_int_data_ready(bool Enable)
{
   this->write_bit(MMA8451_CTRL_REG4, 0, Enable);
}

void
MMA8451Q::set_int_data_ready_pin(int Pin)
{
   this->write_bit(MMA8451_CTRL_REG5, 0, Pin == 1);
}

void
MMA8451Q::set_low_noise_mode(bool Enable)
{
   this->write_bit(MMA8451_CTRL_REG1, 2, Enable);
}

void
MMA8451Q::set_sampling_rate(SamplingRate r)
{
   this->write_bits(MMA8451_CTRL_REG1, 3, 3, int(r));
}

void
MMA8451Q::set_oversampling_mode(OversamplingMode m)
{
   this->write_bits(MMA8451_CTRL_REG2, 0, 2, int(m));
}

void
MMA8451Q::set_sleep_oversampling_mode(OversamplingMode m)
{
   this->write_bits(MMA8451_CTRL_REG2, 3, 2, int(m));
}

void
MMA8451Q::reset()
{
   this->write_bit(MMA8451_CTRL_REG2, 6, 1);
   bool ResetWait = true;
   while (ResetWait)
   {
      wait_ms(1);
      ResetWait = this->read_bit(MMA8451_CTRL_REG2, 6);
   }
}

int
MMA8451Q::readX()
{
   int x = this->read_reg16(MMA8451_OUT_X_MSB);
   if (x < 0)
      return -1;
   return bswap(uint16_t(x));
}

int
MMA8451Q::readY()
{
   int x = this->read_reg16(MMA8451_OUT_Y_MSB);
   if (x < 0)
      return -1;
   return bswap(uint16_t(x));
}

int
MMA8451Q::readZ()
{
   int x = this->read_reg16(MMA8451_OUT_Z_MSB);
   if (x < 0)
      return -1;
   return bswap(uint16_t(x));
}

void
MMA8451Q::readAll(MMA8451Q::vector& v)
{
   int result = this->read_regs(MMA8451_OUT_X_MSB, (char*) &v, 6);
   if (result < 0)
   {
      v[0] = -1;
      v[1] = -1;
      v[2] = -1;
      return;
   }
   v[0] = bswap(v[0]);
   v[1] = bswap(v[1]);
   v[2] = bswap(v[2]);
}

void
MMA8451Q::enable_motion_detect(bool AboveThreshold, bool X, bool Y, bool Z)
{
   uint8_t x = AboveThreshold ? 0x40 : 0x00;
   if (X)
      x |= 0x08;
   if (Y)
      x |= 0x10;
   if (Z)
      x |= 0x20;
   this->write_reg(MMA8451_FF_MT_CFG, x);
}

void
MMA8451Q::set_motion_detect_threshold(uint8_t Threshold)
{
   this->write_reg(MMA8451_FF_MT_THS, Threshold);
}

void
MMA8451Q::set_debounce_time(uint8_t steps)
{
   this->write_reg(MMA8451_FF_MT_COUNT, steps);
}


void
MMA8451Q::set_int_motion_detect(bool Enable)
{
   this->write_bit(MMA8451_CTRL_REG4, 2, Enable);
   this->write_bit(MMA8451_CTRL_REG5, 2, Enable);  // set to pin 1
}

void
MMA8451Q::enable_int_motion_sleep(bool Enable)
{
   this->write_bit(MMA8451_CTRL_REG3, 3, Enable);
}

