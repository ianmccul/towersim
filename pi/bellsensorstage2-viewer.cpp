#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <set>
#include <cstddef>
#include "common/matvec/matvec.h"
#include <boost/program_options.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace prog_opt = boost::program_options;

struct MsgTypes
{
   static unsigned char const Gyro = 'G';
   static unsigned char const GyroCalibration = 'H';
   static unsigned char const GyroTemp = 'T';
   static unsigned char const Status = 'S';
   static unsigned char const BatteryV = 'B';
   static unsigned char const Accel = 'A';
   static unsigned char const BellAvail = 'B';
};

int main(int argc, char** argv)
{
   try
   {
      std::string InFile;

      prog_opt::options_description desc("Allowed options");
      desc.add_options()
         ("help", "show this help message")
         ("infile", prog_opt::value(&InFile), "read input from a file rather than a socket")
         ;

      prog_opt::options_description opt;
      opt.add(desc);

      prog_opt::variables_map vm;
      prog_opt::store(prog_opt::command_line_parser(argc, argv).
                      options(opt).run(), vm);
      prog_opt::notify(vm);

      if (vm.count("help"))
      {
         std::cerr << "usage: bellsensorstage2 [options]\n";
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

      unsigned char buf[100];

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
         int Bell = *static_cast<int8_t const*>(static_cast<void const*>(buf+8));
         unsigned char MsgType = buf[9];
         if (MsgType == MsgTypes::Gyro)
         {
            float z = *static_cast<float const*>(static_cast<void const*>(buf+10));
            std::cout << Time << ' ' << Bell <<  " G " << z << '\n';
         }
         else if (MsgType == MsgTypes::Accel)
         {
            float Ax = *static_cast<float const*>(static_cast<void const*>(buf+10));
            float Ay = *static_cast<float const*>(static_cast<void const*>(buf+14));
            std::cout << Time << ' ' << Bell << " A " << Ax << ' ' << Ay << '\n';
         }
         else if (MsgType == MsgTypes::GyroCalibration)
         {
            float Offset = *static_cast<float const*>(static_cast<void const*>(buf+10));
            std::cout << Time << ' ' << Bell << " H " << Offset << '\n';
         }
         else if (MsgType == MsgTypes::Status)
         {
            uint16_t uid = *static_cast<uint16_t const*>(static_cast<void const*>(buf+10));
            int16_t AccODR = *static_cast<uint16_t const*>(static_cast<void const*>(buf+12));
            int16_t GyroODR = *static_cast<uint16_t const*>(static_cast<void const*>(buf+14));
            int8_t GyroBW = *static_cast<uint16_t const*>(static_cast<void const*>(buf+16));
            bool Power = *static_cast<uint8_t const*>(static_cast<void const*>(buf+17));
            bool Charging = *static_cast<uint8_t const*>(static_cast<void const*>(buf+18));
            bool Sleeping = *static_cast<uint8_t const*>(static_cast<void const*>(buf+19));
            std::cout << Time << ' ' << Bell << " S " << std::hex << uid << std::dec << ' ' << AccODR
                      << ' ' << GyroODR << ' ' << int(GyroBW) << ' ' << Power << ' ' << Charging << ' ' << Sleeping << '\n';
         }
         else if (MsgType == MsgTypes::GyroTemp)
         {
            int8_t T = *static_cast<int8_t const*>(static_cast<void const*>(buf+10));
            std::cout << Time << ' ' << Bell << " T " << int(T) << '\n';
         }
         else if (MsgType == MsgTypes::BatteryV)
         {
            float V = *static_cast<float const*>(static_cast<void const*>(buf+10));
            std::cout << Time << ' ' << Bell << " B " << V << '\n';
         }
         else if (MsgType == MsgTypes::BellAvail)
         {
            std::cout << Time << ' ' << Bell << " E" << '\n';
         }
         else
         {
            std::cerr << "Unknown packet " << int(MsgType) << " at " << Time << '\n';
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
