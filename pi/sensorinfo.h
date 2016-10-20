
#if !defined(SENSORINFO_H)

#include "json.hpp"

using json = nlohmann::json;

struct SensorInfoType
{
   int Bell;
   int Polarity;
};

#endif
