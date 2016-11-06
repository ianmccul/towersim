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
#include "bells.h"
#include <fstream>
#include <string>
#include <boost/program_options.hpp>

namespace prog_opt = boost::program_options;

int main(int argc, char** argv)
{
   try
   {
      int ThisBell = -1;
      bool bdc = false;

      prog_opt::options_description desc("Allowed options");
      desc.add_options()
         ("help", "show this help message")
         ("bell", prog_opt::value(&ThisBell), "show output only for this bell")
         ("bdc", prog_opt::bool_switch(&bdc), "emit bottom dead centre times, instead of strike times")
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


      // load the bells configuration
      std::cout << "Reading bell configurations\n";
      std::ifstream BellsConfig("bells.json");
      json Bells;
      BellsConfig >> Bells;
      ReadBellInfo(Bells["Bells"]);

      std::cout.precision(16);
      std::cerr.precision(16);
      int64_t Epoch = 0;

      std::array<GyroBDC, 13> BDC;


      std::string s;
      while (std::getline(std::cin, s))
      {
         std::istringstream In(s);
         int64_t Tm;
         float z;
         char G;
         int Bell;
         In >> Tm >> Bell >> G;

         if (Epoch == 0)
            Epoch = Tm;

         if (G != 'G')
            continue;

         if (ThisBell != -1 && ThisBell != Bell)
            continue;
a
         In >> z;

         if (BDC[Bell].Process(Tm-Epoch, z))
         {
            int64_t T = BDC[Bell].BDCPoints.front().first;
            double V = BDC[Bell].BDCPoints.front().second;
            BDC[Bell].BDCPoints.pop_front();

            bool Handstroke = V<0;

            int64_t StrikeTime = T;

            if (!bdc)
            {
               // if we're below the cutoff then quit
               if (Handstroke && std::abs(V) < BellInfo[Bell].HandstrokeCutoff)
               {
                  std::cerr << "Ignoring handstroke bell " << Bell << " velocity " << V << " too low.\n";
                  continue;
               }
               if (!Handstroke && std::abs(V) < BellInfo[Bell].BackstrokeCutoff)
               {
                  std::cerr << "Ignoring backstroke bell " << Bell << " velocity " << V << " too low.\n";
                  continue;
               }

               T += Handstroke ? BellInfo[Bell].HandstrokeDelay_ms : BellInfo[Bell].BackstrokeDelay_ms;
            }
            std::cout << (T + Epoch) << ' ' << Bell << ' ' << V << '\n';
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
