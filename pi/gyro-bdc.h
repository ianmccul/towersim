// -*- C++ -*- $Id: gyro-bdc.h 1876 2016-10-02 13:46:09Z uqimccul $
//
// A 'bottom dead centre' sensor via gyro, using
// a quadratic fit.  By inspection, we judge a reasonable fitting
// procedure is about 0.15 seconds of data, approx 100 - 130 samples.
//
// We use a circular_buffer that can hold BufMax samples (around 100 - 130).
// If the maximum velocity (absolute value) is greater than Threshold (around 1000 - 2000),
// and occurs in the middle of the buffer (by time), then we treat this as a local extremum
// and do a quadratic fit to find the location of the extremum and the velocity.
//
// After testing, we note that it sometimes false-triggers on a stay incident.
// It seems that this is difficult to detect from first principles,
// the curvature / velocity doesn't
// differ significantly from BDC when raising/lowering the bell.  We could make a check
// based on a minimum time between BDC, and/or the change in velocity at BDC.
// It is reasonable to assume that the change in squared velocity (ie, the kinetic energy)
// is smaller than say 2e8.
//
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <cmath>
#include <boost/circular_buffer.hpp>
#include <list>
#include "quadfit.h"

double const ThresholdVelocity = 1000;
int64_t const HalfTimeLagMicrosec = 80000;  // 0.08 seconds
double const EnergyThreshold = 2.0e8;


// After ResetTime seconds since a BDC trigger, we accept a BDC even
// if the EnergyThreshold isn't met.
// This is mostly in case something goes wrong.  10 million microseconds = 10 seconds
int64_t const ResetTime = 10000000;

class GyroBDC
{
   public:
      GyroBDC();

      typedef std::pair<int64_t, double> VelocityPoint;

      std::list<VelocityPoint> BDCPoints;

      // Processes a sample.  Returns true if a BDC event has been detected, in which case
      // it is added to the output list
      bool Process(int64_t Time, float z);

   private:
      boost::circular_buffer<std::pair<int64_t, short>> SampleBuf;
      float CurrentMax;
      int64_t CurrentMaxTime;
      int LastSign;
      double LastEnergy;
      int64_t LastBDC;
};

inline
GyroBDC::GyroBDC()
   : SampleBuf(HalfTimeLagMicrosec*2/760+20),
     CurrentMax(0),
     CurrentMaxTime(0),
     LastSign(0),
     LastEnergy(0),
     LastBDC(0)
{
}

inline
bool
GyroBDC::Process(int64_t Time, float z)
{
   bool Found = false;
   SampleBuf.push_back({Time, z});
   if (z*LastSign < 0)
   {
      //std::cout << "sign change\n";
      CurrentMax = z;
      CurrentMaxTime = Time;
      LastSign = 0;
   }
   if (std::abs(z) >= std::abs(CurrentMax))
   {
      CurrentMax = z;
      CurrentMaxTime = Time;
      }
   else
   {
      if (LastSign == 0 && abs(CurrentMax) > ThresholdVelocity && Time - CurrentMaxTime >= HalfTimeLagMicrosec)
      {
         // we have a local maxima.  Trim the buffer to HalfTimeLagMicrosec on both ends
         while (SampleBuf.front().first < (CurrentMaxTime-HalfTimeLagMicrosec))
            SampleBuf.pop_front();
         // fit the buffer
         // Sanity check that we have enough samples
         if (SampleBuf.size() < 10)
         {
            std::cerr << "Severe packet loss, discarding possible BDC at " << CurrentMaxTime << '\n';
         }
         else
         {
            // Assemble the data for a quadratic fit
            std::vector<double> x;
            std::vector<double> y;
            for (auto c : SampleBuf)
            {
               x.push_back((c.first - CurrentMaxTime) / 1.0E6);
               y.push_back(c.second);
            }
            quadratic_fit_result R = quadratic_fit(x,y);
            // Quadratic is ax^2 + bx + c
            // The minima is at 2ax+b = 0, so x = -b/(2a)
            // the value of the quadratic at the minima is b^2/(4a) - b^2/(2a) + c = c-b^2/(4a)
            double T = -R.b / (2*R.a);
            int64_t TimeEx = int64_t(round(T*1E6)) + CurrentMaxTime;
            double V = R.c - R.b*R.b/(4*R.a);
            // DS test.  Check that the energy has changed by less than EnergyThreshold (or the LastEnergy is zero).
            if (LastEnergy != 0 && std::abs(V*V-LastEnergy) > EnergyThreshold && TimeEx < LastBDC+ResetTime)
            {
               std::cerr << "Ignoring local extrema at " << T << " with velocity " << V
                         << " as the change in energy is too big.  Possible stay event?\n";
            }
            else
            {
               LastSign = (V>0) ? 1 : -1;
               BDCPoints.push_back({TimeEx,V});
               LastBDC = TimeEx;
               //std::cout << T << ' ' << V << '\n'; // << R.a << ' ' << R.b << ' ' << R.c << '\n';
               LastEnergy = V*V;
               Found = true;
            }
         }

         // clear the buffer
         SampleBuf.clear();
         CurrentMax = 0;
         CurrentMaxTime = Time;
      }
   }
   return Found;
}
