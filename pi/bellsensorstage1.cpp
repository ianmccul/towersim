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

using namespace std;

//
// Hardware configuration
// Configure the appropriate pins for your connections

/****************** Raspberry Pi ***********************/

// Radio CE Pin, CSN Pin, SPI Speed
// See http://www.airspayce.com/mikem/bcm2835/group__constants.html#ga63c029bd6500167152db4e57736d0939 and the related enumerations for pin information.

// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ);



// NEW: Setup for RPi B+
//RF24 radio(RPI_BPLUS_GPIO_J8_15,RPI_BPLUS_GPIO_J8_24, BCM2835_SPI_SPEED_8MHZ);



// Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

// RPi generic:
//RF24 radio(22,0);

/*** RPi Alternate ***/
//Note: Specify SPI BUS 0 or 1 instead of CS pin number.
// See http://tmrh20.github.io/RF24/RPi.html for more information on usage

//RPi Alternate, with MRAA
//RF24 radio(15,0);

//RPi Alternate, with SPIDEV - Note: Edit RF24/arch/BBB/spi.cpp and  set 'this->device = "/dev/spidev0.0";;' or as listed in /dev
RF24 radio(22,0);


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

   // Setup and configure rf radio
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
   radio.setChannel(76);
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
   radio.printDetails();
   radio.powerDown();

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
            // first client, start up the radio.
            std::cout << "Got the first client, powering up radio." << std::endl;
            radio.powerUp();
            radio.startListening();
         }
         Clients.insert(cl);
      }

      // check for data on the radio
      uint8_t PipeNum = 0;
      while (!Clients.empty() && radio.available(&PipeNum) || (TestMode && (rand() % 50 == 0)))
      {
         char buf[33+8];
         int len = 0;
         if (TestMode)
         {
            std::cerr << "Writing test packet.  Number of clients: " << Clients.size() << "\n";
            *static_cast<int64_t*>(static_cast<void*>(buf)) = get_time();
            buf[8] = 0x42;
            buf[9] = SeqNum++;
            *static_cast<int16_t*>(static_cast<void*>(buf+10)) = ++TestSeq;
            *static_cast<int16_t*>(static_cast<void*>(buf+12)) = ++TestSeq;
            *static_cast<int16_t*>(static_cast<void*>(buf+15)) = ++TestSeq;
            len = 1+1+2+2+2;
         }
         else
         {
            len = radio.getDynamicPayloadSize();
//	    std::cout << len << '\n';
            if (len < 1)
               continue;
            radio.read(buf+9, len);
            *static_cast<int64_t*>(static_cast<void*>(buf)) = get_time();
            buf[8] = PipeNum;  // bell number
         }

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
               if (Clients.empty())
               {
                  std::cout << "Last client has disconnected, powering down radio." << std::endl;
                  // power off the radio
                  radio.stopListening();
                  radio.powerDown();
               }
            }
            else
            {
               ++I;
            }
         }
      }
   } // forever loop

   return 0;
}
