#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <set>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <cmath>
#include <cstring>
#include <boost/program_options.hpp>
#include "common/matvec/matvec.h"

namespace prog_opt = boost::program_options;

// for output
struct MsgTypes
{
   static unsigned char const Gyro = 'G';
   static unsigned char const GyroCalibration = 'H';
   static unsigned char const GyroTemp = 'T';
   static unsigned char const Status = 'S';
   static unsigned char const BatteryV = 'B';
   static unsigned char const Accel = 'A';
};

void WriteMsgToClients(bool WriteToFile, std::set<int>& Clients, unsigned char const* Buf, int Sz)
{
   if (WriteToFile)
   {
      int fd = *Clients.begin();
      uint8_t len = Sz;
      write(fd, &len, 1);
      write(fd, Buf, Sz);
   }
   else
   {
      std::set<int>::iterator I = Clients.begin();
      while (I != Clients.end())
      {
         int c = *I;
         int rc = send(c, Buf, Sz, MSG_NOSIGNAL);
         if (rc <= 0)
         {
            //perror("write");
            std::cerr << "write failure writing to client " << c << std::endl;
            close(c);
            I = Clients.erase(I);
         }
         else
         {
            ++I;
         }
      }
   }
}

//
// gyro calibration
//

// maximum swing values that are still considered to be zero
int const GyroZeroMaxDeviation = 80;

// number of samples required that satisfy GyroZeroMaxDeviation
// to count as a successful zero calibration
int const GyroZeroRequiredSamples = 800;
// require this many samples to also satisfy the threshold before we commit the calibration
int const GyroZeroLagSamples = 800;

void WriteGyroCalibrationMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, float GyroOffset)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::GyroCalibration;
   *static_cast<float*>(static_cast<void*>(Buf+10)) = GyroOffset;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+sizeof(float));
}

void WriteGyroMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, float z)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::Gyro;
   *static_cast<float*>(static_cast<void*>(Buf+10)) = z;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+sizeof(float));
}

void WriteGyroTempMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, int16_t T)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::GyroTemp;
   *static_cast<int16_t*>(static_cast<void*>(Buf+10)) = T;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+sizeof(int16_t));
}

void WriteStatusMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, uint16_t uid, int16_t AccODR,
                    int16_t GryoODR, int8_t GyroBW,
                    bool Power, bool Charging, bool Sleeping)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::Status;
   *static_cast<uint16_t*>(static_cast<void*>(Buf+10)) = uid;
   *static_cast<int16_t*>(static_cast<void*>(Buf+12)) = AccODR;
   *static_cast<int16_t*>(static_cast<void*>(Buf+14)) = GryoODR;
   *static_cast<int8_t*>(static_cast<void*>(Buf+16)) = GyroBW;
   *static_cast<uint8_t*>(static_cast<void*>(Buf+17)) = Power;
   *static_cast<uint8_t*>(static_cast<void*>(Buf+18)) = Charging;
   *static_cast<uint8_t*>(static_cast<void*>(Buf+19)) = Sleeping;
   WriteMsgToClients(WriteToFile, Clients, Buf, 20);
}

void WriteBatteryVMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, uint16_t V)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::BatteryV;
   *static_cast<int16_t*>(static_cast<void*>(Buf+10)) = V;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+sizeof(uint16_t));
}

void WriteAccelMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, vector3<int16_t> const& x)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::Accel;
   *static_cast<vector3<int16_t>*>(static_cast<void*>(Buf+10)) = x;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+6);
}

class GyroProcessor
{
   public:
      GyroProcessor(int Bell_);

      void ProcessStream(bool WriteToFile, std::set<int>& Clients, int64_t Time, uint8_t SeqNum, std::vector<int16_t> const& GyroMeasurements);

   private:
      void ProcessPacket(bool WriteToFile, std::set<int>& Clients, int64_t Time, int16_t z);

      int Bell;

      int16_t GyroMin, GyroMax;
      int32_t GyroAccumulator;
      int GyroCount;

      bool GyroOffsetPending;
      int GyroLagCount;
      float GyroOffsetNext;

      float GyroOffset;

      bool GyroHasZeroCalibration;


      int64_t LastPacketTime;
      static float constexpr GyroDelta = 1e6 / 760.0;
      uint8_t LastSeqNum;

      uint64_t StreamStartTime;

};

GyroProcessor::GyroProcessor(int Bell_)
   : Bell(Bell_),
     GyroMin(INT16_MAX),
     GyroMax(INT16_MIN),
     GyroAccumulator(0),
     GyroCount(0),
     GyroOffsetPending(false),
     GyroLagCount(0),
     GyroOffsetNext(0.0),
     GyroOffset(0.0),
     GyroHasZeroCalibration(false),

     LastPacketTime(0),
     LastSeqNum(0),
     StreamStartTime(0)
{
}

void GyroProcessor::ProcessPacket(bool WriteToFile, std::set<int>& Clients, int64_t Time, int16_t z)
{
   GyroMin = std::min(GyroMin, z);
   GyroMax = std::max(GyroMax, z);
   if (std::abs(GyroMax-GyroMin) > GyroZeroMaxDeviation)
   {
      // reset the counter
      GyroMin = INT16_MAX;
      GyroMax = INT16_MIN;
      GyroAccumulator = 0;
      GyroCount = 0;
      GyroOffsetPending = false;
   }
   else if (GyroOffsetPending)
   {
      ++GyroLagCount;
      if (GyroLagCount >= GyroZeroLagSamples)
      {
         if (GyroHasZeroCalibration)
         {
            // filter step
            GyroOffset = 0.5 * (GyroOffset + GyroOffsetNext);
         }
         else
         {
            GyroOffset= GyroOffsetNext;
            GyroHasZeroCalibration = true;
         }
         WriteGyroCalibrationMsg(WriteToFile, Clients, Time, Bell, GyroOffset);
         // reset the counters
         GyroOffsetPending = false;
         GyroMin = INT16_MAX;
         GyroMax = INT16_MIN;
         GyroAccumulator = 0;
         GyroCount = 0;
      }
   }
   else
   {
      GyroAccumulator += z;
      ++GyroCount;
      if (GyroCount >= GyroZeroRequiredSamples)
      {
         GyroOffsetNext =  GyroAccumulator /float(GyroCount);
         GyroOffsetPending = true;
         GyroLagCount = 0;
      }
   }

   float ZCal = float(z) - GyroOffset;
   WriteGyroMsg(WriteToFile, Clients, Time, Bell, ZCal);
}

void
GyroProcessor::ProcessStream(bool WriteToFile, std::set<int>& Clients, int64_t Time, uint8_t SeqNum, std::vector<int16_t> const& GyroMeasurements)
{
   float Delta = GyroMeasurements.size() * GyroDelta;
   if (LastPacketTime == 0 || (SeqNum - LastSeqNum != 1))
   {
      StreamStartTime = Time;
      LastPacketTime = Time;
      //GyroCount = GyroMeasurements.size();
      // either we are initializing, or packet loss.  Reset the stream.
      for (int i = 0; i < GyroMeasurements.size(); ++i)
      {
         this->ProcessPacket(WriteToFile, Clients, Time - int64_t(std::round((GyroMeasurements.size()-i-1) * GyroDelta)),
                             GyroMeasurements[i]);
      }
      return;
   }
   // continuing on

   // distribute the packets between the time slice
   for (int i = 0; i < GyroMeasurements.size(); ++i)
   {
      int64_t t = LastPacketTime + int64_t(std::round((Time - LastPacketTime) * (float(i) / GyroMeasurements.size())));
      this->ProcessPacket(WriteToFile, Clients, t, GyroMeasurements[i]);
   }
   LastPacketTime = Time;
   LastSeqNum = SeqNum;
}

int main(int argc, char** argv)
{
   try
   {
      std::string InFile;
      std::string OutFile;

      prog_opt::options_description desc("Allowed options");
      desc.add_options()
         ("help", "show this help message")
         ("infile", prog_opt::value(&InFile), "read input from a file rather than a socket")
         ("outfile", prog_opt::value(&OutFile), "direct output to a file rather than a socket")
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

      // initialize the GyroList
      std::vector<GyroProcessor> GyroList;
      for (int i = 0; i < 16; ++i)
      {
         GyroList.push_back(GyroProcessor(i));
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
         std::string InputSocketPath{"\0bellsensordaemonsocketraw", 26};

         infd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
         if (infd < 0)
         {
            perror("socket error");
            exit(1);
         }

         struct sockaddr_un inaddr;
         memset(&inaddr, 0, sizeof(struct sockaddr_un));
         inaddr.sun_family = AF_LOCAL;
         std::copy(InputSocketPath.begin(), InputSocketPath.end(), inaddr.sun_path);

         if (connect(infd, (struct sockaddr*)&inaddr,
                     InputSocketPath.size() + offsetof(sockaddr_un, sun_path)) == -1)
         {
            perror("connect error");
            exit(1);
         }
      }

      unsigned char buf[100];

      // output file descriptor
      int outfd = -1;
      std::set<int> Clients;

      bool const WriteToFile = !OutFile.empty();
      if (WriteToFile)
      {
         int outfd = open(OutFile.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
         if (outfd < 0)
         {
            perror("Cannot open output file");
            exit(1);
         }
         Clients.insert(outfd);
      }
      else
      {
         // construct the socket
         std::string OutputSocketPath("\0bellsensordaemonsocket", 23);

         outfd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_NONBLOCK, 0);
         if (outfd < 0)
         {
            perror("socket error");
            exit(-1);
         }

         struct sockaddr_un outaddr;
         memset(&outaddr, 0, sizeof(struct sockaddr_un));
         outaddr.sun_family = AF_LOCAL;

         std::copy(OutputSocketPath.begin(), OutputSocketPath.end(), outaddr.sun_path);

         if (bind(outfd, (struct sockaddr*)&outaddr,
                  OutputSocketPath.size() + offsetof(sockaddr_un, sun_path)) == -1)
         {
            perror("bind error");
            exit(1);
         }

         if (listen(outfd, 5) == -1)
         {
            perror("listen error");
            exit(1);
         }
      }

      // main loop
      while(1)
      {
         // do we have a client waiting to connect?
         if (!WriteToFile)
         {
            int cl = accept(outfd, NULL, NULL);
            if (cl >= 0)
            {
               std::cout << "Accepted connection " << cl << std::endl;
               if (Clients.empty())
               {
                  // first client, start up the radio.
               }
               Clients.insert(cl);
            }
         }

         // read a message
         uint8_t len;
         if (ReadFromFile)
         {
            read(infd, &len, 1);
            int r = read(infd, buf, len);
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
               std::cout << "read failed.\n";
               return 1;
            }
            else if (len == 0)
            {
               std::cout << "server closed.\n";
               return 0;
            }
         }

         int64_t Time = *static_cast<int64_t const*>(static_cast<void const*>(buf));
         int Bell = buf[8];
         uint16_t Delay = *static_cast<uint16_t const*>(static_cast<void const*>(buf+9));
         unsigned char Flags = buf[11];

         if (Flags & 0x80)
         {
            // status packet
            uint16_t uid = *static_cast<int16_t const*>(static_cast<void const*>(buf+12));
            int16_t AccODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+14));
            int16_t GyroODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+16));
            int8_t GyroBW = *static_cast<int8_t const*>(static_cast<void const*>(buf+18));
            bool Power = Flags & 0x20;
            bool Charging = Flags & 0x10;
            bool Sleeping = Flags & 0x02;
            WriteStatusMsg(WriteToFile, Clients, Time-Delay, Bell, uid, AccODR, GyroODR, GyroBW, Power, Charging, Sleeping);
            int Offset = 19;
            // status packet
            if (Flags & 0x08)
            {
               // we have a temperature
               int8_t T = *static_cast<int8_t const*>(static_cast<void const*>(buf+Offset));
               ++Offset;
               WriteGyroTempMsg(WriteToFile, Clients, Time-Delay, Bell, T);
            }
            if (Flags & 0x04)
            {
               // battery charge
               uint16_t V = *static_cast<uint16_t const*>(static_cast<void const*>(buf+Offset));
               Offset += 2;
               WriteBatteryVMsg(WriteToFile, Clients, Time-Delay, Bell, V);
            }
         }
         else
         {
            // sensor packet
            uint8_t SeqNum = buf[12];

            int NumAccel = (Flags & 0x70) >> 4;
            int NumGyro = Flags & 0x0F;

            int ExpectedPacketLength = NumAccel*6 + NumGyro*2 + 4 + 1 + 8;

            if (len != ExpectedPacketLength)
            {
               std::cerr << "Unexpected packet length " << int(len) << " expected " << ExpectedPacketLength << ", NumAccel=" << NumAccel << ", NumGyro=" << NumGyro
                         << " Delay=" << Delay << " Flags=" << std::hex << uint16_t(Flags) << " SeqNum=" << std::dec << uint16_t(SeqNum) << "\n";
               continue;
            }

            std::vector<int16_t> GyroMeasurements(NumGyro);
            std::memcpy(GyroMeasurements.data(), buf+13+NumAccel*6, NumGyro*2);

            GyroList[Bell].ProcessStream(WriteToFile, Clients, Time-Delay, SeqNum, GyroMeasurements);

            std::vector<vector3<int16_t>> AccelMeasurements(NumAccel);
            std::memcpy(AccelMeasurements.data(), buf+13, NumAccel*6);
            for (auto const& x : AccelMeasurements)
            {
               WriteAccelMsg(WriteToFile, Clients, Time-Delay, Bell, x);
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
