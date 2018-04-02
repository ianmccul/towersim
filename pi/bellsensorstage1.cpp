#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <RF24/RF24.h>

#include <cmath>
#include <cstdio>
#include <time.h>

#include <cstdint>
#include <algorithm>
#include <set>

#include <sys/socket.h>
#include <sys/un.h>
#include <cstddef>
#include <array>
#include <fstream>

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

void sleep_ns(long nanosec)
{
   struct timespec spec;
   spec.tv_sec = 0;
   spec.tv_nsec = nanosec;
   nanosleep(&spec, &spec);
}

void sleep_ms(long microsec)
{
   sleep_ns(microsec*1000);
}

void swap_endian(uint32_t& x)
{
   unsigned char* xx = static_cast<unsigned char*>(static_cast<void*>(&x));
   std::swap(xx[0], xx[3]);
   std::swap(xx[1], xx[2]);
}

namespace detail
{

constexpr uint32_t Prime = 16777619;
constexpr uint32_t Offset = 2166136261;

constexpr
uint32_t hash_fnv32(uint32_t Value, uint8_t const* Beg, uint8_t const* End)
{
   return (Beg == End) ? Value : hash_fnv32((Value ^ uint32_t(*Beg)) * Prime, Beg+1, End);
}

} // namespace detail

constexpr
uint32_t hash_fnv32(uint8_t const* Beg, uint8_t const* End)
{
   return detail::hash_fnv32(detail::Offset, Beg, End);
}

class Radio
{
   public:
      Radio(int ce_pin, int DevMajor, int DevMinor, int Channel, uint64_t PAddr);

      Radio(Radio&) = delete;
      Radio& operator=(Radio const&) = delete;
      Radio(Radio&& r) = default;
      Radio& operator=(Radio&& r) = default;

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
      bool ok;
};

Radio::Radio(int ce_pin, int DevMajor, int DevMinor, int Channel, uint64_t PAddr)
   : radio(ce_pin, DevMajor*10+DevMinor), ok(true)
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
   radio.openReadingPipe(0, (PAddr & 0xffffffff00) | 0xe7);
   radio.openReadingPipe(1, (PAddr & 0xffffffff00) | 0x0f);
   uint8_t PipeAddr = 0x17;
   radio.openReadingPipe(2, &PipeAddr);
   PipeAddr = 0x2c;
   radio.openReadingPipe(3, &PipeAddr);
   radio.enableDynamicPayloads();  // need to do this AFTER opening the pipes!
   std::cout << "Starting radio spidev" << DevMajor << '.' << DevMinor << " ce " << ce_pin << " on channel " << Channel << std::endl;
   radio.printDetails();
   // make sure that all is OK by reading the channel back
   uint8_t Ch = radio.getChannel();
   ok = (Ch == Channel);
   if (!ok)
   {
      std::cout << "Failed to configure radio spidev" << DevMajor << '.' << DevMinor << "!\n";
   }
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
   return ok && radio.available(Pipe);
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

void debug_packet(unsigned char const* buf, int len, std::ostream& out)
{
   std::ios::fmtflags f(out.flags());
   if (len > 32)
   {
      out << "packet too long (" << len << "), truncating ";
      len = 32;
   }
   for (int i = 0; i < len; ++i)
   {
      out << std::hex << "0x" << int(buf[i]) << ' ';
   }
   out.flags(f);
   out << std::endl;
}

int main(int argc, char** argv)
{
   bool TestMode = false;
   uint16_t TestSeq = 0;

   std::array<unsigned char[32], 16> LastBuf;
   std::array<int, 16> LastBufSize{};

   std::ofstream Log("stage1log.txt", std::ofstream::out | std::ofstream::ate);

   int Verbose = 0;
   if (argc >= 2)
   {
      int n = 1;
      while (n < argc && argv[n] == std::string("-v"))
      {
         ++Verbose;
         std::cerr << "Enabling verbose mode " << Verbose << "\n";
         ++n;
      }
      if (n < argc)
      {
         std::cerr << "usage: bellsensorstage1 [-v]\n";
         return 1;
      }
   }

   std::vector<Radio> Radios;
   Radios.reserve(3);

   //
   // configure the radios
   //

   Radios.push_back(Radio(4 , 0, 1, 76, 0xe7e7e7e7e7ULL));
   Radios.push_back(Radio(22, 0, 0, 82, 0x0f0f0f0fe7ULL));
   Radios.push_back(Radio(18, 1, 1, 70, 0x7e7e7ef0e7ULL));

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

   // turn on the radios in verbose mode
   if (Verbose > 0)
   {
      for (auto& r : Radios)
      {
         r.PowerUp();
      }
   }

   // forever loop
   while (1)
   {
      // do we have a client waiting to connect?
      int cl = accept(fd, NULL, NULL);
      if (cl >= 0)
      {
         std::cout << "Accepted connection " << cl << std::endl;
         Log << "Accepted connection " << cl << std::endl;
         if (Verbose == 0 && Clients.empty())
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

      if (Verbose == 0 && Clients.empty())
         continue;

      for (unsigned i = 0; i < Radios.size(); ++i)
      {
         Radio& r = Radios[i];
         // check for data on the radio
         uint8_t PipeNum = 0;
         if (r.Available(&PipeNum))
         {
            if (PipeNum > 3)  // PipeNum of 7 also indicates RX_FIFO is empty.
            {
               Log << "Pipe not available on radio " << i << " time " << get_time() << std::endl;
               std::cerr << "Pipe not available on radio " << i << std::endl;
               continue;
            }
            unsigned char buf[33+9] = { 0 };
            int len = 0;
            len = r.PayloadSize();
            if (Verbose > 3)
            {
               std::cout << "Got a packet, length = " << len << '\n';
            }
            // we don't need to detect the case len > 32, the RF24 lib does that for us and
            // flushes the buffer.  But if that lappens, then we get a length of 0.
            if (len == 0)
            {
               Log << "Discarded invalid packet on pipe " << PipeNum << " at time " << get_time() << std::endl;
               if (Verbose > 0)
                  std::cerr << "Discarded invalid packet on pipe " << PipeNum << " at time " << get_time() << std::endl;
            }
            if (len >= 1)
            {
               int BellNum = (i*4) + PipeNum;

               r.Read(buf+9, len);

               // checksum
               uint32_t Checksum = *static_cast<uint32_t const*>(static_cast<void const*>(buf));
               swap_endian(Checksum);
               if (hash_fnv32(buf+9+4, buf+9+32) != Checksum)
               {
                  std::cout << "FNV hash failed for packet on pipe " << BellNum << ' ';
                  debug_packet(buf+9, len, std::cout);
                  continue;
               }

               // deduplication
               if (LastBufSize[BellNum] == len && (uint8_t(LastBuf[BellNum][0]&0x03) == uint8_t(buf[9]&0x03)))
               {
                  // possible duplicate packet, check the payload
                  if (memcmp(LastBuf[BellNum]+2, buf+11, len-2) == 0)
                  {
                     if (Verbose > 0)
                        std::cout << "Ignoring duplicate packet for pipe " << BellNum
                                  << " seq " << int(uint8_t(buf[9]&0x03)) << std::endl;
                     continue;
                  }
               }

               // save the packet
               memcpy(LastBuf[BellNum], buf+9, len);
               LastBufSize[BellNum] = len;

               // no point removing the sequence number, it only affects us by 0-3 microseconds

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
      }

      if (Verbose == 0 && Clients.empty())
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
