#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <set>
#include <cstddef>
#include <vector>
#include <boost/program_options.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
Program to calculate an average of accelerometer readings, for calibration.  Uses the stage 2 data.
*/


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
};

double mean(std::vector<double> const& v)
{
   double sum = 0;
   for (double x : v)
   {
      sum += x;
   }
   return sum / v.size();
}

double stdev(std::vector<double> const& v, double mean)
{
   if (v.size() <= 1)
      return 0;
   double sumsq = 0;
   for (double x : v)
   {
      sumsq += std::pow(x-mean, 2);
   }
   return std::sqrt(sumsq / (v.size()-1));
}

double covariance(std::vector<double> const& x, double xmean, std::vector<double> const& y, double ymean)
{
   if (x.size() <= 1)
      return 0;
   std::assert(x.size() == y.size());
   double covar = 0;
   for (unsigned i = 0; i < x.size(); ++i)
   {
      covar += (x[i] - xmean) * (y[i] - ymean);
   }
   return covar /

int main(int argc, char** argv)
{
   try
   {
      std::string InFile;
      int RunLength = 100;
      int WhichBell = 0;

      prog_opt::options_description desc("Allowed options");
      desc.add_options()
         ("help", "show this help message")
         ("length", prog_opt::value(&RunLength), "number of samples to average over [100]")
         ("bell", prog_opt::value(&WhichBell), "bell number to sample [default is first bell number to appear in the data]")
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

      std::vector<double> AxVec;
      std::vector<double> AyVec;

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
         if (WhichBell == 0)
            WhichBell = Bell;
         if (MsgType == MsgTypes::Accel && Bell == WhichBell)
         {
            float Ax = *static_cast<float const*>(static_cast<void const*>(buf+10));
            float Ay = *static_cast<float const*>(static_cast<void const*>(buf+14));
            AxVec.push_back(Ax);
            AyVec.push_back(Ay);
            if (AxVec.size() == RunLength)
            {
               double AxMean = mean(AxVec);
               double AyMean = mean(AyVec);
               double AxStdev = stdev(AxVec, AxMean);
               double AyStdev = stdev(AyVec, AyMean);
               std::cout << "\nXaccel = " << AxMean << " +- " << AxStdev
                         << "\nYaccel = " << AyMean << " +- " << AyStdev
                         << "\nXWcovariance = " << covariance(AxVec, AxMean, AyVec, AyMean) << std::endl;
               AxVec.clear();
               AyVec.clear();
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
