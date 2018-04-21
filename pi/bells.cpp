
#include "bells.h"
#include "common/trace.h"

std::vector<BellInfoType> BellInfo;

std::set<BellInfoType> BellInfoSet;

BellInfoType::BellInfoType(json const& j)
   : Number(j["Number"].get<int>()),
     Name(j["Name"].get<std::string>()),
     FriendlyName(j["FriendlyName"].get<std::string>()),
     HandstrokeDelay_ms(j.value("HandstrokeDelay", 300)),
     BackstrokeDelay_ms(j.value("HandstrokeDelay", 300)),
     HandstrokeCutoff(j.value("HandstrokeCutoff", 0.0)),
     BackstrokeCutoff(j.value("BackstrokeCutoff", 0.0)),
     HandstrokeStay(j.value("HandstrokeStay", 186.0)),
     BackstrokeStay(j.value("BackstrokeStay", -186.0)),
     ThetaR(j.value("ThetaR", 0.2)),
     Gamma(j.value("Gamma", -135.0)),
     WheelRadius(j.value("WheelRadius", 0.75)),
     lb(j.value("lb", 0.72)),
     kb(j.value("kb", 0.0)),
     Y(j.value("Y", 240))
{
}

void to_json(json& j, BellInfoType const& b)
{
   j = json{{"Number", b.Number},
            {"Name", b.Name},
            {"FriendlyName", b.FriendlyName},
            {"HandstrokeDelay", b.HandstrokeDelay_ms},
            {"BackstrokeDelay", b.BackstrokeDelay_ms},
            {"HandstrokeCutoff", b.HandstrokeCutoff},
            {"BackstrokeCutoff", b.BackstrokeCutoff},
            {"HandstrokeStay", b.HandstrokeStay},
            {"BackstrokeStay", b.BackstrokeStay},
            {"ThetaR", b.ThetaR},
            {"Gamma", b.Gamma},
            {"WheelRadius", b.WheelRadius},
            {"lb", b.lb},
            {"kb", b.kb},
            {"Y", b.Y}};
}

void from_json(json const& j, BellInfoType& b)
{
   b = BellInfoType(j);
}

void LoadBellsJSON(json const& j)
{
   for (auto const& J : j["Bells"])
   {
      BellInfoType b(J);
      BellInfoSet.insert(b);
      while (b.Number >= BellInfo.size())
      {
         BellInfo.push_back(BellInfoType());
      }
      BellInfo[b.Number] = b;
   }
}

BellInfoType& BellByName(std::string const& Name)
{
   for (auto& i : BellInfo)
   {
      if (i.Name == Name)
         return i;
   }
}

BellInfoType& BellByNumber(int n)
{
   CHECK(n >= 0 && n < BellInfo.size());
   return BellInfo[n];
}


std::set<BellInfoType> const& AllBells()
{
   return BellInfoSet;
}
