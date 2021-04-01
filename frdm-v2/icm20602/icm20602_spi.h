// icm20602_spi_h
//
// Driver for Invensense ICM-20602 6-axis

#if !defined(ICM20602_H)
#define ICM20602_H

#include "mbed.h"

#define ICM20602_XG_OFFS_TC_H       0x04
#define ICM20602_XG_OFFS_TC_L       0x05
#define ICM20602_YG_OFFS_TC_H       0x07
#define ICM20602_YG_OFFS_TC_L       0x08
#define ICM20602_ZG_OFFS_TC_H       0x0A
#define ICM20602_ZG_OFFS_TC_L       0x0B
#define ICM20602_SELF_TEST_X_ACCEL  0x0D
#define ICM20602_SELF_TEST_Y_ACCEL  0x0E
#define ICM20602_SELF_TEST_Z_ACCEL  0x0F
#define ICM20602_XG_OFFS_USRH       0x13
#define ICM20602_XG_OFFS_USRL       0x14
#define ICM20602_YG_OFFS_USRH       0x15
#define ICM20602_YG_OFFS_USRL       0x16
#define ICM20602_ZG_OFFS_USRH       0x17
#define ICM20602_ZG_OFFS_USRL       0x18
#define ICM20602_SMPLRT_DIV         0x19
#define ICM20602_CONFIG             0x1A
#define ICM20602_GYRO_CONFIG        0x1B
#define ICM20602_ACCEL_CONFIG       0x1C
#define ICM20602_ACCEL_CONFIG2      0x1D
#define ICM20602_LP_MODE_CFG        0x1E
#define ICM20602_ACCEL_WOM_X_THR    0x20
#define ICM20602_ACCEL_WOM_Y_THR    0x21
#define ICM20602_ACCEL_WOM_Z_THR    0x22
#define ICM20602_FIFO_EN            0x23
#define ICM20602_FSYNC_INT          0x36
#define ICM20602_INT_PIN_CFG        0x37
#define ICM20602_INT_ENABLE         0x38
#define ICM20602_FIFO_WM_INT_STATUS 0x39
#define ICM20602_INT_STATUS         0x3A
#define ICM20602_ACCEL_XOUT_H       0x3B
#define ICM20602_ACCEL_XOUT_L       0x3C
#define ICM20602_ACCEL_YOUT_H       0x3D
#define ICM20602_ACCEL_YOUT_L       0x3E
#define ICM20602_ACCEL_ZOUT_H       0x3F
#define ICM20602_ACCEL_ZOUT_L       0x40
#define ICM20602_TEMP_OUT_H         0x41
#define ICM20602_TEMP_OUT_L         0x42
#define ICM20602_GYRO_XOUT_H        0x43
#define ICM20602_GYRO_XOUT_L        0x44
#define ICM20602_GYRO_YOUT_H        0x45
#define ICM20602_GYRO_YOUT_L        0x46
#define ICM20602_GYRO_ZOUT_H        0x47
#define ICM20602_GYRO_ZOUT_L        0x48
#define ICM20602_SELF_TEST_X_GYRO   0x50
#define ICM20602_SELF_TEST_Y_GYRO   0x51
#define ICM20602_SELF_TEST_Z_GYRO   0x52
#define ICM20602_FIFO_WM_TH1        0x60
#define ICM20602_FIFO_WM_TH2        0x61
#define ICM20602_SIGNAL_PATH_RESET  0x68
#define ICM20602_ACCEL_INTEL_CTRL   0x69
#define ICM20602_USER_CTRL          0x6A
#define ICM20602_PWR_MGMT_1         0x6B
#define ICM20602_PWR_MGMT_2         0x6C
#define ICM20602_I2C_IF             0x70
#define ICM20602_FIFO_COUNTH        0x72
#define ICM20602_FIFO_COUNTL        0x73
#define ICM20602_FIFO_R_W           0x74
#define ICM20602_WHO_AM_I           0x75
#define ICM20602_XA_OFFSET_H        0x77
#define ICM20602_XA_OFFSET_L        0x78
#define ICM20602_YA_OFFSET_H        0x7A
#define ICM20602_YA_OFFSET_L        0x7B
#define ICM20602_ZA_OFFSET_H        0x7D
#define ICM20602_ZA_OFFSET_L        0x7E

extern float aRes, gRes;
extern Serial pc;

class ICM20602
{
   public:
      // data rate and bandwidth of the gyro
      enum GyroRateBandwidth { ODR_32K_BW_8173=16, ODR_32K_BW_3281=32, ODR_8K_BW_3281=8, ODR_8K_BW_250=0,
                               ODR_1K_BW_176=1,    ODR_1K_BW_92=2,     ODR_1K_BW_41=3,   ODR_1K_BW_20=4,
                               ODR_1K_BW_10=5,     ODR_1K_BW_5=6, };

      ICM20602(PinName mosi_, PinName miso_, PinName sck_, PinName csn_, long SPIFrequency = 2000000);

      void SetFrequency(long Hz) { spi_.frequency(Hz); }

      void writeReg(int reg, int value);
      int readReg(int reg);

      void setBit(int reg, int bit);
      void clearBit(int reg, int bit);

      SPI& bus() { return spi_; }

      uint16_t read16(int reg);

      // returns true if the WHO_AM_I command returns the expected value 0x12
      bool OK();

      // reset registers to their default values
      void soft_reset();

      // hard reset via the PWR_MGMT_1 register.  Blocks until the reset is complete.
      void hard_reset();

      // Initialize the device.  This must be called on power-up and after a reset.
      // The device is in sleep mode after a reset, and stays in sleep mode after intiailization.
      // From the reset state, clear bit 7 of the CONFIG register,
      // set bit 1 of the ACCEL_INTEL_CTRL register,
      // set CLKSEL of PWR_MGMT_1 to 1
      void init();

      // enables or disables sleep mode.
      void sleep(bool state = true);

      // if true, enter standby mode.  If false, leave standby mode.
      // NOTE: at least one gyroscope axis must be on prior to entering standby mode.
      // If all gryos are disabled then this does nothing.
      void GyroStandby(bool state = true);

      void SetGyroRateBandwidth(GyroRateBandwidth g);

      // Set the gyroscope scale.  Valid values are 250, 500, 1000, 2000 degrees per second.
      void SetGyroScale(int DegreesPerSecond);

      // Enable or disable the gyros.  Note that gyros should not be turned off when in standby mode;
      // turning off the last gyro will automatically disable standby mode.
      void EnableGyroX(bool state = true);
      void EnableGyroY(bool state = true);
      void EnableGyroZ(bool state = true);

      int16_t ReadGyroX();
      int16_t ReadGyroY();
      int16_t ReadGyroZ();

      // reads the temperature sensor; returns value in degrees C
      float Temp();

   private:
      SPI spi_;
      DigitalOut csn_;

};

#endif
