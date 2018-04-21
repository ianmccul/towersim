// -*- C++ -*-

#if !defined(TOWERSIM_PI_SENSORS_H)
#define TOWERSIM_PI_SENSORS_H

// Data for each sensor

// Field            Type       Description
// UID              uint16     unique ID of the sensor
// Bell             integer    bell number that this sensor is attached to
// Polarity         integer    +/- 1
// GyroScale        float      Gyroscope scaling coefficient
// AccelCalibration 2x8 matrix matrix of accelerometer readings

#include <cstdint>
#include "json.hpp"

using json = nlohmann::json;

double const DefaultGyroScale = 57.1429;
double const DefaultAccScale = 1670.7; // 16384/g

struct SensorInfoType
{
   SensorInfoType();
   SensorInfoType(json const& j);

   uint16_t UID;
   double BatteryScale;
   int Bell;
   int Polarity;
   double GyroScale;
   double AXScale;
   double AYScale;
   double AXOffset;
   double AYOffset;
};

// The SensorInfo indexed by bell number
extern std::vector<SensorInfoType> SensorFromBell;

// The SensorInfo indexed by UID
extern std::map<uint16_t, SensorInfoType> SensorFromUID;

void ReadSensorInfo(json const& j);

#endif
