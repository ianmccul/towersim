// -*- C++ -*-
//
// gyro and acceleromter parameters, from the datasheets

#if !defined(TOWERSIM_SENSOR_PARAMETERS_H)
#define TOWERSIM_SENSOR_PARAMETERS_H

#include <cmath>

constexpr float pi = M_PI;

//
// accelerometer
//

// Standard deviation of the accelerometer when stationary
// typical value from the datasheet - some sensors are
// bigger than this, especially per axis
constexpr float AccelStdev = 31.74;

// X and Y scale values
constexpr float AxScale = 1670.7;
constexpr float AyScale = 1670.7;

//
// gyro
//

// Bandwidth and noise density from the datasheet, in Hz and dps / sqrt(Hz)
constexpr float GyroBandwidth = 100;
constexpr float GyroNoiseDensity = 0.011;

constexpr float GyroStdev = GyroNoiseDensity * std::sqrt(2*GyroBandwidth);

#endif
