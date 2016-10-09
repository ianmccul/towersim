// -*- C++ -*- $Id: gyrofit-daemon.cpp 1891 2016-10-03 06:05:03Z uqimccul $
//
// daemon for a 'bottom dead centre' sensor via gyro, using
// a quadratic fit.  By inspection, we judge a reasonable fitting
// procedure is about 0.15 seconds of data, approx 100 - 130 samples.
//
// We use a circular_buffer that can hold BufMax samples (around 100 - 130).
// If the maximum velocity (absolute value) is greater than Threshold (around 1000 - 2000),
// and occurs in the middle of the buffer (by time), then we treat this as a local extremum
// and do a quadratic fit to find the location of the extremum and the velocity.

#include "asio_handler.h"
#include "sensortcpserver.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <cmath>
#include "gyro-bdc.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "json.hpp"
#include <fstream>
#include <string>

using json = nlohmann::json;

boost::posix_time::time_duration BellDelayHand;
boost::posix_time::time_duration BellDelayBack;

bool HandstrokePositive = false;

int const MaxBells = 16;
std::vector<GyroBDC> BDC(16);

struct BellInfoType
{
   BellInfoType() : BellNumber(0) {}
   BellInfoType(int BellNumber_, std::string const& FriendlyName_, int HDelay, int BDelay)
      : BellNumber(BellNumber_),
        FriendlyName(FriendlyName_),
        HandstrokeDelay(boost::posix_time::milliseconds(HDelay)),
        BackstrokeDelay(boost::posix_time::milliseconds(BDelay))
   {
      if (FriendlyName.empty())
         FriendlyName = std::to_string(BellNumber);
   }

   int BellNumber;
   std::string FriendlyName;
   // delays from bottom dead centre
   boost::posix_time::time_duration HandstrokeDelay;
   boost::posix_time::time_duration BackstrokeDelay;
};

struct SensorInfoType
{
   int Bell;
   int Polarity;
};

// map from actual bell number to BellInfoType
std::map<int, BellInfoType> BellInfo;

// map from raw bell number to SensorInfoType
std::map<int,SensorInfoType> SensorInfo;


void Process(SensorTCPServer& MyServer, std::vector<char> const& Buf)
{
   int64_t Time = *static_cast<int64_t const*>(static_cast<void const*>(Buf.data()));
   char PacketType = Buf[8];
   if (PacketType != 'G')
      return;
   int Bell = Buf[9];
   float z = *static_cast<float const*>(static_cast<void const*>(Buf.data()+10));

   if (BDC[Bell].Process(Time, z))
   {
      int64_t T = BDC[Bell].BDCPoints.front().first;
      double V = BDC[Bell].BDCPoints.front().second;

      int ActualBell = SensorInfo[Bell].Bell;
      bool Handstroke = (V>0) ^ (SensorInfo[Bell].Polarity == -1);

      std::cout << "TRIGGER BELL " << ActualBell << " at " << T << " V: " << V << (Handstroke ? " Handstroke" : " Backstroke") << '\n';

      boost::posix_time::ptime StrikeTime = boost::posix_time::from_time_t(T/1000000) +
         boost::posix_time::microseconds(T % 1000000);
      StrikeTime += Handstroke ? BellInfo[ActualBell].HandstrokeDelay : BellInfo[ActualBell].BackstrokeDelay;
      MyServer.TriggerSensor(BellInfo[ActualBell].FriendlyName, StrikeTime);

      BDC[Bell].BDCPoints.pop_front();
   }
}

int main(int argc, char** argv)
{
   // load the bells configuration
   std::cout << "Reading bell configurations\n";
   std::ifstream BellsConfig("bells.json");
   json Bells;
   BellsConfig >> Bells;
   for (json::iterator it = Bells.begin(); it != Bells.end(); ++it)
   {
      int Bell = std::stoi(it.key());
      BellInfo[Bell] = BellInfoType(Bell, it.value()["FriendlyName"], it.value()["HandstrokeDelay"], it.value()["BackstrokeDelay"]);
      std::cout << it.key() << " : " << it.value() << "\n";
   }

   // load the sensor configuration
   std::cout << "Reading sensor configurations\n";
   std::ifstream SensorsConfig("sensors.json");
   json Sensors;
   SensorsConfig >> Sensors;

   for (json::iterator it = Sensors.begin(); it != Sensors.end(); ++it)
   {
      SensorInfo[std::stoi(it.key())] = {it.value()["Bell"], it.value()["Polarity"]};
      std::cout << it.key() << " : " << it.value() << "\n";
   }

   boost::asio::io_service io;
   PacketHandler Sensor(io);
   SensorTCPServer MyServer(io, "0.0.0.0", "5700");

   MyServer.Attach("StJ:5", 0, 0);
   MyServer.Attach("StJ:6", 0, 0);
   Sensor.Connect(std::string("\0bellsensordaemonsocket", 23));

   // And start the loop
   Sensor.AsyncRead(boost::bind(&Process, boost::ref(MyServer), _1));

   // run the io server to wait for triggers
   io.run();

   return 0;
}