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
#include "common/trace.h"

namespace prog_opt = boost::program_options;

int main(int argc, char** argv)
{
   try
   {
      std::string ThisBell = "";
      bool bdc = false;
      bool Raw = false;
      bool AbsoluteTime = false;

      prog_opt::options_description desc("Allowed options");
      desc.add_options()
         ("help", "show this help message")
         ("bell", prog_opt::value(&ThisBell), "show output only for this bell")
         ("bdc", prog_opt::bool_switch(&bdc), "emit bottom dead centre times, instead of strike times")
         ("raw", prog_opt::bool_switch(&Raw), "raw mode; read data in two columns <time> <z>")
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
      std::cerr << "Reading bell configurations\n";
      std::ifstream BellsConfig("bells.json");
      json Bells;
      BellsConfig >> Bells;
      LoadBellsJSON(Bells);

      std::cout.precision(16);
      std::cerr.precision(16);
      int64_t Epoch = 0;

      std::map<std::string, GyroBDC> BDC;

      std::string s;
      while (std::getline(std::cin, s))
      {
         //         TRACE(s);
         std::istringstream In(s);
         int64_t Tm;
         float z;
         char G;
         std::string Bell = ThisBell;
         if (!Raw)
         {
            In >> Tm >> Bell >> G;

            //if (Epoch == 0)
            //Epoch = Tm;

            if (G != 'G')
               continue;

            if (!ThisBell.empty() && ThisBell != Bell)
               continue;

            In >> z;
         }
         else
         {
            In >> Tm >> z;
         }

         if (BDC[Bell].Process(Tm-Epoch, z))
         {
            int64_t T = std::get<0>(BDC[Bell].BDCPoints.front());
            double V = std::get<1>(BDC[Bell].BDCPoints.front());
            double C = std::get<2>(BDC[Bell].BDCPoints.front());
            BDC[Bell].BDCPoints.pop_front();

            bool Handstroke = V<0;

            int64_t StrikeTime = T;

            if (!bdc)
            {
               BellInfoType const& B = BellByName(Bell);
               // if we're below the cutoff then quit
               if (!ThisBell.empty() && Handstroke && std::abs(V) < B.HandstrokeCutoff)
               {
                  std::cerr << "Ignoring handstroke bell " << Bell << " velocity " << V << " too low.\n";
                  continue;
               }
               if (!ThisBell.empty() && !Handstroke && std::abs(V) < B.BackstrokeCutoff)
               {
                  std::cerr << "Ignoring backstroke bell " << Bell << " velocity " << V << " too low.\n";
                  std::cerr << B.BackstrokeCutoff << '\n';
                  continue;
               }
               //TRACE(T);
               T += 1000*(Handstroke ? B.HandstrokeDelay_ms
                          : B.BackstrokeDelay_ms);
               //TRACE(T);
            }
            std::cout << (AbsoluteTime ? (T+Epoch) : T) << ' ' << Bell << ' ' << V << ' ' << C << '\n';
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
