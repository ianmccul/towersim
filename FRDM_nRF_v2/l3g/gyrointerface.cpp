// -*- C++ -*-

#include "gyrointerface.h"

template <>
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

template <>
GyroInterfaceBase<SPI>::GyroInterfaceBase(PinName mosi, PinName miso, PinName sck, PinName csn,
                                          PinName DRdyPin)
   : Impl(mosi, miso, sck, csn),
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
