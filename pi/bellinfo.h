
#if !defined(BELLINFO_H)
#define BELLINFO_H

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "json.hpp"

using json = nlohmann::json;

struct BellInfoType
{
   BellInfoType() : BellNumber(0) {}
   BellInfoType(int BellNumber_, std::string const& FriendlyName_, int HDelay, int BDelay)
      : BellNumber(BellNumber_),
        FriendlyName(FriendlyName_),
        HandstrokeDelay_ms(HDelay),
        BackstrokeDelay_ms(BDelay),
        HandstrokeDelay(boost::posix_time::milliseconds(HDelay)),
        BackstrokeDelay(boost::posix_time::milliseconds(BDelay))
   {
      if (FriendlyName.empty())
         FriendlyName = std::to_string(BellNumber);
   }

   int BellNumber;
   std::string FriendlyName;
   // delays from bottom dead centre
   int HandstrokeDelay_ms;
   int BackstrokeDelay_ms;
   boost::posix_time::time_duration HandstrokeDelay;
   boost::posix_time::time_duration BackstrokeDelay;
};

#endif
