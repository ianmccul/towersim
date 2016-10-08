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

int main(int argc, char** argv)
{
   if (argc != 2)
   {
      std::cout << "usage: bellsensorstage1-log <file>\n";
      return 1;
   }

   int out_fd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, 0666);
   if (out_fd == -1)
   {
      std::cerr << "error: " << strerror(errno) << '\n';
      return 2;
   }

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

   while(1)
   {
      int rc=read(fd, buf+1, sizeof(buf)-1);
      if (rc > 0)
      {
         if (rc > 255)
         {
            std::cerr << "unexpected: packet too long, size " << rc << "\n";
            return 3;
         }
         // include the length
         buf[0] = rc;
         if (int k = write(out_fd, buf, rc+1) != rc+1)
         {
            std::cout << "error writing to file: " << strerror(errno) << ", bytes written =" << k << " requested = " << rc << '\n';
            return 2;
         }
      }
      else
      {
         close(out_fd);
         std::cerr << "error reading from socket: " << strerror(errno) << '\n';
         return 2;
      }
   }
   return 0;
}
