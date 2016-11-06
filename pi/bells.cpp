
#include "bells.h"

std::vector<BellInfoType> BellInfo(16);

BellInfoType::BellInfoType(json const& j) : BellInfoType(j["Bell"], j)
{
}

BellInfoType::BellInfoType(int BellNumber_, json const& j)
   : Bell(BellNumber_),
     FriendlyName(j["FriendlyName"].get<std::string>()),
     HandstrokeDelay_ms(j["HandstrokeDelay"]),
     BackstrokeDelay_ms(j["HandstrokeDelay"]),
     HandstrokeDelay(boost::posix_time::milliseconds(int64_t(j["HandstrokeDelay"]))),
     BackstrokeDelay(boost::posix_time::milliseconds(int64_t(j["BackstrokeDelay"]))),
     HandstrokeCutoff(j["HandstrokeCutoff"]),
     BackstrokeCutoff(j["BackstrokeCutoff"]),
     ThetaR(j.value("BackstrokeCutoff", 0.0)),
     Omega(j.value("Omega", 0.0)),
     WheelRadius(j.value("WheelRadius", 0.0))
{
}

void
ReadBellInfo(json const& j)
{
   for (json::const_iterator it = j.begin(); it != j.end(); ++it)
   {
      BellInfoType Bell(it.value());
      std::cout << it.value() << '\n';
      if (Bell.Bell < 0 || Bell.Bell >= BellInfo.size())
      {
         std::cerr << "warning: ignoring out of range bell number " << Bell.Bell << '\n';
      }
      else
         BellInfo[Bell.Bell] = Bell;
   }
}
