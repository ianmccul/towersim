// -*- C++ -*-
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
#include "bells.h"
#include "sensors.h"
#include <fstream>
#include <string>

using json = nlohmann::json;

int const MaxBells = 16;
std::vector<GyroBDC> BDC(16);

void Process(SensorTCPServer& MyServer, std::vector<char> const& Buf)
{
   int64_t Time = *static_cast<int64_t const*>(static_cast<void const*>(Buf.data()));
   int Bell = Buf[8];
   char PacketType = Buf[9];
   if (PacketType != 'G')
      return;
   float z = *static_cast<float const*>(static_cast<void const*>(Buf.data()+10));

   if (BDC[Bell].Process(Time, z))
   {
//      std::cout << Bell << ' ' << z <<  '\n';

      int64_t T = BDC[Bell].BDCPoints.front().first;
      double V = BDC[Bell].BDCPoints.front().second;
      BDC[Bell].BDCPoints.pop_front();

     // std::cout << V << '\n';

      // positive velocity is backstroke (bell is moving up towards handstroke say)
      // negative velocity is handstroke (bell is moving back towards backstroke stay)

      bool Handstroke = V<0;

      // if we're below the cutoff then quit
      if (Handstroke && std::abs(V) < BellInfo[Bell].HandstrokeCutoff)
      {
         std::cout << "Ignoring handstroke bell " << Bell << " velocity " << V << " too low.\n";
         return;
      }
      if (!Handstroke && std::abs(V) < BellInfo[Bell].BackstrokeCutoff)
      {
         std::cout << "Ignoring backstroke bell " << Bell << " velocity " << V << " too low.\n";
         return;
      }

      std::cout << "TRIGGER BELL " << Bell << ' ' << " at " << T << " V: "
                << V << (Handstroke ? " Handstroke" : " Backstroke") << '\n';

      boost::posix_time::ptime StrikeTime = boost::posix_time::from_time_t(T/1000000) +
         boost::posix_time::microseconds(T % 1000000);
      StrikeTime += Handstroke ? BellInfo[Bell].HandstrokeDelay : BellInfo[Bell].BackstrokeDelay;
      MyServer.TriggerSensor(BellInfo[Bell].FriendlyName, StrikeTime);

   }
}

int main(int argc, char** argv)
{
   // load the bells configuration
   std::cout << "Reading bell configurations\n";
   std::ifstream BellsConfig("bells.json");
   json Bells;
   BellsConfig >> Bells;
   ReadBellInfo(Bells["Bells"]);

   boost::asio::io_service io;
   PacketHandler Sensor(io);
   SensorTCPServer MyServer(io, "0.0.0.0", "5700");

   // attach the bells to the server.
   for (auto const& x : BellInfo)
   {
      MyServer.Attach(x.FriendlyName, 0, 0);
   }

   // connect to the local stage 2 socket
   Sensor.Connect(std::string("\0bellsensordaemonsocket", 23));

   // And start the loop
   Sensor.AsyncRead(boost::bind(&Process, boost::ref(MyServer), _1));

   // run the io server to wait for triggers
   io.run();

   return 0;
}
