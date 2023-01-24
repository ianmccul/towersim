// daemon to read from UDP port 5702, which is the arduino sensor information,
// and relay that into a bottom-dead-centre TCP server running on localhost (TCP port 5700)


#include "sensortcpserver.h"
#include "asio_udp_receiver.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <cmath>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "bells.h"
#include <fstream>
#include <string>

using json = nlohmann::json;

void Process(SensorTCPServer& MyServer, std::vector<char> const& Buf)
{
   boost::posix_time::ptime Time = boost::posix_time::microsec_clock::local_time();
   for (auto c : Buf)
   {
      BellInfoType const& b = BellByName(std::string(1, c));

      boost::posix_time::ptime StrikeTime = Time + (b.HandstrokeDelay() + b.BackstrokeDelay()) / 2;
      MyServer.TriggerSensor(b.FriendlyName, StrikeTime);
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

   boost::asio::io_context io;
   UDPReceiver Sensor(io, 5702);
   SensorTCPServer MySensorServer(io, "0.0.0.0", "5700");

   // attach the bells to the server.
   for (auto const& x : AllBells())
   {
      MySensorServer.Attach(x.FriendlyName, 0, 0);
   }

   // And start the loop
   Sensor.AsyncRead(boost::bind(&Process, boost::ref(MySensorServer), _1));

   // run the io server to wait for triggers
   io.run();

   return 0;
}
