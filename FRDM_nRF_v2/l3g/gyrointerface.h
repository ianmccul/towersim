// -*- C++ -*-

#if !defined(GRYOINTERFACE_H_)
#define GRYOINTERFACE_H_

#include "l3g.h"

//#define GYRO_DEFAULT_RATE_BANDWIDTH L3G_ODR_95_25
#define GYRO_DEFAULT_RATE_BANDWIDTH L3G_ODR_95_25


template <typename BusType>
class GyroInterfaceBase;

template <>
class GyroInterfaceBase<I2C>
{
   public:
      GyroInterfaceBase(PinName sda, PinName scl, int Addr, PinName DRdyPin,
                        PinName Int1Pin);

   protected:
      L3G<I2C> Impl;
      DigitalIn DRdy;
      DigitalIn Int1;
      int Scale;
      unsigned RateBandwidth;
      bool IsFunctional;
      bool LastReadSuccess;
      Timer WatchdogTimer;
};

template <>
class GyroInterfaceBase<SPI>
{
   public:
      GyroInterfaceBase(PinName mosi, PinName miso, PinName sck, PinName csn,
                        PinName DRdyPin, PinName Int1Pin);

   protected:
      L3G<SPI> Impl;
      DigitalIn DRdy;
      DigitalIn Int1;
      int Scale;
      unsigned RateBandwidth;
      bool IsFunctional;
      bool LastReadSuccess;
      Timer WatchdogTimer;
};

template <typename BusType>
class GyroInterface : public GyroInterfaceBase<BusType>
{
   public:
      using GyroInterfaceBase<BusType>::GyroInterfaceBase;

      using GyroInterfaceBase<BusType>::Impl;
      using GyroInterfaceBase<BusType>::DRdy;
      using GyroInterfaceBase<BusType>::Scale;
      using GyroInterfaceBase<BusType>::RateBandwidth;
      using GyroInterfaceBase<BusType>::IsFunctional;
      using GyroInterfaceBase<BusType>::LastReadSuccess;
      using GyroInterfaceBase<BusType>::WatchdogTimer;

      typedef L3GTypes::vector vector;

      // attempts to initialize the gyro, returns true if successful
      bool Initialize();

      // enables all of the axes
      void EnableAll();

      // Enable individual axes
      void EnableX();
      void EnableY();
      void EnableZ();

      // Clear the data buffers.  This is useful when waking up from sleep
      void ClearData();

      // changes the scale of the gyro.  Default 500 degress/sec
      void SetScale(int Scale_);

      // changes the rate/bandwidth of the gyro from the default DEFAULT_RATE_BANDWIDTH
      void SetRateBandwidth(unsigned RateBandwidth_);

      // returns true if the gyro is functioning and data is available
      bool DataAvailable();

      // returns true if the gyro is notionally functional
      bool Functional();

      // reads the velocity from the gyro, returns 0 on success
      int Read(vector& v);

      // reads the velocity from the X gyro, returns 0 on success
      int ReadX(int16_t& x);
      int ReadY(int16_t& y);
      int ReadZ(int16_t& z);

      // sleep mode - this powers down the device
      void Sleep();

      // recover from sleep mode and renable normal operation
      void Wakeup();

      // Enters low power mode with interupt generation on INT1 for Z-axis motion
      // larger than Threshold
      void SleepForMotionInterrupt(uint16_t Threshold);

      // Disables the low power interrupt mode
      void DisableMotionInterrupt();

      L3G<BusType>& device() { return Impl; }

};

#include "gyrointerface.tcc"

#endif
