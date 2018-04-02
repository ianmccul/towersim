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
#include <fstream>
#include "sensors.h"
#include <boost/program_options.hpp>
#include "common/matvec/matvec.h"
#include "common/prog_opt_accum.h"
#include "common/trace.h"

namespace prog_opt = boost::program_options;

// for output
struct MsgTypes
{
   static unsigned char const Gyro = 'G';
   // float DegreesPerSecond

   static unsigned char const GyroCalibration = 'H';
   // float GyroZeroValue
   // float GyroScaleFactor

   static unsigned char const GyroTemp = 'T';
   // int8_t GyroTemp

   static unsigned char const Status = 'S';
   // int16_t AccelODR
   // int16_t GyroODR
   // int8_t GyroBW
   // uint8_t Power (bool)
   // uint8_t Charging (bool)
   // uint8_t Sleeping (bool)

   static unsigned char const BatteryV = 'B';
   // float BatteryVoltage (battery, in Volts)

   static unsigned char const Accel = 'A';
   // float Ax in m/s^2
   // float Ay in m/s^2

   static unsigned char const BellAvail = 'E';
   // no payload required
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

void WriteGyroCalibrationMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, float GyroOffset, float GyroScale)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::GyroCalibration;
   *static_cast<float*>(static_cast<void*>(Buf+10)) = GyroOffset;
   *static_cast<float*>(static_cast<void*>(Buf+14)) = GyroScale;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+sizeof(float));
}

std::array<double, 16> GyroDiff{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
std::array<double, 16> GyroLast{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void WriteGyroMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, float z)
{
   GyroDiff[Bell] = std::max(GyroDiff[Bell], std::abs(GyroLast[Bell]-z));
   GyroLast[Bell] = z;

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

void WriteBatteryVMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, float V)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::BatteryV;
   *static_cast<float*>(static_cast<void*>(Buf+10)) = V;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+sizeof(float));
}

void WriteAccelMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, float Ax, float Ay)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::Accel;
   *static_cast<float*>(static_cast<void*>(Buf+10)) = Ax;
   *static_cast<float*>(static_cast<void*>(Buf+14)) = Ay;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+4+4);
}

void WriteBellAvailMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int8_t Bell)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::BellAvail;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1);
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
         WriteGyroCalibrationMsg(WriteToFile, Clients, Time, Bell, GyroOffset, SensorFromBell[Bell].GyroScale);
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

   float ZCal = float((z - GyroOffset) / SensorFromBell[Bell].GyroScale) * SensorFromBell[Bell].Polarity;
   WriteGyroMsg(WriteToFile, Clients, Time, Bell, ZCal);
}


namespace std
{
template <typename T>
std::ostream& operator<<(std::ostream& Out, std::vector<T> const& v)
{
   for (auto x : v)
   {
      Out << x << ' ';
   }
   Out << '\n';
   return Out;
}
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
         if (Time - int64_t(std::round((GyroMeasurements.size()-i-1) * GyroDelta)) == 1479609706936045ull)
         {
            TRACE("Got the packet")(Time)(int(SeqNum));
            TRACE(GyroMeasurements);
            abort();
         }
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

std::array<int, 16> BellSeqNum {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

int main(int argc, char** argv)
{
   try
   {
      std::string InFile;
      std::string OutFile;
      int Verbose;

      prog_opt::options_description desc("Allowed options");
      desc.add_options()
         ("help", "show this help message")
         ("infile", prog_opt::value(&InFile), "read input from a file rather than a socket")
         ("outfile", prog_opt::value(&OutFile), "direct output to a file rather than a socket")
         ("verbose,v", prog_opt_ext::accum_value(&Verbose), "verbose output")
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

      // load the sensor configuration
      std::cout << "Reading sensor configurations\n";
      std::ifstream SensorsConfig("sensors.json");
      json Sensors;
      SensorsConfig >> Sensors;

      ReadSensorInfo(Sensors["Sensors"]);

      // mapping of stage 1 channel number to bell number
      std::vector<int8_t> BellNumber(16, -1);
      // UID of the given channel number
      std::vector<uint16_t> SensorUID(16, -1);

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
               // send the 'B' messages to indicate which bells we have available
               for (unsigned i = 0; i < 16; ++i)
               {
                  if (BellNumber[i] != -1)
                  {
                     std::set<int> TempC;  TempC.insert(cl);
                     WriteBellAvailMsg(WriteToFile, TempC, 0, BellNumber[i]);
                  }
               }
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
               for (int i = 1; i <= 12; ++i)
               {
                  std::cout << "Gyro " << i << " max change " << GyroDiff[i] << '\n';
               }
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
         int PipeNumber = buf[8];
         int Bell = BellNumber[PipeNumber];
         uint16_t Delay = *static_cast<uint16_t const*>(static_cast<void const*>(buf+9+4));
         unsigned char Flags = buf[11+4];

         if (PipeNumber > 11)
         {
            // invalid pipe - this should't happen, but for bugs in stage 1...
            std::cerr << "Invalid pipe " << PipeNumber << '\n';
            continue;
         }

         if (Flags & 0x80)
         {
            // status packet
            uint16_t uid = *static_cast<int16_t const*>(static_cast<void const*>(buf+12+4));
            if (Bell == -1 || uid != SensorFromBell[Bell].UID)
            {
               // associate the pipe number with the correct bell number, by looking up the sensor UID.
               // It is also possible that pipe assignments have changed.  This might have happened if a pipe has been
               // changed on a sensor but we didn't restart the server.
               Bell = SensorFromUID[uid].Bell;
               BellNumber[PipeNumber] = Bell;
               std::cerr << "Associating sensor " << std::hex << uid << " with pipe " << std::dec << PipeNumber
                         << " and bell " << std::dec << Bell << "\n";
               if (Bell != -1)
               {
                  WriteBellAvailMsg(WriteToFile, Clients, Time-Delay, Bell);
               }
            }
            if (Bell == -1)
            {
               // if we still don't have a bell number, then we can't do anything useful
               if (Verbose > 1)
                  std::cerr << "Discarding packet with no associated bell number from pipe " << PipeNumber << '\n';
               continue;
            }
            int16_t AccODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+14+4));
            int16_t GyroODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+16+4));
            int8_t GyroBW = *static_cast<int8_t const*>(static_cast<void const*>(buf+18+4));
            bool Power = Flags & 0x20;
            bool Charging = Flags & 0x10;
            bool Sleeping = Flags & 0x02;
            WriteStatusMsg(WriteToFile, Clients, Time-Delay, Bell, uid, AccODR, GyroODR, GyroBW,
			   Power, Charging, Sleeping);
            int Offset = 19+4;
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
               WriteBatteryVMsg(WriteToFile, Clients, Time-Delay, Bell, float(V / SensorFromBell[Bell].BatteryScale));
            }
         }
         else
         {
            if (Bell == -1)  // can't do anything yet, we don't know what bell we have
            {
               if (Verbose > 1)
                  std::cerr << "Discarding packet with no associated bell number from pipe " << PipeNumber << '\n';
               continue;
            }
            // sensor packet
            uint8_t SeqNum = buf[12+4];

            int NumAccel = (Flags & 0x70) >> 4;
            int NumGyro = Flags & 0x0F;

            int ExpectedPacketLength = NumAccel*6 + NumGyro*2 + 4 + 1 + 8 + 4;

            if (len != ExpectedPacketLength)
            {
               std::cerr << Time << " bell " << Bell << " pipe " << PipeNumber
			 << " unexpected packet length " << int(len) << " expected "
                         << ExpectedPacketLength << ", NumAccel=" << NumAccel << ", NumGyro=" << NumGyro
                         << " Delay=" << Delay << " Flags=" << std::hex << uint16_t(Flags) << " SeqNum="
			 << std::dec << uint16_t(SeqNum) << "\n";
               if (Verbose > 2)
               {
                  std::cerr << "Seq numbers: ";
                  for (int i = 1; i <= 12; ++i)
                  {
                     std::cerr << BellSeqNum[i] << ' ';
                  }
                  std::cerr << '\n';
                  std::cerr << "Packet: ";
                  for (int i = 0; i < len; ++i)
                  {
                     std::cerr << int(buf[i]) << ' ';
                  }
                  std::cerr << '\n';
               }
               continue;
            }

            if (Verbose > 0 && BellSeqNum[Bell] != -1 && SeqNum != uint8_t(BellSeqNum[Bell]+1))
            {
               int LostPackets = uint8_t(SeqNum-uint8_t(BellSeqNum[Bell])-1);
               if (LostPackets > (Verbose > 2 ? 0 : (Verbose > 1 ? 1 : 5)))
               {
                  std::cerr << Time << " Packet loss bell " << Bell << " pipe " << PipeNumber << " delay " << Delay
                            << " packets " << LostPackets
                            << " last seq " << int(BellSeqNum[Bell]) << " next seq " << int(SeqNum) << '\n';
               }

               if (Verbose > 2)
               {
                  std::cerr << "Seq numbers: ";
                  for (int i = 1; i <= 12; ++i)
                  {
                     std::cerr << BellSeqNum[i] << ' ';
                  }
                  std::cerr << '\n';
                  if (int(uint8_t(SeqNum-uint8_t(BellSeqNum[Bell])-1)) == 254)
                  {
                     std::cerr << "Packet: ";
                     for (int i = 0; i < len; ++i)
                     {
                        std::cerr << int(buf[i]) << ' ';
                     }
                     std::cerr << '\n';
                  }
               }
            }

            BellSeqNum[Bell] = SeqNum;

            std::vector<int16_t> GyroMeasurements(NumGyro);
            std::memcpy(GyroMeasurements.data(), buf+13+NumAccel*6, NumGyro*2);

         if (Time-Delay == 1479609706946571ull)
         {
            std::cerr << "got the packet " << Time << '\n';
            for (unsigned i = 0; i < len; ++i)
            {
               std::cerr << int(buf[i]) << ' ' << '\n';
            }
         }



            GyroList[Bell].ProcessStream(WriteToFile, Clients, Time-Delay, SeqNum, GyroMeasurements);

            std::vector<vector3<int16_t>> AccelMeasurements(NumAccel);
            std::memcpy(AccelMeasurements.data(), buf+13, NumAccel*6);
            for (auto const& x : AccelMeasurements)
            {
               float Ax = (x[0] - SensorFromBell[Bell].AXOffset) / SensorFromBell[Bell].AXScale;
               float Ay = (x[1] - SensorFromBell[Bell].AYOffset) / SensorFromBell[Bell].AYScale;
               WriteAccelMsg(WriteToFile, Clients, Time-Delay, Bell, Ax, Ay);
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
