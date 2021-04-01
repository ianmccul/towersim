//
// l3g driver
//

#ifndef L3G_h
#define L3G_h

#include "mbed.h"
#include "common/matvec/matvec.h"

// device types
#define L3G_DEVICE_AUTO  0
#define L3G4200D_DEVICE  1
#define L3GD20_DEVICE    2
#define L3GD20H_DEVICE   3

// SA0 states
#define L3G_SA0_LOW  0
#define L3G_SA0_HIGH 1
#define L3G_SA0_AUTO 2

// register addresses
#define L3G_WHO_AM_I      0x0F
#define L3G_CTRL_REG1     0x20
#define L3G_CTRL_REG2     0x21
#define L3G_CTRL_REG3     0x22
#define L3G_CTRL_REG4     0x23
#define L3G_CTRL_REG5     0x24
#define L3G_REFERENCE     0x25
#define L3G_OUT_TEMP      0x26
#define L3G_STATUS_REG    0x27

#define L3G_OUT_X_L       0x28
#define L3G_OUT_X_H       0x29
#define L3G_OUT_Y_L       0x2A
#define L3G_OUT_Y_H       0x2B
#define L3G_OUT_Z_L       0x2C
#define L3G_OUT_Z_H       0x2D

#define L3G_FIFO_CTRL_REG 0x2E
#define L3G_FIFO_SRC_REG  0x2F

#define L3G_INT1_CFG      0x30
#define L3G_INT1_SRC      0x31
#define L3G_INT1_THS_XH   0x32
#define L3G_INT1_THS_XL   0x33
#define L3G_INT1_THS_YH   0x34
#define L3G_INT1_THS_YL   0x35
#define L3G_INT1_THS_ZH   0x36
#define L3G_INT1_THS_ZL   0x37
#define L3G_INT1_DURATION 0x38
#define L3G_LOW_ODR       0x39    // L3GD20H only

// Data rate and bandwidth settings for the L3GD20
// Data rate varies from 95Hz to 760Hz
// Bandwidth of the low-pass filter varies from 12.5Hz to 100Hz
#define L3G_ODR_95_12   0x00
#define L3G_ODR_95_25   0x10
#define L3G_ODR_190_12  0x40
#define L3G_ODR_190_25  0x50
#define L3G_ODR_190_50  0x60
#define L3G_ODR_190_70  0x70
#define L3G_ODR_380_20  0x80
#define L3G_ODR_380_25  0x90
#define L3G_ODR_380_50  0xA0
#define L3G_ODR_380_100 0xB0
#define L3G_ODR_760_30  0xC0
#define L3G_ODR_760_35  0xD0
#define L3G_ODR_760_50  0xE0
#define L3G_ODR_760_100 0xF0

// Data rate and bandwidth settings for the L3GD20H
// Data rate varies from 12.5Hz to 800Hz
// Bandwidth of the low-pass filter varies from 12.5Hz to 110Hz
#define L3G_H_ODR_12       0x0100
#define L3G_H_ODR_25       0x0140
#define L3G_H_ODR_50_16    0x0180
#define L3G_H_ODR_100_12   0x0000
#define L3G_H_ODR_100_25   0x0010
#define L3G_H_ODR_200_12   0x0040
#define L3G_H_ODR_200_70   0x0070
#define L3G_H_ODR_400_20   0x0080
#define L3G_H_ODR_400_25   0x0090
#define L3G_H_ODR_400_50   0x00A0
#define L3G_H_ODR_400_110  0x00B0
#define L3G_H_ODR_800_30   0x00C0
#define L3G_H_ODR_800_35   0x00D0
#define L3G_H_ODR_800_110  0x00F0

// I2C addresses
#define L3G4200D_ADDRESS_SA0_LOW  0xD0
#define L3G4200D_ADDRESS_SA0_HIGH 0xD2
#define L3GD20_ADDRESS_SA0_LOW    0xD4
#define L3GD20_ADDRESS_SA0_HIGH   0xD6
#define L3G_ADDRESS_AUTO          0x00

// use L3G<I2C> or L3G<SPI>

class L3GTypes
{
   public:
      typedef vector3<int16_t> vector;

      enum FIFO_Modes { Bypass=0, FIFO=1, Stream=2,
                        StreamFIFO=3, BypassStream=4,
                        DynamicStream=6, BypassFIFO=7 };

      struct FIFOStatus
      {
         FIFOStatus(char r) : Reg(r) {}
         char Reg;
         bool empty() const { return Reg & 0x20; }
         bool overrun() const { return Reg & 0x40; }
         bool threshold() const { return Reg & 0x80; }
         int level() const { return int(Reg & 0x1F); }
      };
};


template <typename BusType>
class L3G_base;

template <>
class L3G_base<I2C> : public L3GTypes
{
   public:
      using L3GTypes::vector;
      using L3GTypes::FIFO_Modes;
      using L3GTypes::FIFOStatus;

      L3G_base(PinName sda, PinName scl);

      //L3G_base(PinName sda, PinName scl, int device_, int sa0_);

      void SetFrequency(int hz) { i2c.frequency(hz); }

      //      bool init(int Device, int sa0);

      void writeReg(int reg, int value);
      int readReg(int reg);

      uint16_t read16(int reg);

      // returns 0 if no error
      int Read(vector& g);

      int ReadX(int16_t& x);
      int ReadY(int16_t& y);
      int ReadZ(int16_t& z);

      // returns true if the WHOAMI command returns a valid response
      bool OK();

      I2C& bus() { return i2c; }

   protected:
      int device;

   private:
      bool init(int Device, int sa0);

      bool AutoDetectAddress();

      I2C i2c;
      int address;
};

template <>
class L3G_base<SPI> : public L3GTypes
{
   public:
      using L3GTypes::vector;
      using L3GTypes::FIFO_Modes;
      using L3GTypes::FIFOStatus;

      L3G_base(PinName mosi, PinName miso, PinName sck, PinName csn_, long SPIFrequency = 2000000);

      void SetFrequency(int hz) { spi.frequency(hz); }

      void writeReg(int reg, int value);
      int readReg(int reg);

      SPI& bus() { return spi; }

      uint16_t read16(int reg);

      // returns 0 if no error
      int Read(vector& g);

      int ReadX(int16_t& x);
      int ReadY(int16_t& y);
      int ReadZ(int16_t& z);

      // returns true if the WHOAMI command returns a valid response
      bool OK();

   protected:
      int device;

   private:
      SPI spi;
      DigitalOut csn;
};


template <typename BusType>
class L3G : public  L3G_base<BusType>
{
   public:
      using L3G_base<BusType>::L3G_base;
      using L3G_base<BusType>::device;

      using FIFO_Modes = L3GTypes::FIFO_Modes;
      using FIFOStatus = L3GTypes::FIFOStatus;

      L3G() = delete;

      int which_device() const { return device; }

      // resets the registers to the initial state
      void soft_reset();

      // Hard reset, L3GD20H only.
      void hard_reset();

      // Powers up the device.  From the reset state, this switches the device into sleep mode,
      // where the X,Y,Z gyros are operating but not ready to send data.  To send data,
      // each axis needs to be enabled.
      void PowerUp();

      // Powers down the device.  When powered down, the masses of the gyros are not moving,
      // and most functions are powered off.  Content of registers is preserved.
      void PowerDown();

      // Enables the X, Y, or Z axes.
      void EnableX();
      void EnableY();
      void EnableZ();

      // Actives all 3 axes.
      void EnableAll();

      // sets the scale of the gyro; valid values are 250, 500, 2000 degrees
      // per second
      void SetScale(int DegreesPerSecond);

      // Enables or Ddisables 'block data update mode', so the
      // registers are only updated on read of MSb and LSb
      void SetBlockDataUpdate(bool Enable);

      // sets the data rate and bandwidth, valid settings are any of the
      // L3G_ODR_XX_XX
      // or L3G_H_ODR_XXX constants defined above
      void SetRateBandwidth(unsigned Setting);

      // enable or disable DRDY/INT2
      void SetInt2DataReady(bool Enable);

      // enable/disable FIFO threshold interrupt
      // default: disable
      void SetInt2FTH(bool Enable);

      // Set the FIFO mode.
      // default: bypass (no FIFO)
      void SetFIFOMode(L3GTypes::FIFO_Modes Mode);

      // Set the threshold for the FIFO interrupt
      void SetFIFOThreshold(int Threshold);

      void EnableFIFO(bool Enable);

      FIFOStatus GetFIFOStatus();

      // Enables interrupt pin 1
      // Default: disable
      void EnableInt1(bool Enable);

      // Int1 line active low
      // default: active high (false)
      void SetInt1ActiveLow(bool Low);

      // Set the threshold for int1 generation on a high event
      // (magnitude of gyro reading larger than threshold).
      // The Threshold is an unsigned 15-bits.
      // The sensitivity is slightly different, see
      // datasheet for details.
      void SetInt1ThresholdHighZ(uint16_t Thresh);

      // Enable triggering int1 line on a high gyro Z event
      void EnableInt1ThresholdHighZ(bool Enable);

      // Set the duration for threshold detection, in units of the data rate.
      // Default is no wait, interrupt immediately
      void SetInt1ThresholdDuration(bool Enable, uint8_t Duration);

      // reads the temperature sensor, returns the raw reading,
      // which is -1LSB/deg, unspecified offset (supposed to be 25?)
      int TempRaw();
};

#include "l3g.tcc"

#endif
