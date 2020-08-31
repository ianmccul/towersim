// -*- C++ -*- $Id: sensorparser.h 1959 2017-05-22 12:10:04Z uqimccul $

#if !defined(SENSORPARSER_H_SHDCWEIUTYWUINLSD)
#define SENSORPARSER_H_SHDCWEIUTYWUINLSD

#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include <boost/variant.hpp>

// sensor commands
struct AttachSensor
{
   std::string Sensor;
   int HandAdjust;
   int BackAdjust;

   AttachSensor() : HandAdjust(0), BackAdjust(0) {}

   AttachSensor(std::string const& S) : Sensor(S) {}
   AttachSensor(std::string const& S, int H, int B) : Sensor(S), HandAdjust(H), BackAdjust(B) {}
};

struct DetachSensor
{
   std::string Sensor;

   DetachSensor() {}

   DetachSensor(std::string const& S) : Sensor(S) {}
};

struct TriggerSensor
{
   enum StrokeType { Hand, Back, None };
   std::string Sensor;
   boost::posix_time::ptime At;
   StrokeType Stroke;

   TriggerSensor() {}
   TriggerSensor(std::string const& Sensor_,
                 boost::posix_time::ptime At_, StrokeType Stroke_ = None)
      : Sensor(Sensor_), At(At_), Stroke(Stroke_) {}
};

struct CommandError
{
   CommandError() {}
   CommandError(std::string const& What_) : What(What_) {}
   std::string What;
};

typedef boost::variant<AttachSensor, 
		       DetachSensor, 
		       TriggerSensor, 
		       CommandError> SensorInputCommand;

SensorInputCommand ParseSensorCommand(std::string const& Str);

#endif
