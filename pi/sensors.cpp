
#include "sensors.h"

std::map<uint16_t, SensorInfoType> SensorFromUID;

std::vector<SensorInfoType> SensorFromBell(16);

void ReadSensorInfo(json const& j)
{
   for (json::const_iterator it = j.begin(); it != j.end(); ++it)
   {
      SensorInfoType S(it.value());
      std::cout << it.value() << '\n';
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
     GyroQuadratic(DefaultGyroQuadratic),
     AccelTransformation({{1.0,0.0}, {0.0,1.0}}),
     AccelOffset({0.0,0.0})//,

//AccelBDC(0.0)
{
}

SensorInfoType::SensorInfoType(json const& j)
   : UID(std::stoul(j.value("UID", "0x0000"), 0, 0)),
     BatteryScale(j.value("BatteryScale", 1.0)),
     Bell(j.value("Bell", -1)),
     Polarity(j.value("Polarity", 1)),
     GyroScale(j.value("GyroScale", 57.1429)),
     GyroQuadratic(j.value("GyroQuadratic", 0)),
     AccelTransformation(j.value("AccelTransformation",
                                 std::vector<std::vector<double>>({{1.0,0.0},{0.0,1.0}}))),
     AccelOffset(j.value("AccelOffset", std::vector<double>{0.0,0.0})),
     AccelBDC(j.value("AccelBDC", 0.0))
{
}
