#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <set>
#include <cstddef>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <array>

int main(int argc, char** argv)
{
   std::string SocketPath{"\0bellsensordaemonsocketraw", 26};

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

   std::array<bool, 16> HaveLastSeqNum{};
   std::array<uint8_t, 16> LastSeqNum;
   std::array<uint16_t, 16> LastDelay;

   while(1)
   {
      int rc=read(fd, buf, sizeof(buf));

      if (rc > 0)
      {
         int Size = rc;
         int64_t Time = *static_cast<int64_t const*>(static_cast<void const*>(buf));
         int Bell = buf[8];
         uint16_t Delay = *static_cast<uint16_t const*>(static_cast<void const*>(buf+9));
         unsigned char Flags = buf[11];

         if (Bell < 0 || Bell > 15)
         {
            std::cerr << "unexpected bell number " << Bell << '\n';
            continue;
         }

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
               std::cout << "Gyro " << Bell << " temperature " << T << '\n';
            }
            if (Flags & 0x10)
            {
               // charging voltage
               uint16_t V = *static_cast<uint16_t const*>(static_cast<void const*>(buf+Offset));
               Offset += 2;
               std::cout << "Gyro " << Bell << " charging " << V << '\n';
            }
            if (Flags & 0x04)
            {
               // battery charge
               uint16_t V = *static_cast<uint16_t const*>(static_cast<void const*>(buf+Offset));
               Offset += 2;
               std::cout << "Gyro " << Bell << " battery " << V << '\n';
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
               std::cerr << "Bell " << Bell << " unexpected packet length " << rc << " expected " << ExpectedPacketLength << ", NumAccel=" << NumAccel << ", NumGyro=" << NumGyro
                         << " Delay=" << Delay << " Flags=" << uint16_t(Flags) << " SeqNum=" << uint16_t(SeqNum) << "\n";
               continue;
            }

            if (HaveLastSeqNum[Bell] && SeqNum != uint8_t(LastSeqNum[Bell]+1))
            {
               std::cout << "Packet loss " << (int(uint8_t(SeqNum-LastSeqNum[Bell]))-1) << " on bell " << Bell << '\n';
               std::cout << "Delay=" << Delay << " old delay=" << LastDelay[Bell] << '\n';
            }
            LastSeqNum[Bell] = SeqNum;
            HaveLastSeqNum[Bell] = true;
            LastDelay[Bell] = Delay;
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
