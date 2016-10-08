#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>

#include <math.h>
#include <stdio.h>
#include <time.h>

#include <cstdint>
#include <algorithm>
#include <set>

#include <sys/socket.h>
#include <sys/un.h>
#include <cstddef>

//returns the system time in microseconds since epoch
int64_t get_time()
{
   int64_t         ms; // Milliseconds
   time_t          s;  // Seconds
   struct timespec spec;

   clock_gettime(CLOCK_REALTIME, &spec);

   s  = spec.tv_sec;
   ms = spec.tv_nsec / 1000; // Convert nanoseconds to microseconds
   return ms + int64_t(s)*1000000;
}

class Radio
{
   public:
      Radio(int ce_pin, int DevMajor, int DevMinor, int Channel);

      Radio(Radio&) = delete;
      Radio& operator=(Radio const&) = delete;
      Radio(Radio&& r) : radio(std::move(r.radio)) {}
      Radio& operator=(Radio&& r) { radio = std::move(r.radio); }

      // power up the radio and start listening for packets
      void PowerUp();

      // stop listening for packets and power down the radio
      void PowerDown();

      // returns true if there is a packet available.  If so,
      // sets the pipe number.
      bool Available(uint8_t* Pipe);

      // returns the payload size
      int PayloadSize();

      // reads a packet into the buffer
      void Read(unsigned char* buf, int len);

   private:
      RF24 radio;
};

Radio::Radio(int ce_pin, int DevMajor, int DevMinor, int Channel)
   : radio(ce_pin, DevMajor*10+DevMinor)
{
   radio.begin();

   // optionally, increase the delay between retries & # of retries
   radio.setRetries(15,15);
   // Dump the configuration of the rf unit for debugging

   // very important to set the payload size before opening the pipe
   // even though we use dynamic payloads.  NOTE: this seems to be
   // a MAXIMUM permitted payload size if we have dynamic payloads.
   radio.setPayloadSize(32);
   radio.setAddressWidth(5);
   radio.enableDynamicPayloads();  // need to do this AFTER opening the pipes!
   radio.setDataRate(RF24_2MBPS);
   radio.setChannel(Channel);
   radio.setCRCLength(RF24_CRC_16);
   radio.setPALevel(RF24_PA_HIGH);
   radio.setAutoAck(true);  // probably not needed
   radio.closeReadingPipe(0);
   radio.closeReadingPipe(1);
   radio.closeReadingPipe(2);
   radio.closeReadingPipe(3);
   radio.closeReadingPipe(4);
   radio.closeReadingPipe(5);
   radio.openReadingPipe(0, 0xe7e7e7e7e7);
   radio.openReadingPipe(1, 0xf0f0f0f0f0);
   uint8_t PipeAddr = 0x17;
   radio.openReadingPipe(2, &PipeAddr);
   PipeAddr = 0x2c;
   radio.openReadingPipe(3, &PipeAddr);
   radio.enableDynamicPayloads();  // need to do this AFTER opening the pipes!
   std::cout << "Starting radio spidev" << DevMajor << '.' << DevMinor << " ce " << ce_pin << " on channel " << Channel << std::endl;
   radio.printDetails();
   radio.powerDown();
}

inline
void
Radio::PowerUp()
{
   radio.powerUp();
   radio.startListening();
}

inline
void
Radio::PowerDown()
{
   radio.stopListening();
   radio.powerDown();
}

inline
bool
Radio::Available(uint8_t* Pipe)
{
   return radio.available(Pipe);
}

inline
int
Radio::PayloadSize()
{
   return radio.getDynamicPayloadSize();
}

inline
void
Radio::Read(unsigned char* buf, int len)
{
   radio.read(buf, len);
}

void sleep_us(int t)
{
   struct timespec req;
   req.tv_sec = t/1000000;
   req.tv_nsec = (t % 100000) *1000L;

   struct timespec rem;
   nanosleep(&req, &rem);
}

int main(int argc, char** argv)
{
   bool TestMode = false;
   uint16_t TestSeq = 0;
   unsigned char SeqNum = 0;
   if (argc >= 2)
   {
      if (argc == 2 && argv[1] == std::string("--test"))
      {
         TestMode = true;
         std::cerr << "Enabling test mode.\n";
      }
      else
      {
         std::cerr << "usage: bellsensorstage1 [--test]\n";
         return 1;
      }
   }

   std::vector<Radio> Radios;

   Radios.push_back(Radio(22, 0, 0, 76));
   Radios.push_back(Radio(27, 0, 1, 78));


   std::string SocketPath("\0bellsensordaemonsocketraw", 26);
   std::set<int> Clients;

   int fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_NONBLOCK, 0);
   if (fd < 0)
   {
      perror("socket error");
      exit(-1);
   }

   struct sockaddr_un addr;
   memset(&addr, 0, sizeof(struct sockaddr_un));
   addr.sun_family = AF_LOCAL;

   std::copy(SocketPath.begin(), SocketPath.end(), addr.sun_path);

   if (bind(fd, (struct sockaddr*)&addr,
            SocketPath.size() + offsetof(sockaddr_un, sun_path)) == -1)
   {
      perror("bind error");
      exit(-1);
   }

   if (listen(fd, 5) == -1)
   {
      perror("listen error");
      exit(-1);
   }

   // forever loop
   while (1)
   {
      // do we have a client waiting to connect?
      int cl = accept(fd, NULL, NULL);
      if (cl >= 0)
      {
         std::cout << "Accepted connection " << cl << std::endl;
         if (Clients.empty())
         {
            // first client, start up the radios
            std::cout << "Got the first client, powering up radios." << std::endl;
            for (auto& r : Radios)
            {
               r.PowerUp();
            }
         }
         Clients.insert(cl);
      }

      if (Clients.empty())
         continue;

      for (unsigned i = 0; i < Radios.size(); ++i)
      {
         Radio& r = Radios[i];
         // check for data on the radio
         uint8_t PipeNum = 0;
         while (r.Available(&PipeNum))
         {
            unsigned char buf[33+9];
            int len = 0;
            len = r.PayloadSize();
//	    std::cout << len << '\n';
            if (len < 1)
               continue;
            r.Read(buf+9, len);
            *static_cast<int64_t*>(static_cast<void*>(buf)) = get_time();
            buf[8] = (i*4) + PipeNum;  // bell number

            std::set<int>::iterator I = Clients.begin();
            while (I != Clients.end())
            {
               int c = *I;
               int rc = send(c, buf, 9+len, MSG_NOSIGNAL);
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

      if (Clients.empty())
      {
         std::cout << "Last client has disconnected, powering down radios." << std::endl;
         for (auto& r : Radios)
         {
            r.PowerDown();
         }
      }
   } // forever loop

   return 0;
}
