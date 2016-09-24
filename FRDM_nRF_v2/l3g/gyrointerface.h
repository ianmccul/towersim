// -*- C++ -*- $Id$

#if !defined(GRYOINTERFACE_H_)
#define GRYOINTERFACE_H_

#include "l3g.h"

//#define GYRO_DEFAULT_RATE_BANDWIDTH L3G_ODR_95_25
#define GYRO_DEFAULT_RATE_BANDWIDTH L3G_ODR_95_25

class GyroInterface
{
   public:
      GyroInterface(I2CType& i2c, int Addr, PinName DRdyPin);

      // attempts to initialize the gyro, returns true if successful
      bool Initialize();

      // changes the scale of the gyro.  Default 500 degress/sec
      void SetScale(int Scale_);

      // changes the rate/bandwidth of the gyro from the default DEFAULT_RATE_BANDWIDTH
      void SetRateBandwidth(unsigned RateBandwidth_);

      // returns true if the gyro is functioning and data is available
      bool DataAvailable();

      // returns true if the gyro is notionally functional
      bool Functional();

      // reads the velocity from the gyro, returns 0 on success
      int Read(L3G::vector& v);

      L3G& device() { return Impl; }

   private:
      L3G Impl;
      DigitalIn DRdy;
      int Scale;
      unsigned RateBandwidth;
      bool IsFunctional;
      bool LastReadSuccess;
      Timer WatchdogTimer;
};

#endif
