#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <set>
#include <cstddef>

struct MsgTypes
{
   static unsigned char const Gyro = 'G';
   static unsigned char const GyroCalibration = 'H';
   static unsigned char const GyroTemp = 'T';
   static unsigned char const BatteryV = 'B';
   static unsigned char const ChargeV = 'C';
};

int main()
{
   std::string SocketPath{"\0bellsensordaemonsocket", 23};

   unsigned char buf[100];

   int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
   if (fd < 0)
   {
      perror("socket error");
      exit(-1);
   }

   struct sockaddr_un addr;
   memset(&addr, 0, sizeof(struct sockaddr_un));
   addr.sun_family = AF_LOCAL;
   std::copy(SocketPath.begin(), SocketPath.end(), addr.sun_path);

   if (connect(fd, (struct sockaddr*)&addr,
               SocketPath.size() + offsetof(sockaddr_un, sun_path)) == -1)
   {
      perror("connect error");
      exit(-1);
   }

   int n = 0;

   while(1)
   {
      int rc=read(fd, buf, sizeof(buf));
      if (rc > 0)
      {
         int64_t Time = *static_cast<int64_t const*>(static_cast<void const*>(buf));
         unsigned char MsgType = buf[8];
         if (MsgType == MsgTypes::Gyro)
         {
            int Bell = *static_cast<int8_t const*>(static_cast<void const*>(buf+9));
            float z = *static_cast<float const*>(static_cast<void const*>(buf+10));
            std::cout << Time << " G " << Bell << ' ' << z << std::endl;
         }
         else if (MsgType == MsgTypes::GyroCalibration)
         {
            int Bell = *static_cast<int8_t const*>(static_cast<void const*>(buf+9));
            float Offset = *static_cast<float const*>(static_cast<void const*>(buf+10));
            std::cout << Time << " H " << Bell << ' ' << Offset << std::endl;
         }
         else if (MsgType == MsgTypes::GyroTemp)
         {
            int Bell = *static_cast<int8_t const*>(static_cast<void const*>(buf+9));
            int8_t T = *static_cast<int8_t const*>(static_cast<void const*>(buf+10));
            std::cout << Time << " T " << Bell << ' ' << int(T) << std::endl;
         }
         else if (MsgType == MsgTypes::BatteryV)
         {
            int Bell = *static_cast<int8_t const*>(static_cast<void const*>(buf+9));
            uint16_t V = *static_cast<uint16_t const*>(static_cast<void const*>(buf+10));
            std::cout << Time << " B " << Bell << ' ' << V << std::endl;
         }
         else if (MsgType == MsgTypes::ChargeV)
         {
            int Bell = *static_cast<int8_t const*>(static_cast<void const*>(buf+9));
            uint16_t V = *static_cast<uint16_t const*>(static_cast<void const*>(buf+10));
            std::cout << Time << " C " << Bell << ' ' << V << std::endl;
         }
         else
         {
            std::cerr << "Unknown packet " << int(MsgType) << " at " << Time << '\n';
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
