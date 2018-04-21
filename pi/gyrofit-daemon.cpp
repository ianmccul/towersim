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
#include "bdc-tcpserver.h"
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
std::array<GyroBDC, 16> BDC{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

void Process(SensorTCPServer& MyServer, BDC_TCPServer& BDCServer, std::vector<char> const& Buf)
{
   int64_t Time = *static_cast<int64_t const*>(static_cast<void const*>(Buf.data()));
   int Bell = Buf[8];
   char PacketType = Buf[9];
   if (PacketType != 'G')
      return;
   if (Buf.size() != 8+1+1+sizeof(float))
   {
      std::cerr << "unexpected stage 2 packet length!";
   }
   float z = *static_cast<float const*>(static_cast<void const*>(Buf.data()+10));

   if (BDC[Bell].Process(Time, z))
   {
//      std::cout << Bell << ' ' << z <<  '\n';

      int64_t T = BDC[Bell].BDCPoints.front().first;
      double V = BDC[Bell].BDCPoints.front().second;
      BDC[Bell].BDCPoints.pop_front();

      boost::posix_time::ptime Time = boost::posix_time::from_time_t(T/1000000) +
         boost::posix_time::microseconds(T % 1000000);

      BDCServer.TriggerBDC(Bell, Time, V);

     // std::cout << V << '\n';

      // positive velocity is backstroke (bell is moving up towards handstroke say)
      // negative velocity is handstroke (bell is moving back towards backstroke stay)

      bool Handstroke = V<0;

      // if we're below the cutoff then quit
      if (Handstroke && std::abs(V) < BellByNumber(Bell).HandstrokeCutoff)
      {
         std::cout << "Ignoring handstroke bell " << Bell << " velocity " << V << " too low.\n";
         return;
      }
      if (!Handstroke && std::abs(V) < BellByNumber(Bell).BackstrokeCutoff)
      {
         std::cout << "Ignoring backstroke bell " << Bell << " velocity " << V << " too low.\n";
         return;
      }

      std::cout << "TRIGGER BELL " << Bell << ' ' << " at " << T << " V: "
                << V << (Handstroke ? " Handstroke" : " Backstroke") << '\n';

      boost::posix_time::ptime StrikeTime = Time +
	 (Handstroke ? BellByNumber(Bell).HandstrokeDelay() : BellByNumber(Bell).BackstrokeDelay());
      MyServer.TriggerSensor(BellByNumber(Bell).FriendlyName, StrikeTime);

   }
}

int main(int argc, char** argv)
{
   // load the bells configuration
   std::cout << "Reading bell configurations\n";
   std::ifstream BellsConfig("bells.json");
   json Bells;
   BellsConfig >> Bells;
   LoadBellsJSON(Bells);

   boost::asio::io_service io;
   PacketHandler Sensor(io);
   SensorTCPServer MySensorServer(io, "0.0.0.0", "5700");
   BDC_TCPServer MyBDCServer(io, "0.0.0.0", "5701");

   // attach the bells to the server.
   for (auto const& x : AllBells())
   {
      MySensorServer.Attach(x.FriendlyName, 0, 0);
      //      MyBDCServer.Attach(x.FriendlyName, 0, 0);
   }

   // connect to the local stage 2 socket
   Sensor.Connect(std::string("\0bellsensordaemonsocket", 23));

   // And start the loop
   Sensor.AsyncRead(boost::bind(&Process, boost::ref(MySensorServer), boost::ref(MyBDCServer), _1));

   // run the io server to wait for triggers
   io.run();

   return 0;
}
