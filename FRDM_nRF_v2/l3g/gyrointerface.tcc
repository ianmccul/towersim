// -*- C++ -*-

template <typename BusType>
bool
GyroInterface<BusType>::Functional()
{
   if (IsFunctional)
   {
      // check the watchdog timer
      if (WatchdogTimer.read_ms() > 1000)
         IsFunctional = false;
   }
   return IsFunctional;
}

template <typename BusType>
bool
GyroInterface<BusType>::Initialize()
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
   Impl.SetFIFOMode(L3GTypes::Stream);
   Impl.PowerUp();
   IsFunctional = true;
   LastReadSuccess = true;
   WatchdogTimer.reset();
   WatchdogTimer.start();
   return true;
}

template <typename BusType>
void
GyroInterface<BusType>::EnableAll()
{
   Impl.EnableAll();
}

template <typename BusType>
void
GyroInterface<BusType>::EnableX()
{
   Impl.EnableX();
}

template <typename BusType>
void
GyroInterface<BusType>::EnableY()
{
   Impl.EnableY();
}

template <typename BusType>
void
GyroInterface<BusType>::EnableZ()
{
   Impl.EnableZ();
}

template <typename BusType>
void
GyroInterface<BusType>::Sleep()
{
   Impl.PowerDown();
}

template <typename BusType>
void
GyroInterface<BusType>::Wakeup()
{
   Impl.PowerUp();
}

template <typename BusType>
void
GyroInterface<BusType>::SetRateBandwidth(unsigned RateBandwidth_)
{
   RateBandwidth = RateBandwidth_;
   Impl.SetRateBandwidth(RateBandwidth);
}

template <typename BusType>
bool
GyroInterface<BusType>::DataAvailable()
{
   return IsFunctional && DRdy;
}

template <typename BusType>
int
GyroInterface<BusType>::Read(vector& v)
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

template <typename BusType>
int
GyroInterface<BusType>::ReadX(int16_t& x)
{
   int err = Impl.ReadX(x);
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

template <typename BusType>
int
GyroInterface<BusType>::ReadY(int16_t& x)
{
   int err = Impl.ReadY(x);
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

template <typename BusType>
int
GyroInterface<BusType>::ReadZ(int16_t& x)
{
   int err = Impl.ReadZ(x);
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
