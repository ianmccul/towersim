#ifndef MMA8451_H
#define MMA8451_H

#include "i2cdevice.h"

#define MMA8451_I2C_ADDRESS (0x1d<<1)

// registers
#define MMA8451_STATUS           0x00
#define MMA8451_OUT_X_MSB        0x01
#define MMA8451_OUT_X_LSB        0x02
#define MMA8451_OUT_Y_MSB        0x03
#define MMA8451_OUT_Y_LSB        0x04
#define MMA8451_OUT_Z_MSB        0x05
#define MMA8451_OUT_Z_LSB        0x06

#define MMA8451_F_SETUP          0x09
#define MMA8451_TRIG_CFG         0x0A
#define MMA8451_SYSMOD           0x0B
#define MMA8451_INT_SOURCE       0x0C
#define MMA8451_WHO_AM_I         0x0D
#define MMA8451_XYZ_DATA_CFG     0x0E
#define MMA8451_HP_FILTER_CUTOFF 0x0F
#define MMA8451_PL_STATUS        0x10
#define MMA8451_PL_CFG           0x11
#define MMA8451_PL_COUNT         0x12
#define MMA8451_PL_BF_ZCOMP      0x13
#define MMA8451_P_L_THS_REG      0x14
#define MMA8451_FF_MT_CFG        0x15
#define MMA8451_FF_MT_SRC        0x16
#define MMA8451_FF_MT_THS        0x17
#define MMA8451_FF_MT_COUNT      0x18

#define MMA8451_TRANSIENT_CFG    0x1D
#define MMA8451_TRANSIENT_SCR    0x0E
#define MMA8451_TRANSIENT_THS    0x1F
#define MMA8451_TRANSIENT_COUNT  0x20
#define MMA8451_PULSE_CFG        0x21
#define MMA8451_PULSE_SRC        0x22
#define MMA8451_PULSE_THSX       0x23
#define MMA8451_PULSE_THSY       0x24
#define MMA8451_PULSE_THSZ       0x25
#define MMA8451_PULSE_TMLT       0x26
#define MMA8451_PULSE_LTCY       0x27
#define MMA8451_PULSE_WIND       0x28
#define MMA8451_ASLP_COUNT       0x29
#define MMA8451_CTRL_REG1        0x2A
#define MMA8451_CTRL_REG2        0x2B
#define MMA8451_CTRL_REG3        0x2C
#define MMA8451_CTRL_REG4        0x2D
#define MMA8451_CTRL_REG5        0x2E
#define MMA8451_OFF_X            0x2F
#define MMA8451_OFF_Y            0x30
#define MMA8451_OFF_Z            0x31

class MMA8451Q : public I2CRegisterDevice
{
   public:
      struct vector                                                                                                                                                                                             
      {                                                                                                                                                                                                         
         short int x, y, z;                                                                                                                                                                                            
      };      
      
      enum SamplingRate
      {
         RATE_800       = 0x0,   // 800 Hz
         RATE_400       = 0x1,   // 400 Hz
         RATE_200       = 0x2,   // 200 Hz
         RATE_100       = 0x3,   // 100 Hz
         RATE_50        = 0x4,   // 50 Hz
         RATE_12_5      = 0x5,   // 12.5 Hz
         RATE_6_25      = 0x6,   // 6.25 Hz
         RATE_1_563     = 0x7,   // 1.563 Hz
      };
      
      enum OversamplingMode
      {
         OSMODE_Normal           = 0x0,
         OSMODE_LowNoiseLowPower = 0x1,
         OSMODE_HighResolution   = 0x2,
         OSMODE_LowPower         = 0x3,
      };                                                                                                                                                                                         

      MMA8451Q(I2C& i2c_, int addr = MMA8451_I2C_ADDRESS);

      // turn the contents of the WHO_AM_I register
      // should be 0x1A
      uint8_t readWhoAmI();
 
      // returns true if the accelerometer identifies properly
      bool OK();
      
      // true -> activate the accelerometer, false == put accelerometer in standby mode (default: standby)
      void set_active(bool Active);
      
      // Sets the full-scale range in units of g.  Valid values are 2, 4, 8 (default: 2)
      void set_range(int Range);
      
      // enable or disable the high pass filter.
      // default: disabled
      void set_hpf(bool Enable);
      
      // set the cutoff frequency for the high pass filter.
      // Valid values are 0 - 3.  Meaning depends on the output data rate and oversampling mode.
      // see datasheet for details.
      // default: 0
      void set_hpf_cutoff(unsigned c);

      // enable or disable the low pass filter
    // default: enabled
      void set_lpf(bool Enable);
      
      // true == interrupt pad is open drain, false == interrupt pad is push-pull (default: push-pull)
      void set_int_od(bool od);
      
      // true == interrupt pad is active high, false == interrupt pad is active low (default: active low)
      void set_int_polarity(bool polarity);
      
      // Enable or disable interrupt on data ready (default: disabled)
      void set_int_data_ready(bool Enable);
      
      // Route the data ready interrupt to either pin 1 or pin 2 (default: pin 2)
      void set_int_data_ready_pin(int Pin);
      
      // Enables low noise mode, with maximum range +/- 4g (default: false)
      void set_low_noise_mode(bool Enable);
      
      // Sets the sampling rate, 800Hz, 400Hz, 200Hz, 100Hz, 60Hz, 12.5Hz, 6.25Hz, 1.563Hz (default: 800Hz)
      void set_sampling_rate(SamplingRate r);
      
      // Sets the oversampling mode for ACTIVE operation
      // normal, low-noise low-power, high-resolution, low-power (default: normal)
      void set_oversampling_mode(OversamplingMode m);

      // Sets the oversampling mode when in SLEEP mode
      void set_sleep_oversampling_mode(OversamplingMode m);
      
      // Resets the accelerometer to default
      void reset();

      // Read the accelerometer axes
      int readX();
      int readY();
      int readZ();
      
      // Read all of the accelerometer axes
      void readAll(vector& v);
};

#endif
