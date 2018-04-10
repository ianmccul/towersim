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
#include <boost/program_options.hpp>

namespace prog_opt = boost::program_options;

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
         std::string SocketPath{"\0bellsensordaemonsocketraw", 26};

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

      std::array<bool, 16> HaveLastSeqNum{};
      std::array<uint8_t, 16> LastSeqNum;
      std::array<uint16_t, 16> LastDelay;

      while(1)
      {
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

         int Size = len;
         int64_t Time = *static_cast<int64_t const*>(static_cast<void const*>(buf));
         int Pipe = buf[8];
         // 4-byte checksum comes next
         uint16_t Delay = *static_cast<uint16_t const*>(static_cast<void const*>(buf+13));
         unsigned char Flags = buf[15];

         if (Pipe < 0 || Bell > 15)
         {
            std::cerr << "unexpected pipe number " << Pipe << '\n';
            continue;
         }

         if (Flags & 0x80)
         {
            // status packet
            uint16_t uid = *static_cast<int16_t const*>(static_cast<void const*>(buf+16));
            int16_t AccODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+18));
            int16_t GyroODR = *static_cast<int16_t const*>(static_cast<void const*>(buf+20));
            int8_t GyroBW = *static_cast<int8_t const*>(static_cast<void const*>(buf+22));
            bool Power = Flags & 0x20;
            bool Charging = Flags & 0x10;
            bool Sleeping = Flags & 0x02;
            std::cout << Time << " STATUS UID 0x" << std::hex << uid << " Pipe " << std::dec << Pipe
                      << " power " << Power << " charging " << Charging << " Sleeping " << Sleeping;
            int Offset = 23;
            // status packet
            if (Flags & 0x08)
            {
               // we have a temperature
               int8_t T = *static_cast<int8_t const*>(static_cast<void const*>(buf+Offset));
               ++Offset;
               std::cout << " Temp " << int(T);
            }
            if (Flags & 0x04)
            {
               // battery charge
               uint16_t V = *static_cast<uint16_t const*>(static_cast<void const*>(buf+Offset));
               Offset += 2;
               std::cout << " Battery " << (V * 0.001);
            }
            std::cout << std::endl;
         }
         else
         {
            // sensor packet
            uint8_t SeqNum = buf[16];

            int NumAccel = (Flags & 0x70) >> 4;
            int NumGyro = Flags & 0x0F;

            int ExpectedPacketLength = NumAccel*4 + NumGyro*2 + 4 + 1 + 8 + 4;

            if (Size != ExpectedPacketLength)
            {
               std::cerr << Time << " Pipe " << Pipe << " unexpected packet length " << Size << " expected " << ExpectedPacketLength << ", NumAccel=" << NumAccel << ", NumGyro=" << NumGyro
                         << " Delay=" << Delay << " Flags=" << uint16_t(Flags) << " SeqNum=" << uint16_t(SeqNum) << std::endl;
               continue;
            }

            if (HaveLastSeqNum[Pipe] && SeqNum != uint8_t(LastSeqNum[Pipe]+1))
            {
               std::cout << Time << " Packet loss pipe " << Pipe << " packets " << (int(uint8_t(SeqNum-LastSeqNum[Pipe]))-1) << std::endl;
            }
            LastSeqNum[Pipe] = SeqNum;
            HaveLastSeqNum[Pipe] = true;
            LastDelay[Pipe] = Delay;
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
