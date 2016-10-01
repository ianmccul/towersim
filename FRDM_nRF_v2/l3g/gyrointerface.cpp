// -*- C++ -*-

#include "gyrointerface.h"

GyroInterfaceBase<I2C>::GyroInterfaceBase(PinName sda, PinName scl, int Addr, PinName DRdyPin)
   : Impl(sda, scl),
     DRdy(DRdyPin),
     Scale(500),
     RateBandwidth(GYRO_DEFAULT_RATE_BANDWIDTH),
     IsFunctional(false),
     LastReadSuccess(true)
{
   bool OK = Impl.OK();
   if (!OK)
   {
        printf("Gyro failed to initialize.\r\n");
    }
   Impl.soft_reset();
}

GyroInterfaceBase<SPI>::GyroInterfaceBase(PinName mosi, PinName miso, PinName sck, PinName csn,
                                          PinName DRdyPin)
   : Impl(mosi, miso, sck, csn, 8000000),
     DRdy(DRdyPin),
     Scale(500),
     RateBandwidth(GYRO_DEFAULT_RATE_BANDWIDTH),
     IsFunctional(false),
     LastReadSuccess(true)
{
   bool OK = Impl.OK();
   if (!OK)
   {
        printf("Gyro failed to initialize.\r\n");
    }
   Impl.soft_reset();
}
