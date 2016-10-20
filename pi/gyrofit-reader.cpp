// -*- C++ -*- $Id: gyrofit-reader.cpp 1876 2016-10-02 13:46:09Z uqimccul $
//
// daemon for a 'bottom dead centre' sensor via gyro, using
// a quadratic fit.  By inspection, we judge a reasonable fitting
// procedure is about 0.15 seconds of data, approx 100 - 130 samples.
//
// We use a circular_buffer that can hold BufMax samples (around 100 - 130).
// If the maximum velocity (absolute value) is greater than Threshold (around 1000 - 2000),
// and occurs in the middle of the buffer (by time), then we treat this as a local extremum
// and do a quadratic fit to find the location of the extremum and the velocity.
//
// After testing, we note that it sometimes false-triggers on a stay incident.
// It seems that this is difficult to detect from first principles,
// the curvature / velocity doesn't
// differ significantly from BDC when raising/lowering the bell.  We could make a check
// based on a minimum time between BDC, and/or the change in velocity at BDC.
// It is reasonable to assume that the change in squared velocity (ie, the kinetic energy)
// is smaller than say 2e8.

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <cmath>
#include <boost/circular_buffer.hpp>
#include "gyro-bdc.h"
#include <fstream>
#include "bellinfo.h"
#include "sensorinfo.h"
#include <boost/program_options.hpp>

namespace prog_opt = boost::program_options;

int main(int argc, char** argv)
{
   std::cout.precision(16);
   std::cerr.precision(16);

   std::map<int, BellInfoType> BellInfo;
   std::map<int,SensorInfoType> SensorInfo;

   std::ifstream BellsConfig("bells.json");
   json Bells;
   BellsConfig >> Bells;
   for (json::iterator it = Bells.begin(); it != Bells.end(); ++it)
   {
      int Bell = std::stoi(it.key());
      BellInfo[Bell] = BellInfoType(Bell, it.value()["FriendlyName"], it.value()["HandstrokeDelay"], it.value()["BackstrokeDelay"]);
   }

   // load the sensor configuration
   std::ifstream SensorsConfig("sensors.json");
   json Sensors;
   SensorsConfig >> Sensors;

   for (json::iterator it = Sensors.begin(); it != Sensors.end(); ++it)
   {
      SensorInfo[std::stoi(it.key())] = {it.value()["Bell"], it.value()["Polarity"]};
   }

   try
   {
      bool Relative = false;
      bool MeasureBDC = false;

      prog_opt::options_description desc("Allowed options");
      desc.add_options()
         ("help", "show this help message")
         //("binary", prog_opt::bool_switch(&Raw), "read binary rather than text")
         ("bdc",  prog_opt::bool_switch(&MeasureBDC), "emit bottom-dead-centre times rather than bell strike times")
         ("relative,r",  prog_opt::bool_switch(&Relative), "outpit times relative to the initial time")
         ;

      prog_opt::options_description opt;
      opt.add(desc);

      prog_opt::variables_map vm;
      prog_opt::store(prog_opt::command_line_parser(argc, argv).
                      options(opt).run(), vm);
      prog_opt::notify(vm);

      if (vm.count("help"))
      {
         std::cerr << "usage: gyrofit-reader [options]\n";
         std::cerr << desc << "\n";
         return 1;
      }

      int64_t Epoch = 0;
      int64_t Tm;
      std::string G;
      float z;

      std::vector<GyroBDC> BDC(16);

      std::string s;
      while (std::getline(std::cin, s))
      {
         std::istringstream In(s);
         int64_t Tm;
         char S;
         int Bell;
         In >> Tm >> S >> Bell;
         if (S == 'G')
         {
            double z;
            In >> z;

            //std::cout << S << ' ' << Bell << ' ' << z << '\n';

            z *= SensorInfo[Bell].Polarity;

            if (Epoch == 0 && Relative)
               Epoch = Tm;

            if (BDC[Bell].Process(Tm-Epoch, z))
            {
               uint64_t t = BDC[Bell].BDCPoints.front().first;
               double v = BDC[Bell].BDCPoints.front().second;
               if (!MeasureBDC)
               {
                  t += (v > 0) ? BellInfo[Bell].HandstrokeDelay_ms : BellInfo[Bell].BackstrokeDelay_ms;
               }
               std::cout << t << ' ' << int(Bell) << ' ' << v << '\n';
               BDC[Bell].BDCPoints.pop_front();
            }
         }
      }
   }
   catch (prog_opt::error& e)
   {
      std::cerr << "Exception while processing command line options: " << e.what() << '\n';
      return 1;
   }
   catch(std::exception& e)
   {
      std::cerr << "error: " << e.what() << "\n";
      return 1;
   }
   catch(...)
   {
      std::cerr << "Exception of unknown type!\n";
   }
   return 0;
}
