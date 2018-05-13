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
#include <boost/circular_buffer.hpp>
#include "sensor-parameters.h"

namespace prog_opt = boost::program_options;

// number of samples required for an accelerometer tilt measurement
constexpr int AccelNumSamples = 100;

// standard deviation of the accelerometer when stationary.
// Theoretical value is 31.74
constexpr float AccelStdevThreshold = 35.0;

// GyroStdev from the datasheet (in sensor-parameters.h)
constexpr float GyroStdevThreshold = GyroStdev * 1.2;
//constexpr int GyroNumSamples = 800;
constexpr int GyroZeroRequiredSamples = 1600;

// once the gyro is calibrated we can detect if it is close to zero,
// which is when tilt detection on the accelerometer is likely to be effective.
constexpr float GyroZeroThreshold = GyroStdevThreshold * 2;

// fast path - if the raw gyro reading is bigger than this then it is not close to zero
constexpr int16_t GyroZeroMaxDeviation = 200;

// for output
struct MsgTypes
{
   // Gyro messages contain the adjusted gyro reading and the offset.
   // The original gyro reading can be recovered from adjusted+offset.
   // The current version of stage2 doesn't use GyroCalibration messages.
   static unsigned char const Gyro = 'G';
   // float DegreesPerSecond
   // float OffsetDegreesPerSecond

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

   static unsigned char const AccelAngle = 'X';
   // float angle
};

void WriteMsgToClients(bool WriteToFile, std::set<int>& Clients, unsigned char const* Buf, int Sz)
{
   if (WriteToFile)
   {
      int fd = *Clients.begin();
      uint8_t len = Sz;
      int r = write(fd, &len, 1);
      r = write(fd, Buf, Sz);
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

void debug_packet(unsigned char const* buf, int len, std::ostream& out)
{
   std::ios::fmtflags f(out.flags());
   if (len > 41)
   {
      out << "packet too long (" << len << "), truncating ";
      len = 41;
   }
   for (int i = 0; i < len; ++i)
   {
      out << std::hex << "0x" << int(buf[i]) << ' ';
   }
   out.flags(f);
   out << std::endl;
}

//
// gyro calibration
//

// debugging duplicate packets
std::array<unsigned char[41], 16> LastBuf;
std::array<int, 16> LastBufSize{};

void WriteGyroCalibrationMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time,
                             int Bell, float GyroOffset, float GyroScale)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::GyroCalibration;
   std::memcpy(Buf+10, &GyroOffset, sizeof(float));
   std::memcpy(Buf+14, &GyroScale, sizeof(float));
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+sizeof(float));
}

std::array<double, 16> GyroDiff{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
std::array<double, 16> GyroLast{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void WriteGyroMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time,
                  int Bell, float z, float offset)
{
   GyroDiff[Bell] = std::max(GyroDiff[Bell], std::abs(GyroLast[Bell]-z));
   GyroLast[Bell] = z;

   unsigned char Buf[100];
   std::memcpy(Buf, &Time, sizeof(Time));
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::Gyro;
   std::memcpy(Buf+10, &z, sizeof(float));
   std::memcpy(Buf+14, &offset, sizeof(float));
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+2*sizeof(float));
}

void WriteGyroTempMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, int16_t T)
{
   unsigned char Buf[100];
   std::memcpy(Buf, &Time, sizeof(Time));
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::GyroTemp;
   *static_cast<int16_t*>(static_cast<void*>(Buf+10)) = T;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+sizeof(int16_t));
}

void WriteStatusMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell,
                    uint16_t uid, int16_t AccODR,
                    int16_t GyroODR, int8_t GyroBW,
                    bool Power, bool Charging, bool Sleeping)
{
   unsigned char Buf[100];
   std::memcpy(Buf, &Time, sizeof(Time));
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::Status;
   std::memcpy(Buf+10, &uid, sizeof(uid));
   std::memcpy(Buf+12, &AccODR, sizeof(AccODR));
   std::memcpy(Buf+14, &GyroODR, sizeof(GyroODR));
   *static_cast<int8_t*>(static_cast<void*>(Buf+16)) = GyroBW;
   *static_cast<uint8_t*>(static_cast<void*>(Buf+17)) = Power;
   *static_cast<uint8_t*>(static_cast<void*>(Buf+18)) = Charging;
   *static_cast<uint8_t*>(static_cast<void*>(Buf+19)) = Sleeping;
   WriteMsgToClients(WriteToFile, Clients, Buf, 20);
}

void WriteBatteryVMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, float V)
{
   unsigned char Buf[100];
   std::memcpy(Buf, &Time, sizeof(Time));
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::BatteryV;
   *static_cast<float*>(static_cast<void*>(Buf+10)) = V;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+sizeof(float));
}

void WriteAccelMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, float Ax, float Ay)
{
   unsigned char Buf[100];
   std::memcpy(Buf, &Time, sizeof(Time));
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::Accel;
   *static_cast<float*>(static_cast<void*>(Buf+10)) = Ax;
   *static_cast<float*>(static_cast<void*>(Buf+14)) = Ay;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+4+4);
}

void WriteAngleMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int Bell, float Theta)
{
   unsigned char Buf[100];
   std::memcpy(Buf, &Time, sizeof(Time));
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::AccelAngle;
   std::memcpy(Buf+10, &Theta, sizeof(Theta));
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1+4);
}

void WriteBellAvailMsg(bool WriteToFile, std::set<int>& Clients, int64_t Time, int8_t Bell)
{
   unsigned char Buf[100];
   std::memcpy(Buf, &Time, sizeof(Time));
   *static_cast<int8_t*>(static_cast<void*>(Buf+8)) = Bell;
   Buf[9] = MsgTypes::BellAvail;
   WriteMsgToClients(WriteToFile, Clients, Buf, 8+1+1);
}

class GyroProcessor
{
   public:
      GyroProcessor(int Bell_);

      void ProcessStream(bool WriteToFile, std::set<int>& Clients, int64_t Time, uint8_t SeqNum,
                         std::vector<int16_t> const& GyroMeasurements, uint16_t GyroSampleNumber, int Verbose);

      void ProcessAccel(bool WriteToFile, std::set<int>& Clients, int64_t Time, int16_t Ax, int16_t Ay);

   private:
      void ProcessPacket(bool WriteToFile, std::set<int>& Clients, int64_t Time, int16_t z);

      int Bell;

      boost::circular_buffer<float> GyroBuffer;

      // true if we've had one complete buffer of steady samples, so collect another buffer for the actual offset
      bool GyroOffsetPending;

      // true if we've collected a good buffer for the next gyro offset.  Wait one more buffer worth
      // before actually setting the offset.
      bool GyroOffsetNext;

      // if GyroOffsetNext is true, then NextGyroOffset is valid
      float NextGyroOffset;

      // true if the gyro is reading close to zero
      bool GyroNearZero;

      float GyroOffset;

      int64_t LastSampleTime;  // computed time of the last sample (might be different from actual time due to dejitter)
      double GyroTimestep;     // timestep of the gyro samples, in microseconds
      //      uint8_t LastSeqNum;      // The sequence number of the last-seen packet
      uint16_t LastSampleNum;  // The sample number of the last-seen packet
      double GyroSampleWeighting; // number of previous samples to weight the GyroTimestep
      constexpr static double GyroSampleWeightingMax = 100000;  // maximum value of GyroSampleWeighting

      boost::circular_buffer<std::pair<int16_t, int16_t>> AccelBuffer;

      int ShowNext;
};

GyroProcessor::GyroProcessor(int Bell_)
   : Bell(Bell_),
     GyroBuffer(GyroZeroRequiredSamples+10),
     GyroOffsetPending(false),
     GyroOffsetNext(false),
     NextGyroOffset(0.0),
     GyroNearZero(false),
     GyroOffset(0.0),

     LastSampleTime(0),
     GyroTimestep(1e6 / 760.0),
     //LastSeqNum(0),
     LastSampleNum(0),
     GyroSampleWeighting(100),

     AccelBuffer(AccelNumSamples+10),

     ShowNext(0)
{
}

void GyroProcessor::ProcessAccel(bool WriteToFile, std::set<int>& Clients, int64_t Time, int16_t Ax, int16_t Ay)
{
   if (!GyroNearZero)
   {
      AccelBuffer.clear();
      return;
   }

   AccelBuffer.push_back(std::make_pair(Ax, Ay));

   if (AccelBuffer.size() >= AccelNumSamples)
   {
      int32_t x = 0, y = 0;
      for (auto const& c : AccelBuffer)
      {
         x += c.first;
         y += c.second;
      }
      float AX = float(x) / AccelBuffer.size();
      float AY = float(y) / AccelBuffer.size();
      float x2=0, y2=0;
      for (auto const& c : AccelBuffer)
      {
         x2 += std::pow(c.first - AX, 2);
         y2 += std::pow(c.second - AY, 2);
      }

      float XStdev = std::sqrt(float(x2) / AccelBuffer.size());
      float YStdev = std::sqrt(float(y2) / AccelBuffer.size());

      //std::cout << XStdev << ' ' << YStdev << '\n';

      if (XStdev < AccelStdevThreshold && YStdev < AccelStdevThreshold)
      {
         float Axc = SensorFromBell[Bell].AccelTransformation[0][0]*(AX/AxScale)
            + SensorFromBell[Bell].AccelTransformation[0][1]*(AY/AyScale)
            + SensorFromBell[Bell].AccelOffset[0];
         float Ayc = SensorFromBell[Bell].AccelTransformation[1][0]*(AX/AxScale)
            + SensorFromBell[Bell].AccelTransformation[1][1]*(AY/AyScale)
            + SensorFromBell[Bell].AccelOffset[1];

         // Flip the signs if we have the opposite polarity, this amounts
         // to adding 180 degrees to the angle, on the assimption that polarity -1
         // has the sensor power button facing the same way, but on the other side of the bell.
         // We also need to negate the angle (which is the real effect of the polarity).
         Axc *= SensorFromBell[Bell].Polarity;
         Ayc *= SensorFromBell[Bell].Polarity;
         float Theta = atan2(Axc, Ayc)*SensorFromBell[Bell].Polarity*180/pi - SensorFromBell[Bell].AccelBDC;

         // normalize Theta to [-180,180]
         Theta = std::fmod(std::fmod(Theta+180, 360)+720, 360)-180;

         WriteAngleMsg(WriteToFile, Clients, Time, Bell, Theta);

         AccelBuffer.clear();
      }
      else
      {
         while (AccelBuffer.size() > AccelNumSamples)
            AccelBuffer.pop_front();
      }
   }
}

void GyroProcessor::ProcessPacket(bool WriteToFile, std::set<int>& Clients, int64_t Time, int16_t z)
{
   // zero offset detection
   bool ShouldClearBuffer = false;

   // fast path if the gyro reading isn't close to zero
   if (std::abs(z) > GyroZeroMaxDeviation)
   {
      GyroOffsetPending = false;
      GyroOffsetNext = false;
      ShouldClearBuffer = true;
   }
   else
   {
      GyroBuffer.push_back(z);

      if (GyroBuffer.size() >= GyroZeroRequiredSamples)
      {
         int32_t gsum = 0;
         for (auto x : GyroBuffer)
         {
            gsum += x;
         }
         float g = float(gsum) / (GyroBuffer.size() * SensorFromBell[Bell].GyroScale);

         float g2sum = 0;
         for (auto x : GyroBuffer)
         {
            g2sum += std::pow(x/SensorFromBell[Bell].GyroScale - g,2);
         }
         float gstdev = std::sqrt(g2sum / GyroBuffer.size());

         if (gstdev < GyroStdevThreshold)
         {
            //std::cout << Bell << ' ' << gstdev << ' ' << GyroOffsetPending << '\n';
            if (GyroOffsetPending)
            {
               NextGyroOffset = g;
               GyroOffsetPending = false;
               GyroOffsetNext = true;
            }
            else if (GyroOffsetNext)
            {
               if (GyroOffset == 0)
                  GyroOffset = NextGyroOffset;
               else
                  GyroOffset = 0.5 * (GyroOffset + NextGyroOffset);
               GyroOffsetNext = false;
            }
            else
            {
               GyroOffsetPending = true;
            }
            ShouldClearBuffer = true;
         }
         else
         {
            GyroOffsetPending = false;
            GyroOffsetNext = false;
         }
      }
   }

   if (ShouldClearBuffer)
   {
      GyroBuffer.clear();
   }
   else
   {
      while (GyroBuffer.size() >= GyroZeroRequiredSamples)
      {
         GyroBuffer.pop_front();
      }
   }

   float ZCal = (float(z) / SensorFromBell[Bell].GyroScale) * SensorFromBell[Bell].Polarity;
   float OffsetCal = GyroOffset * SensorFromBell[Bell].Polarity;
   GyroNearZero = (std::abs(ZCal-OffsetCal) < GyroZeroThreshold) && (GyroOffset != 0);
   WriteGyroMsg(WriteToFile, Clients, Time, Bell, ZCal-OffsetCal, OffsetCal);
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
GyroProcessor::ProcessStream(bool WriteToFile, std::set<int>& Clients, int64_t Time, uint8_t SeqNum,
                             std::vector<int16_t> const& GyroMeasurements, uint16_t GyroSampleNumber, int Verbose)
{
   // See if the time of this sample is consistent
   uint16_t NumSamplesSinceLast = GyroSampleNumber - LastSampleNum;
   double ApproxElapsed = NumSamplesSinceLast * GyroTimestep;
   int64_t TimeSinceLast = Time-LastSampleTime;

   if (ShowNext > 0 && Verbose > 0)
   {
      std::cerr << "Next packet: " << Time << " bell " << Bell
                << " samples elapsed: " << NumSamplesSinceLast
                << " current timestep: " << GyroTimestep
                << " time since last: " << TimeSinceLast << " expected time: " << ApproxElapsed << '\n';
      --ShowNext;
   }

   // The worst-case scenario in normal use is that a slow accelerometer sample comes just before a packet
   // is ready, which slows down the packet by around 6000 microseconds.  The addtional delay gets
   // spread over the following half-dozen packets coming slightly sooner than expected.
   if (std::abs(TimeSinceLast - ApproxElapsed) > (10000) || TimeSinceLast > 800*GyroTimestep
       || ApproxElapsed > 800*GyroTimestep)
   {
      if (Verbose > 0)
         std::cerr << "Reset stream " << Time << " bell " << Bell
                   << " samples elapsed: " << NumSamplesSinceLast
                   << " current timestep: " << GyroTimestep
                   << " time since last: " << TimeSinceLast << " expected time: " << ApproxElapsed << '\n';
      if (Verbose > 1)
         ShowNext = Verbose-1;
      // reset the stream
      // this doesn't require anything special here, everything else happens on the 'else' part
      GyroSampleWeighting = 20000;
      LastSampleTime = Time;
   }
   else
   {
      // continuing on
      // Update GyroTimestep with a weighted average
      GyroTimestep = (GyroSampleWeighting*GyroTimestep + TimeSinceLast) / (GyroSampleWeighting + NumSamplesSinceLast);
      GyroSampleWeighting = std::min(GyroSampleWeighting+NumSamplesSinceLast, GyroSampleWeightingMax);
      LastSampleTime += GyroTimestep*NumSamplesSinceLast;
   }

   // Divide the packets up into timeslices
   LastSampleNum = GyroSampleNumber;
   for (int i = 0; i < GyroMeasurements.size(); ++i)
   {
      this->ProcessPacket(WriteToFile, Clients, LastSampleTime - int64_t(std::round((GyroMeasurements.size()-i-1) * GyroTimestep)),
                          GyroMeasurements[i]);
   }


}

std::array<int, 16> BellSeqNum {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

int main(int argc, char** argv)
{
   try
   {
      std::string InFile;
      std::string OutFile;
      int Verbose = 0;

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
            int r = read(infd, &len, 1);
            r = read(infd, buf, len);
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
               // TODO: handle better the case where more than one sensor has the same bell.
               Bell = SensorFromUID[uid].Bell;
               if (Bell == -1)
               {
                  // unrecognised UID
                  std::cerr << "Unrecognised sensor UID: 0x" << std::hex << uid << std::dec << " on pipe " << PipeNumber << "\n";
                  continue;
               }
               BellNumber[PipeNumber] = Bell;
               std::cerr << "Associating sensor " << std::hex << uid << " with pipe " << std::dec << PipeNumber
                         << " and bell " << std::dec << Bell << "\n";
               WriteBellAvailMsg(WriteToFile, Clients, Time-Delay, Bell);
            }
            if (Bell == -1)
            {
               // if we still don't have a bell number, then we can't do anything useful
               if (Verbose > 1)
                  std::cerr << "Discarding packet with no associated bell number from pipe " << PipeNumber
                            << " UID 0x" << std::hex << uid << std::dec << '\n';
               continue;
            }
            int16_t AccODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+14+4));
            int16_t GyroODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+16+4));
            int8_t GyroBW = *static_cast<int8_t const*>(static_cast<void const*>(buf+18+4));
            bool Power = (Flags & 0x20) == 0x20;
            bool Charging = (Flags & 0x10) == 0x10;
            bool Sleeping = (Flags & 0x02) == 0x02;
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
               WriteBatteryVMsg(WriteToFile, Clients, Time-Delay, Bell, float(V) / 1000);
            }
            //std::cout << "Status." << std::endl;
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

            int ExpectedPacketLength = NumAccel*4 + NumGyro*2 + 4 + 1 + 8 + 4 + 2;

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
                            << " basic_seq " << (Delay & 0x03)
                            << " packets " << LostPackets
                            << " last seq " << int(BellSeqNum[Bell]) << " next seq " << int(SeqNum) << '\n';
                  std::cerr << "Packet: ";
                  debug_packet(buf, len, std::cerr);
                  std::cerr << "Previous packet on this pipe: ";
                  debug_packet(LastBuf[PipeNumber], LastBufSize[PipeNumber], std::cerr);
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

            uint16_t GyroSampleNumber;
            std::memcpy(&GyroSampleNumber, buf+17, 2);
            std::vector<int16_t> GyroMeasurements(NumGyro);
            std::memcpy(GyroMeasurements.data(), buf+19+NumAccel*4, NumGyro*2);

            // debugging duplicate packets
            memcpy(LastBuf[PipeNumber], buf, len);
            LastBufSize[PipeNumber] = len;

            GyroList[Bell].ProcessStream(WriteToFile, Clients, Time-Delay, SeqNum, GyroMeasurements, GyroSampleNumber, Verbose);

            std::vector<int16_t> AccelMeasurements(NumAccel*2);
            std::memcpy(AccelMeasurements.data(), buf+19, NumAccel*4);
            for (int i = 0; i < NumAccel; ++i)
            {
               int16_t Ax = AccelMeasurements[i*2];
               int16_t Ay = AccelMeasurements[i*2+1];
               WriteAccelMsg(WriteToFile, Clients, Time-Delay, Bell, Ax/AxScale, Ay/AyScale);

               GyroList[Bell].ProcessAccel(WriteToFile, Clients, Time-Delay, Ax, Ay);
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
