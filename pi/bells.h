// -*- C++ -*-

#if !defined(TOWERSIM_PI_BELLS_H)
#define TOWERSIM_PI_BELLS_H

#include <string>
#include <cstdint>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "json.hpp"

using json = nlohmann::json;

struct BellInfoType
{
   BellInfoType() : Bell(0), HandstrokeDelay_ms(0), BackstrokeDelay_ms(0), HandstrokeCutoff(0),
                    BackstrokeCutoff(0), ThetaR(0), Omega(0), WheelRadius(0) {}
   BellInfoType(int BellNumber_, json const& j);
   BellInfoType(json const& j);

   int Bell;
   std::string FriendlyName;
   // delays from bottom dead centre
   int64_t HandstrokeDelay_ms;
   int64_t BackstrokeDelay_ms;
   boost::posix_time::time_duration HandstrokeDelay;
   boost::posix_time::time_duration BackstrokeDelay;
   double HandstrokeCutoff;
   double BackstrokeCutoff;
   double ThetaR;
   double Omega;
   double WheelRadius;
};

// The BellInfo indexed by bell number
extern std::vector<BellInfoType> BellInfo;

void ReadBellInfo(json const& j);

#endif
