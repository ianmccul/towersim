// -*- C++ -*-

// A particle filter for processing gyro readings

#include "bootstrapfilter.h"
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include "common/randutil.h"
#include <fstream>
#include <boost/program_options.hpp>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


namespace prog_opt = boost::program_options;

struct MsgTypes
{
   static unsigned char const Gyro = 'G';
   static unsigned char const GyroCalibration = 'H';
   static unsigned char const GyroTemp = 'T';
   static unsigned char const Status = 'S';
   static unsigned char const BatteryV = 'B';
   static unsigned char const Accel = 'A';
   static unsigned char const BellAvail = 'E';
   static unsigned char const AccelAngle = 'X';
};

timeval Epoch;

double GainFactor = 1; // 0.99844; // 1.0;
double GyroQuadraticCorrection = 0; // -0.000164831; //0;

int main(int argc, char** argv)
{
   try
   {
      std::string InFile;
      int Bell = -1;
      bool Raw = false;
      double InitialOffset = 0;
      bool Allow_l_b = true;
      double ForceWidth = 150;
      int NumParticles = 2000;

      prog_opt::options_description desc("Allowed options");
      desc.add_options()
         ("help", "show this help message")
         ("bell", prog_opt::value(&Bell), "show output for this bell [default first bell in stream]")
         ("offset", prog_opt::value(&InitialOffset), "initial gyro offset (degrees)")
         ("raw", prog_opt::bool_switch(&Raw), "raw mode; read data in two columns <time> <z>")
         ("force", prog_opt::value(&ForceWidth), "width of the force distribution")
         ("infile", prog_opt::value(&InFile), "read input from a file rather than a socket")
         ("gyroscale", prog_opt::value(&GainFactor), "gyro scale factor")
         ("gyroquad", prog_opt::value(&GyroQuadraticCorrection), "gyro quadratic correction")
         ("particles", prog_opt::value(&NumParticles), "number of particles")
         ;

      prog_opt::options_description opt;
      opt.add(desc);

      prog_opt::variables_map vm;
      prog_opt::store(prog_opt::command_line_parser(argc, argv).
                      options(opt).run(), vm);
      prog_opt::notify(vm);

      if (vm.count("help"))
      {
         std::cerr << "usage: particlefilter [options]\n";
         std::cerr << desc << "\n";
         return 1;
      }

      // the input file descriptor
      bool const ReadFromFile = !InFile.empty();

      int infd = -1;
      if (ReadFromFile)
      {
         infd = open(InFile.c_str(), O_RDONLY);
         if (infd < 0)
         {
            perror("Cannot open input file");
            exit(1);
         }
      }
      else
      {
         // construct the socket
         std::string SocketPath{"\0bellsensordaemonsocket", 23};

         infd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
         if (infd < 0)
         {
            perror("socket error");
            exit(1);
         }

         struct sockaddr_un addr;
         memset(&addr, 0, sizeof(struct sockaddr_un));
         addr.sun_family = AF_LOCAL;
         std::copy(SocketPath.begin(), SocketPath.end(), addr.sun_path);

         if (connect(infd, (struct sockaddr*)&addr,
                     SocketPath.size() + offsetof(sockaddr_un, sun_path)) == -1)
         {
            perror("connect error");
            exit(1);
         }
      }

      std::ifstream BellsConfig("bells.json");
      json Bells;
      BellsConfig >> Bells;
      LoadBellsJSON(Bells);

      randutil::seed();

      ParticleFilter Filter;

      BellInfoType ThisBell;
      if (Bell != -1)
      {
         ThisBell = BellByName(std::to_string(Bell));
         Filter.Initialize(ThisBell, NumParticles, to_rad(InitialOffset)); //, 0, 0);
         Filter.Adjust_l_b = Allow_l_b;
         Filter.ForceWidth = ForceWidth;
      }

      //ThisBell.kb = 0;

      unsigned char buf[100];

      double LastGyro = 0;

      while(1)
      {
         // read a message
         uint8_t len = 0;
         if (ReadFromFile)
         {
            int r = read(infd, &len, 1);
            if (r == 0)
            {
               std::cerr << "reached end of file.\n";
               return 0;
            }
            if (r == 1)
               r = read(infd, buf, len);
            if (r != len)
            {
               perror("Error reading from file");
               return 1;
            }
         }
         else
         {
            // read from the socket
            len = read(infd, buf, sizeof(buf));
            if (len < 0)
            {
               std::cerr << "read failed.\n";
               return 1;
            }
            else if (len == 0)
            {
               std::cerr << "server closed.\n";
               return 0;
            }
         }

         int64_t Time = *static_cast<int64_t const*>(static_cast<void const*>(buf));
         int MsgBell = *static_cast<int8_t const*>(static_cast<void const*>(buf+8));
         if (Bell == -1)
         {
            Bell = MsgBell;
            ThisBell = BellByName(std::to_string(Bell));
            Filter.Initialize(ThisBell, NumParticles, to_rad(InitialOffset)); //, 0, 0);
            Filter.Adjust_l_b = Allow_l_b;
            Filter.ForceWidth = ForceWidth;
         }
         if (MsgBell != Bell)
            continue;
         unsigned char MsgType = buf[9];
         if (MsgType == MsgTypes::Gyro)
         {
            float z = *static_cast<float const*>(static_cast<void const*>(buf+10));
            float ofs = *static_cast<float const*>(static_cast<void const*>(buf+14));
            double gg = to_rad(z+ofs);
            gg += to_rad(2.26781);
            gg *= GainFactor;
            gg += GyroQuadraticCorrection * gg*gg;
            LastGyro = gg;
            Filter.ProcessGyro(Time, gg);
         }
         else if (MsgType == MsgTypes::AccelAngle)
         {
            float Theta;
            std::memcpy(&Theta, buf+10, sizeof(Theta));
            Filter.ProcessAccel(Time, to_rad(Theta));
         }
         else
            continue;

         double t = Filter.StateEstimate<&Particle::Theta>();
         double v = Filter.StateEstimate<&Particle::Velocity>();
         double f = Filter.StateEstimate<&Particle::Force>();

         double ForceOnRope = f / RopeToBellCutoff(to_rad(ThisBell.Gamma), t);

         double lb = Filter.StateEstimate<&Particle::l_b>();
         double energy = 0.5*v*v*lb - g*std::cos(t);

         double offset = Filter.StateEstimate<&Particle::GyroOffset>();

         double KE = 0.5*v*v*lb;
         double PE = -g*std::cos(t);

         double RKE = RopeKineticEnergy(ThisBell, t, v);
         double RPE = RopePotentialEnergy(ThisBell, t, v);

         double StayForce = ForceFromStay(ThisBell, t, v);

         std::cout << Time << ' ' << to_deg(t) << ' ' << to_deg(v)
                   << ' ' << to_deg(LastGyro-offset)
                   << ' ' << to_deg(f)
                   << ' ' << to_deg(ForceOnRope)
                   << ' ' << to_deg(StayForce)
                   << ' ' << to_deg(KE+PE+RKE+RPE)
                   << ' ' << to_deg(KE)
                   << ' ' << to_deg(PE)
                   << ' ' << to_deg(RKE)
                   << ' ' << to_deg(RPE)
                   << ' ' << to_deg(offset)
                   << ' ' << lb
                   << std::endl;
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
