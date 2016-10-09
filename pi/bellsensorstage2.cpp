#include <sys/socket.h>
#include <sys/un.h>
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

// for output
struct MsgTypes
{
   static unsigned char const Gyro = 'G';
   static unsigned char const GyroCalibration = 'H';
   static unsigned char const GyroTemp = 'T';
   static unsigned char const BatteryV = 'B';
   static unsigned char const ChargeV = 'C';
};

void WriteMsgToClients(std::set<int>& Clients, unsigned char const* Buf, int Sz)
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

void WriteGyroCalibrationMsg(std::set<int>& Clients, int64_t Time, int Bell, float GyroOffset)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   Buf[8] = MsgTypes::GyroCalibration;
   *static_cast<int8_t*>(static_cast<void*>(Buf+9)) = Bell;
   *static_cast<float*>(static_cast<void*>(Buf+10)) = GyroOffset;
   WriteMsgToClients(Clients, Buf, 8+1+1+sizeof(float));
}

void WriteGyroMsg(std::set<int>& Clients, int64_t Time, int Bell, float z)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   Buf[8] = MsgTypes::Gyro;
   *static_cast<int8_t*>(static_cast<void*>(Buf+9)) = Bell;
   *static_cast<float*>(static_cast<void*>(Buf+10)) = z;
   WriteMsgToClients(Clients, Buf, 8+1+1+sizeof(float));
}

void WriteGyroTempMsg(std::set<int>& Clients, int64_t Time, int Bell, int16_t T)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   Buf[8] = MsgTypes::GyroTemp;
   *static_cast<int8_t*>(static_cast<void*>(Buf+9)) = Bell;
   *static_cast<int16_t*>(static_cast<void*>(Buf+10)) = T;
   WriteMsgToClients(Clients, Buf, 8+1+1+sizeof(int16_t));
}

void WriteBatteryVMsg(std::set<int>& Clients, int64_t Time, int Bell, uint16_t V)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   Buf[8] = MsgTypes::BatteryV;
   *static_cast<int8_t*>(static_cast<void*>(Buf+9)) = Bell;
   *static_cast<int16_t*>(static_cast<void*>(Buf+10)) = V;
   WriteMsgToClients(Clients, Buf, 8+1+1+sizeof(uint16_t));
}

void WriteChargeVMsg(std::set<int>& Clients, int64_t Time, int Bell, uint16_t V)
{
   unsigned char Buf[100];
   *static_cast<int64_t*>(static_cast<void*>(Buf)) = Time;
   Buf[8] = MsgTypes::ChargeV;
   *static_cast<int8_t*>(static_cast<void*>(Buf+9)) = Bell;
   *static_cast<int16_t*>(static_cast<void*>(Buf+10)) = V;
   WriteMsgToClients(Clients, Buf, 8+1+1+sizeof(uint16_t));
}

class GyroProcessor
{
   public:
      GyroProcessor(int Bell_);

      void ProcessStream(std::set<int>& Clients, int64_t Time, uint8_t SeqNum, std::vector<int16_t> const& GyroMeasurements);

   private:
      void ProcessPacket(std::set<int>& Clients, int64_t Time, int16_t z);

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

void GyroProcessor::ProcessPacket(std::set<int>& Clients, int64_t Time, int16_t z)
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
         WriteGyroCalibrationMsg(Clients, Time, Bell, GyroOffset);
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
   WriteGyroMsg(Clients, Time, Bell, ZCal);
}

void
GyroProcessor::ProcessStream(std::set<int>& Clients, int64_t Time, uint8_t SeqNum, std::vector<int16_t> const& GyroMeasurements)
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
         this->ProcessPacket(Clients, Time - int64_t(std::round((GyroMeasurements.size()-i-1) * GyroDelta)),
                             GyroMeasurements[i]);
      }
      return;
   }
   // continuing on

   // distribute the packets between the time slice
   for (int i = 0; i < GyroMeasurements.size(); ++i)
   {
      int64_t t = LastPacketTime + int64_t(std::round((Time - LastPacketTime) * (float(i) / GyroMeasurements.size())));
      this->ProcessPacket(Clients, t, GyroMeasurements[i]);
   }
   LastPacketTime = Time;
   LastSeqNum = SeqNum;
}

int main()
{
   // initialize the GyroList
   std::vector<GyroProcessor> GyroList;
   for (int i = 0; i < 16; ++i)
   {
      GyroList.push_back(GyroProcessor(i));
   }

   std::string InputSocketPath{"\0bellsensordaemonsocketraw", 26};

   unsigned char buf[100];

   int infd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
   if (infd < 0)
   {
      perror("socket error");
      exit(-1);
   }

   struct sockaddr_un inaddr;
   memset(&inaddr, 0, sizeof(struct sockaddr_un));
   inaddr.sun_family = AF_LOCAL;
   std::copy(InputSocketPath.begin(), InputSocketPath.end(), inaddr.sun_path);

   if (connect(infd, (struct sockaddr*)&inaddr,
               InputSocketPath.size() + offsetof(sockaddr_un, sun_path)) == -1)
   {
      perror("connect error");
      exit(-1);
   }


   std::string OutputSocketPath("\0bellsensordaemonsocket", 23);
   std::set<int> Clients;

   int outfd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_NONBLOCK, 0);
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
      exit(-1);
   }

   if (listen(outfd, 5) == -1)
   {
      perror("listen error");
      exit(-1);
   }

   int n = 0;

   while(1)
   {
      // do we have a client waiting to connect?
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

      int rc=read(infd, buf, sizeof(buf));
      if (rc > 0)
      {
         int Size = rc;
         int64_t Time = *static_cast<int64_t const*>(static_cast<void const*>(buf));
         int Bell = buf[8];
         uint16_t Delay = *static_cast<uint16_t const*>(static_cast<void const*>(buf+9));
         unsigned char Flags = buf[11];

         if (Flags & 0x80)
         {
            int16_t AccODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+12));
            int16_t GyroODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+14));
            int8_t GyroBW = *static_cast<int8_t const*>(static_cast<void const*>(buf+16));
            int Offset = 17;
            // status packet
            if (Flags & 0x20)
            {
               // we have a temperature
               int8_t T = *static_cast<int8_t const*>(static_cast<void const*>(buf+Offset));
               ++Offset;
               WriteGyroTempMsg(Clients, Time-Delay, Bell, T);
            }
            if (Flags & 0x10)
            {
               // charging voltage
               uint16_t V = *static_cast<uint16_t const*>(static_cast<void const*>(buf+Offset));
               Offset += 2;
               WriteChargeVMsg(Clients, Time-Delay, Bell, V);
            }
            if (Flags & 0x04)
            {
               // battery charge
               uint16_t V = *static_cast<uint16_t const*>(static_cast<void const*>(buf+Offset));
               Offset += 2;
               WriteBatteryVMsg(Clients, Time-Delay, Bell, V);
            }
         }
         else
         {
            // sensor packet
            uint8_t SeqNum = buf[12];

            int NumAccel = (Flags & 0x70) >> 4;
            int NumGyro = Flags & 0x0F;

            int ExpectedPacketLength = NumAccel*6 + NumGyro*2 + 4 + 1 + 8;

            if (rc != ExpectedPacketLength)
            {
               std::cerr << "Unexpected packet length " << rc << " expected " << ExpectedPacketLength << ", NumAccel=" << NumAccel << ", NumGyro=" << NumGyro 
                         << " Delay=" << Delay << " Flags=" << uint16_t(Flags) << " SeqNum=" << uint16_t(SeqNum) << "\n";
               continue;
            }

            std::vector<int16_t> GyroMeasurements(NumGyro);
            std::memcpy(GyroMeasurements.data(), buf+13+NumAccel*6, NumGyro*2);

            GyroList[Bell].ProcessStream(Clients, Time-Delay, SeqNum, GyroMeasurements);
         }
      }
      else if (rc < 0)
      {
         std::cout << "read failed.\n";
      }
      else if (rc == 0)
      {
         std::cout << "server closed.\n";
         return 0;
      }
   }
   return 0;
}
