// -*- C++ -*-

#if !defined(TOWERSIM_PI_BELLS_H)
#define TOWERSIM_PI_BELLS_H

// Data for each bell
//
// Field            Type     Description
// Number           integer  integer number of the bell
// Name             string   name of the bell
// FriendlyName     string   name of the bell that appears in the sensor configuration
// HandstrokeDelay  integer  delay from bottom dead centre to bell sound at handstroke (ms)
// BackstrokeDelay  integer  delay from bottom dead centre to bell sound at backstroke (ms)
// HandstrokeCutoff float    cutoff velocity below which the bell doesn't sound when approaching handstroke (dps)
// BackstrokeCutoff float    cutoff velocity below which the bell doesn't sound when approaching backstroke (dps)
// HandstrokeStay   float    angle of the bell when resting against the stay at handstroke (degrees)
// BackstrokeStay   float    angle of the bell when resting against the stay at backstroke (degrees)
// ThetaR           float    shift in angle of the bell due to the rope (degrees)
// Gamma            float    angle of the garter hole relative to BDC (degrees)
// WheelRadius      float    radius of the wheel (meters)
// lb               float    equivalent pendulum length of the bell (meters)
// kb               float    coefficient of friction of the bell (seconds^-1)
// Y                float    Young's modulus of the stay (seconds^-2)
// ks               float    coefficient of friction of the stay (seconds^-1)

// NOTES:
// The HandstrokeCutoff and BackstrokeCutoff are arguably misnamed,
// the HandstrokeCutoff is relevant as the bell is approaching handstroke, meaning the strike
// is the previous backstroke.
//
// By convention, the angle of the bell when over the balance at handstroke is > 180 degrees,
// and < -180 degrees when over the balance at backstroke.  The garter hole angle is negative,
// typically around -135 degrees
//
// ThetaR is positive

#include <string>
#include <cstdint>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "json.hpp"
#include <set>

using json = nlohmann::json;

struct BellInfoType
{
   BellInfoType() : HandstrokeDelay_ms(0), BackstrokeDelay_ms(0), HandstrokeCutoff(0),
                    BackstrokeCutoff(0), ThetaR(0), WheelRadius(0) {}
   BellInfoType(json const& j);

   int         Number;
   std::string Name;
   std::string FriendlyName;
   int         HandstrokeDelay_ms;
   int         BackstrokeDelay_ms;
   double      HandstrokeCutoff;
   double      BackstrokeCutoff;
   double      HandstrokeStay;
   double      BackstrokeStay;
   double      ThetaR;
   double      Gamma;
   double      WheelRadius;
   double      lb;
   double      kb;
   double      Y;

   boost::posix_time::time_duration HandstrokeDelay()
   {
      return boost::posix_time::milliseconds(HandstrokeDelay_ms);
   }
   boost::posix_time::time_duration BackstrokeDelay()
   {
      return boost::posix_time::milliseconds(BackstrokeDelay_ms);
   }
};

inline
bool operator<(BellInfoType const& x, BellInfoType const& y)
{
   return x.Number < y.Number;
}

void to_json(json& j, BellInfoType const& b);

void from_json(json const& j, BellInfoType& b);

void LoadBellsJSON(json const& j);

BellInfoType& BellByName(std::string const& Name);

BellInfoType& BellByNumber(int n);

std::set<BellInfoType> const& AllBells();

#endif
