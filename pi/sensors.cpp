
#include "sensors.h"

std::map<uint16_t, SensorInfoType> SensorFromUID;

std::vector<SensorInfoType> SensorFromBell(16);

void ReadSensorInfo(json const& j)
{
   for (json::const_iterator it = j.begin(); it != j.end(); ++it)
   {
      SensorInfoType S(it.value());
      std::cout << it.value() << '\n';
      std::cout << S.UID << ' ' << S.Bell << '\n';
      if (S.UID != 0)
         SensorFromUID[S.UID] = S;
      if (S.Bell >= 0)
      {
         if (S.Bell >= SensorFromBell.size())
         {
            std::cerr << "warning: ignoring out-of-range bell " << S.Bell << " attached to sensor " << std::hex << S.UID << '\n';
         }
         else
            SensorFromBell[S.Bell] = S;
      }
   }
}

SensorInfoType::SensorInfoType()
   : UID(0),
     BatteryScale(0),
     Bell(-1),
     Polarity(0),
     GyroScale(DefaultGyroScale),
     AXScale(DefaultAccScale),
     AYScale(DefaultAccScale),
     AXOffset(0),
     AYOffset(0)
{
}

SensorInfoType::SensorInfoType(json const& j)
   : UID(std::stoul(j.value("UID", "0x0000"), 0, 0)),
     BatteryScale(j.value("BatteryScale", 1.0)),
     Bell(j.value("Bell", -1)),
     Polarity(j.value("Polarity", 1)),
     GyroScale(j.value("GyroScale", 57.1429)),
     AXScale(j.value("AXScale", 1670.7)),
     AYScale(j.value("AYScale", 1670.7)),
     AXOffset(j.value("AXOffset", 0.0)),
     AYOffset(j.value("AYOffset", 0.0))
{
}
