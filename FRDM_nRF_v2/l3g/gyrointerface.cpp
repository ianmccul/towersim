// -*- C++ -*- $Id$

#include "gyrointerface.h"

GyroInterface::GyroInterface(PinName sda, PinName scl, int Addr, PinName DRdyPin)
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

bool
GyroInterface::Functional()
{
   if (IsFunctional)
   {
      // check the watchdog timer
      if (WatchdogTimer.read_ms() > 1000)
         IsFunctional = false;
   }
   return IsFunctional;
}

bool
GyroInterface::Initialize()
{
   if (!Impl.OK())
   {
      IsFunctional = false;
      return false;
   }

   Impl.SetScale(Scale);
   Impl.SetRateBandwidth(RateBandwidth);
   //   Impl.SetInt2DataReady(true);
   Impl.SetInt2FTH(true);
   //Impl.SetBlockDataUpdate(true);  // in case there is lag
   Impl.SetFIFOThreshold(1);
   Impl.EnableFIFO(true);
   Impl.SetFIFOMode(L3G<I2C>::Stream);
   Impl.EnableAll();
   IsFunctional = true;
   LastReadSuccess = true;
   WatchdogTimer.reset();
   WatchdogTimer.start();
   return true;
}

void
GyroInterface::SetRateBandwidth(unsigned RateBandwidth_)
{
   RateBandwidth = RateBandwidth_;
   Impl.SetRateBandwidth(RateBandwidth);
}

bool
GyroInterface::DataAvailable()
{
   return IsFunctional && DRdy;
}

int
GyroInterface::Read(vector& v)
{
   int err = Impl.Read(v);
   if (err == 0)
   {
      // if we succeeded, reset the watchdog
      WatchdogTimer.reset();
   }
   else if (!LastReadSuccess)
   {
      // if we fail a read twice in a row, assume that the gyro is non-functional
      //      IsFunctional = false;
      //WatchdogTimer.stop();
   }
   LastReadSuccess = (err == 0);
   return err;
}
