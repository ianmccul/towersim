//
// l3g driver
//

#ifndef L3G_h
#define L3G_h

#include "mbed.h"
#include "matvec.h"

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

template <typename BusType>
class L3G_base;

template <>
class L3G_base<I2C>
{
   public:
      typedef vector3<int16_t> vector;

      L3G_base(PinName sda, PinName scl);

      //L3G_base(PinName sda, PinName scl, int device_, int sa0_);

      void SetFrequency(int hz) { i2c.frequency(hz); }

      //      bool init(int Device, int sa0);

      void writeReg(int reg, int value);
      int readReg(int reg);

      uint16_t read16(int reg);

      // returns 0 if no error
      int Read(vector& g);

      void hard_reset();

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
class L3G_base<SPI>
{
   public:
      //L3G_base(SPI& Device_) : Device(Device_) {}

      bool init();

      void writeReg(int reg, int value);
      int readReg(int reg);

      SPI& bus() { return spi; }

   protected:
      int device;

   private:
      SPI spi;
};


template <typename BusType>
class L3G : public  L3G_base<BusType>
{
   public:
      using L3G_base<BusType>::L3G_base;
      using L3G_base<BusType>::device;

      L3G() = delete;

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

      // initializes the L3G device, returns true on success
      bool init(int device = L3G_DEVICE_AUTO, int sa0 = L3G_SA0_AUTO);

      int which_device() const { return device; }

      // resets the registers to the initial state
      void soft_reset();

      // Hard reset, L3GD20H only.
      void hard_reset();

      // Turns on the device and actives all 3 axes
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
      // default: disalbe
      void SetInt2FTH(bool Enable);

      // Set the FIFO mode.
      // default: bypass (no FIFO)
      void SetFIFOMode(FIFO_Modes Mode);

      // Set the threshold for the FIFO interrupt
      void SetFIFOThreshold(int Threshold);

      void EnableFIFO(bool Enable);

      FIFOStatus GetFIFOStatus();

      // reads the temperature sensor, returns the raw reading,
      // which is -1LSB/deg with an offset of 50
      int TempRaw();

      // Sensor temperature in degrees C
      int Temp();

      int16_t ReadX();
      int16_t ReadY();
      int16_t ReadZ();
};

#include "l3g.tcc"

#endif
