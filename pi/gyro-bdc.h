// -*- C++ -*- $Id: gyro-bdc.h 1876 2016-10-02 13:46:09Z uqimccul $
//
// A 'bottom dead centre' sensor via gyro, using
// a polynomial fit.
// For a quadratic polynomial, the fit interval needs to be fairly small, about 0.05 seconds.
// Better to do a quartic fit, which can be done over a larger interval, around 0.3 seconds.

// We use a moving window of data, and require that the fitted value is approximately
// in the middle of the window.

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <cmath>
#include <boost/circular_buffer.hpp>
#include <list>
#include "quadfit.h"
#include "quarticfit.h"
#include "common/trace.h"
#include <fstream>
#include <tuple>

constexpr double pi = M_PI;

double const ThresholdVelocity = 15;   // degrees per second, ignore anything less than this

//#define QUADRATIC_FIT

#if defined(QUADRATIC_FIT)
int64_t const BufferWidthMicrosec = 300000/4;  // 0.3 seconds
int64_t const SamplingRateMicroSec = 100000/4; // 0.1 seconds
#else
int64_t const BufferWidthMicrosec = 150000;  // 0.15 seconds
int64_t const SamplingRateMicroSec = BufferWidthMicrosec/5;
#endif

// Stay check if we get a false positive for small velocity when the bell is up.
// If an event has a higher squared velocity (ie energy) then we always accept it.
// if it has a lower energy then only accept it if the change in energy is smaller than
// EnergyThreshold.  After 10 seconds, we reset the energy meter so we need to avoid
// false positives before the first real strike.  For that, we rely on the ThresholdVelocity.
constexpr double EnergyThreshold = 60000;

constexpr double SampleRate = 760; // sampling rate in Hz
constexpr int MinBufferSamples = 50; // require at least this many samples before fitting
constexpr int64_t EventMinDelay = 150000; // 0.15 seconds minimum delay between events

// After ResetTime seconds since a BDC trigger, we accept a BDC even
// if the EnergyThreshold isn't met.
// This is mostly in case something goes wrong.  10 million microseconds = 10 seconds
int64_t const ResetTime = 10000000;

class GyroBDC
{
   public:
      GyroBDC();

      GyroBDC(int Bell_);

      // the BDC records are the time, velocity and ratio of C/V
      typedef std::tuple<int64_t, double, double> VelocityPoint;

      std::list<VelocityPoint> BDCPoints;

      // Processes a sample.  Returns true if a BDC event has been detected, in which case
      // it is added to the output list
      bool Process(int64_t Time, float z);

   private:
      bool TryRoot(quartic_fit_result const& Res, double T);
      bool FitBufferQuadratic();
      bool FitBufferQuartic();

      boost::circular_buffer<std::pair<int64_t, float>> SampleBuf;
      int64_t BuffStart;
      int64_t LastEvent;  // microseconds time of the last minima/maxima event
      double LastEnergy;

      int Bell;
};

inline
GyroBDC::GyroBDC()
   : SampleBuf(BufferWidthMicrosec*760/1000+100),
     BuffStart(0),
     LastEvent(0),
     LastEnergy(0),
     Bell(0)
{
}

inline
GyroBDC::GyroBDC(int Bell_)
   : SampleBuf(BufferWidthMicrosec*760/1000+100),
     BuffStart(0),
     LastEvent(0),
     LastEnergy(0),
     Bell(Bell_)
{
}

namespace std
{
inline
std::ostream& operator<<(std::ostream& Out, std::vector<double> const& v)
{
   for (auto x : v)
   {
      Out << x << ' ';
   }
   Out << '\n';
   return Out;
}
}

std::ofstream MyFile("vlog.dat");

inline
bool
GyroBDC::Process(int64_t Time, float z)
{
   //   TRACE(z);
   bool Found = false;
   if (SampleBuf.empty())
      BuffStart = Time;

   SampleBuf.push_back({Time, z});

   // if we've filled the buffer then attempt a fit
   if (Time > BuffStart + BufferWidthMicrosec)
   {
      // paranoid check that we have enough data, or if we are just starting and don't have
      // enough data yet
      if (SampleBuf.size() < MinBufferSamples)
      {
         //  std::cerr << "Severe packet loss, discarding possible BDC at " << Time << '\n';
         BuffStart = Time - BufferWidthMicrosec - SamplingRateMicroSec;
      }
      else
      {
#if defined(QUADRATIC_FIT)
         Found = this->FitBufferQuadratic();
#else
         Found = this->FitBufferQuartic();
#endif
      }

      if (Found)
      {
         // found, move the buffer forward to the next possible window
         BuffStart = LastEvent+EventMinDelay-SamplingRateMicroSec/2;
      }
      else
      {
         // not found, move the buffer window forward
         BuffStart += SamplingRateMicroSec;
      }
   }

   // remove unneeded samples from the buffer
   while (!SampleBuf.empty() && SampleBuf.front().first < BuffStart)
   {
      SampleBuf.pop_front();
   }

   return Found;
}

bool
GyroBDC::FitBufferQuadratic()
{
   // Assemble the data for a quadratic fit
   std::vector<double> x;
   std::vector<double> y;
   for (auto c : SampleBuf)
   {
      x.push_back((c.first - BuffStart) / 1.0E6);
      y.push_back(c.second);
   }
   quadratic_fit_result R = quadratic_fit(x,y);

   // Quadratic is ax^2 + bx + c
   // The minima is at 2ax+b = 0, so x = -b/(2a)
   // the value of the quadratic at the minima is b^2/(4a) - b^2/(2a) + c = c-b^2/(4a)
   double T = -R.b / (2*R.a);

   // we expect that the minima will be within SamplingRateMicroSec
   // of the centre of the buffer, but allow some leeway.
   if (std::abs(T - BufferWidthMicrosec*0.5e-6) > (SamplingRateMicroSec*1.1e-6))
      return false;

   // make sure the velocity meets the threshold
   double V = R.c - R.b*R.b/(4*R.a);
   if (std::abs(V) < ThresholdVelocity)
      return false;

   // When the bell is freely swinging, the ratio of the curvature to the velocity
   // is equal to -g/l_b.  The bell might not be freely swinging, eg if it is being chimed,
   // and we don't know l_b here, but a valid range would be around
   // 0.1 to 10 from the smallest to largest bells in existence.  So check that the
   // ratio is < -0.1 and > -100
   double CurvatureRatio = 2*R.a / R.c;
   if (CurvatureRatio > -0.1 || CurvatureRatio < -100)
      return false;

   int64_t EventTime = int64_t(round(T*1E6)) + BuffStart;

   // Energy check
   double Energy = V*V;
   if (Energy < LastEnergy-EnergyThreshold && EventTime < LastEvent+10000000LL)
      return false;

   // we have a valid minima
   LastEvent = EventTime;
   LastEnergy = Energy;
   BDCPoints.push_back({EventTime,V, CurvatureRatio});

   //   std::cout << "Quadratic found " << LastEvent << ' ' << V << '\n';

   //   MyFile << LastEvent << ' ' << V << ' ' << CurvatureRatio << '\n';
   return true;
}


bool
GyroBDC::TryRoot(quartic_fit_result const& Res, double T)
{
   // velocity at the root
   double V = Res.a*std::pow(T,4) + Res.b*std::pow(T,3) + Res.c*std::pow(T,2) + Res.d*T + Res.e;

   // make sure the velocity meets the threshold
   if (std::abs(V) < ThresholdVelocity)
      return false;

   // curvataure of the root is the second derivative evaluated at T
   // Second derivative is 12*a*x^2 + 6*b*x + 2*c
   double C = 12*Res.a*std::pow(T,2) + 6*Res.b*T + 2*Res.c;

   // When the bell is freely swinging, the ratio of the curvature to the velocity
   // is equal to -g/l_b.  The bell might not be freely swinging, eg if it is being chimed,
   // and we don't know l_b here, but a valid range of l_b would be around
   // 0.1 to 10 from the smallest to largest bells in existence.  So check that the
   // ratio is < -0.1 and > -100
   // TODO: there was a typo here, the ratio limit is actually -1 to -100, need to check this!
   double CurvatureRatio = C / V;
   //   if (CurvatureRatio > -0.1 || CurvatureRatio < -100)
   //      return false;

   int64_t EventTime = int64_t(round(T*1E6)) + BuffStart;

   // Energy check
   double Energy = V*V;
   if (Energy < (LastEnergy-EnergyThreshold) && EventTime < LastEvent+10000000LL)
      return false;

   // we have a valid minima
   LastEvent = EventTime;
   LastEnergy = Energy;
   BDCPoints.push_back({EventTime,V,CurvatureRatio});

   //std::cout << T << ' ' << Res.a << ' ' << Res.b << ' ' << Res.c << ' ' << Res.d << ' ' << Res.e << '\n';

   //   std::cout << EventTime << ' ' << V << ' ' << C << ' ' << '\n';

   //   MyFile << EventTime << ' ' << V << ' ' << CurvatureRatio << '\n';

   //MyFile << LastEvent << ' ' << V << ' ' << (Res.a / Res.c) << ' ' << Res.r << '\n';
   return true;
}

bool
GyroBDC::FitBufferQuartic()
{
   // Assemble the data for a quartic fit
   std::vector<double> x;
   std::vector<double> y;
   for (auto c : SampleBuf)
   {
      x.push_back((c.first - BuffStart) / 1.0E6);
      y.push_back(c.second);
   }
   quartic_fit_result Res = quartic_fit(x,y);

   // Quadratic is ax^4 + bx^3 + cx^2 + dx + e
   // The minima is when the derivative is zero, so
   // 4ax^3 + 3bx^2 + 2cx + d = 0,
   // write this in the form of
   // x^3 + a2*x^2 + a1*x + a0
   double a2 = 3.0*Res.b / (4.0*Res.a);
   double a1 = 2.0*Res.c / (4.0*Res.a);
   double a0 = Res.d / (4.0*Res.a);

   // we expect that the 4-th order term will have the opposite sign to the 2nd order term,
   // so it increases slower than
   // a quadratic away from the minimum.  Hence it should have 3 real roots.
   // this means that the discriminant should be positive.
   double Delta = 18*a2*a1*a0 - 4*std::pow(a2,3)*a0 + std::pow(a2*a1,2) - 4*std::pow(a1,3) - 27*a0*a0;
   if (Delta <= 0)
      return false;

   // find the roots, from http://mathworld.wolfram.com/CubicFormula.html
   double Q = (3.0*a1 - a2*a2) / 9.0;
   double R = (9.0*a2*a1 - 27.0*a0 - 2*std::pow(a2,3)) / 54.0;

   //std::cout << Q << ' ' << R << '\n';

   double sQ = std::sqrt(-Q);
   double sQ3 = std::sqrt(-std::pow(Q,3));
   double Ratio = std::min(R / sQ3, 1.0);
   double TT = std::acos(Ratio);

   // the roots
   double x1 = 2*sQ * std::cos(TT/3) - a2/3.0;
   double x2 = 2*sQ * std::cos((TT + 2*pi)/3) - a2/3.0;
   double x3 = 2*sQ * std::cos((TT + 4*pi)/3) - a2/3.0;

   bool Found = false;
   double T = 0;
   // find a root that lies within the buffer
   // if we find a root it seems to be almost always root 3, so check this one first.
   if (!Found && std::abs(x3 - BufferWidthMicrosec*0.5e-6) <= (SamplingRateMicroSec*0.5*1.1e-6))
   {
      Found = this->TryRoot(Res, x3);
   }
   if (std::abs(x1 - BufferWidthMicrosec*0.5e-6) <= (SamplingRateMicroSec*0.5*1.1e-6))
   {
      Found = this->TryRoot(Res, x1);
   }
   if (!Found && std::abs(x2 - BufferWidthMicrosec*0.5e-6) <= (SamplingRateMicroSec*0.5*1.1e-6))
   {
      Found = this->TryRoot(Res, x2);
   }

   return Found;
}
