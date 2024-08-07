// -*- C++ -*-

#ifndef L3G_SPI_H
#define L3G_SPI_H

#include "mbed.h"
#include "matvec.h"

// device types

#define L3G_DEVICE_AUTO  0
#define L3G4200D_DEVICE  1
#define L3GD20_DEVICE    2
#define L3GD20H_DEVICE   3

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

class L3G
{
    public:

    typedef vector3<short int> vector;

    L3G(I2CType& i2c_);

    // initializes the L3G device, returns true on success
    bool init(int device = L3G_DEVICE_AUTO, int sa0 = L3G_SA0_AUTO);

    int which_device() const { return device; }

    // resets the registers to the initial state
    void soft_reset();

    // Hard reset, L3GD20H only.
    void hard_reset();

    // returns true if the WHOAMI command returns a valid response
    bool OK();

    // Turns on the device and actives all 3 axes
    void EnableAll();

    // sets the scale of the gyro; valid values are 250, 500, 2000 degrees per second
    void SetScale(int DegreesPerSecond);

    // Enables or Ddisables 'block data update mode', so the registers are only updated on read of MSb and LSb
    void SetBlockDataUpdate(bool Enable);

    // sets the data rate and bandwidth, valid settings are any of the L3G_ODR_XX_XX
    // or L3G_H_ODR_XXX constants defined above
    void SetRateBandwidth(unsigned Setting);

    // enable or disable DRDY/INT2
    void SetInt2DataReady(bool Enable);

    void writeReg(int reg, int value);
    int readReg(int reg);

    // read the velocity vector.  Returns 0 on success, non-zero on failure
    int Read(vector& v);

    short int ReadX();
    short int ReadY();
    short int ReadZ();

  private:
      SPI Spi;
      DigitalOut NCS;
      int device; // chip type (4200D or D20 or D20H)
};

#endif
