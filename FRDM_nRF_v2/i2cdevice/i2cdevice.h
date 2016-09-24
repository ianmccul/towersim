// i2cdevice.h
// provides register support for i2c devices

#ifndef I2CDEVICE_H
#define I2CDEVICE

#include "mbed.h"

class I2CRegisterDevice
{
   public:
      I2CRegisterDevice(I2C& i2c_, int Address_);

      // write a register, assuming the caller has disabled interrupts
      int write_reg_IRQ(int reg, int value);
      
      // read from a register, assuming the caller has disabled interrupts
      int read_reg_IRQ(int reg);

      // write a register
      int write_reg(int reg, int value);
      
      // read from a register.  -ve value on error
      int read_reg(int reg);

      // reads a 32-bit register
      int read_reg16(int reg);
      
      // Writes a sequence of registers, returns 0 on success, <0 on failure
      // int write_regs(int reg, char const* buf, int length);  - not yet implemented

      // Reads a sequence of registers, returns 0 on success, <0 on failure
      int read_regs(int reg, char* buf, int length);

      // reads a single bit from the given register
      bool read_bit(int Register, int Bit);

      // reads a few bits, FirstBit is the lowest order bit that we want to read
      int read_bits(int Register, int FirstBit, int NumBits);

      // write a single bit - returns < 0 on error
      int write_bit(int Register, int Bit, bool Value);

      // writes a set of bits, from FirstBit (lowest order) to FirstBit+NumBits-1
      void write_bits(int Register, int FirstBit, int NumBits, uint8_t Value);

      // direct reference to the device
      I2C& device();
      
      // returns the I2C address of this device
      int address() const;
      
      // resets the I2C address of this device
      void set_address(int NewAddress);

   private:
      I2C& Device;
      int Address;
};

inline
I2CRegisterDevice::I2CRegisterDevice(I2C& i2c_, int Address_)
  : Device(i2c_), Address(Address_)
{
}

inline
int I2CRegisterDevice::write_reg_IRQ(int reg, int value)
{
   char data[2];
   data[0] = reg;
   data[1] = value;
   return Device.write(Address, &data[0], 2);
}

inline
int I2CRegisterDevice::read_reg_IRQ(int reg)
{
    char regc = reg;
    char result;
    int err = Device.write(Address, &regc, 1, true);
    if (err == 0)
       err = Device.read(Address, &result, 1);
    if (err < 0)
       return err;
    return result;
}

inline
int I2CRegisterDevice::write_reg(int reg, int value)
{
   //__disable_irq();
   char data[2];
   data[0] = reg;
   data[1] = value;
   int err = Device.write(Address, &data[0], 2);
   //__enable_irq();
   return err;
}

inline
int I2CRegisterDevice::read_reg(int reg)
{
   //__disable_irq();
   char regc = reg;
   char result;
   int err = Device.write(Address, &regc, 1, true);
   if (err == 0)
      err = Device.read(Address, &result, 1);
   //__enable_irq();
   if (err < 0)
      return err;
    return result;
}

inline
int I2CRegisterDevice::read_reg16(int reg)
{
    char regc = reg;
    uint16_t result;
    int err = Device.write(Address, &regc, 1, true);
    if (err == 0)
       err = Device.read(Address, (char*) &result, 2);
    if (err < 0)
       return err;
    return result;
}

#if 0
inline 
void
I2CRegisterDevice::write_regs(int reg, char const* buf, int length)
{
   int err = Device.write(Address, &regc, 1, true);
   if (err == 0)
      err = Device.write(Address, buf, length);
   return err;
}
#endif

inline
int
I2CRegisterDevice::read_regs(int reg, char* buf, int length)
{
   char regc = reg;
   int err = Device.write(Address, &regc, 1, true);
   if (err == 0)
      err = Device.read(Address, buf, length);
   return err;
}

inline
bool
I2CRegisterDevice::read_bit(int Register, int Bit)
{
   int x = this->read_reg(Register);
   return (x >> Bit) & 0x01;
}

inline
int
I2CRegisterDevice::read_bits(int Register, int FirstBit, int NumBits)
{
   int x = this->read_reg(Register);
   return (x >> FirstBit) & ((1U << (NumBits)) - 1);
}

inline
int
I2CRegisterDevice::write_bit(int Register, int Bit, bool Value)
{
   //__disable_irq();
   int x = this->read_reg_IRQ(Register);
   if (x < 0)
   {
      __enable_irq();
      return x;
   }
   x = Value ? (x | (1 << Bit)) : (x & ~(1 << Bit));
   x = this->write_reg_IRQ(Register, x);
   //__enable_irq();
   return x;
}

inline
void
I2CRegisterDevice::write_bits(int Register, int FirstBit, int NumBits, uint8_t Value)
{
   //__disable_irq();
   int x = this->read_reg_IRQ(Register);
   char Mask = ((1U << (NumBits)) - 1) << FirstBit;
   x &= ~Mask;
   x |= (Value << FirstBit) & Mask;
   this->write_reg_IRQ(Register, x);
   //__enable_irq();
}

inline
I2C&
I2CRegisterDevice::device()
{
   return Device;
}

inline
int
I2CRegisterDevice::address() const
{
   return Address;
}

inline
void
I2CRegisterDevice::set_address(int NewAddress)
{
   Address = NewAddress;
}
 
#endif
