// -*- C++ -*- $Id: soundclient.cpp 991 2012-07-16 06:58:31Z uqimccul $

#include "towersim/outputparser.h"
#include "change/method.h"

#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include <boost/bind.hpp>

//#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

// argh - we need to include these to get vector<> -> tuple conversions working
#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/include/boost_tuple.hpp>


#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/spirit/home/classic/phoenix.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/asio.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant.hpp>

#include <iostream>
#include <string>
#include <cstdlib>
#include <set>
#include <vector>
#include <deque>
#include <string>

#include <boost/algorithm/string.hpp>

//#include "display_attribute.h"
#include <boost/tuple/tuple.hpp>

#include "common/trace.h"

int const AudioBufferSize = 1024;
int const NumAudioChannels = 65;
int const ReserveChannels = 1;
int const CallChannel = 0;

std::set<OutCom::RingBell> ActiveBells;
boost::asio::deadline_timer* BellTimer;

std::deque<Mix_Chunk*> BellSounds;
std::map<int, std::deque<Mix_Chunk*> > BellSoundsList;

std::deque<Mix_Chunk*> AllBellSounds;

std::map<std::string, int> SensorCount;
std::map<std::string, boost::posix_time::time_duration> SensorTotalError;


std::set<OutCom::Call> ActiveCalls;
boost::asio::deadline_timer* CallTimer;
std::map<std::string, Mix_Chunk*> CallSounds;
std::list<Mix_Chunk*> PendingCalls;

void LoadCallSound(std::string const& Name)
{
   if (CallSounds.find(Name) != CallSounds.end())
      return;

   std::string File = std::string(std::getenv("HOME")) 
	 + "/.towersim/calls/" + Name + ".wav";
   CallSounds[Name] = Mix_LoadWAV(File.c_str());
   if (CallSounds[Name] == NULL)
   {
      std::cerr << "soundclient: warning: cannot load call sound file \"" << File << "\"\n";
   }
}   

void PlaySounds(const boost::system::error_code& e,
                boost::asio::deadline_timer* t)
{
#if 0
   if (e == boost::asio::error::operation_aborted)
   {
      //      TRACE("wait aborted");
      return;
   }
#endif

   boost::posix_time::time_duration Latency = boost::posix_time::microseconds(5000);
   boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
   while (!ActiveBells.empty() && ActiveBells.begin()->Time <= Now+Latency)
   {
      Mix_PlayChannel(-1, BellSounds[ActiveBells.begin()->Bell-1], 0);
      boost::posix_time::ptime Now2 = boost::posix_time::microsec_clock::universal_time();
      //      TRACE("Latency")(Now)(Now2- ActiveBells.begin()->Time);
      ActiveBells.erase(ActiveBells.begin());
   }
   if (!ActiveBells.empty() && e != boost::asio::error::operation_aborted)
   {
      t->expires_at(ActiveBells.begin()->Time);
      t->async_wait(boost::bind(PlaySounds,
                                boost::asio::placeholders::error, t));
   }
}

extern "C"
void ChannelFinished(int Channel)
{
   if (Channel == CallChannel)
   {
      if (!PendingCalls.empty())
      {
	 Mix_PlayChannel(CallChannel, PendingCalls.front(), 0);
	 PendingCalls.pop_front();
      }
   }
}

void PlayCalls(const boost::system::error_code& e,
               boost::asio::deadline_timer* t)
{
   boost::posix_time::time_duration Latency = boost::posix_time::microseconds(5000);
   boost::posix_time::ptime Now = boost::posix_time::microsec_clock::universal_time();
   while (!ActiveCalls.empty() && ActiveCalls.begin()->Time <= Now+Latency)
   {
      SDL_LockAudio();
      for (unsigned i = 0; i < ActiveCalls.begin()->Calls.size(); ++i)
      {
	 // Don't schedule NULL chunks
	 if (CallSounds[ActiveCalls.begin()->Calls[i]] != NULL)
	    PendingCalls.push_back(CallSounds[ActiveCalls.begin()->Calls[i]]);
      }
      if (!Mix_Playing(CallChannel))
      {
	 ChannelFinished(CallChannel);
      }
      SDL_UnlockAudio();
      ActiveCalls.erase(ActiveCalls.begin());
   }
   if (!ActiveCalls.empty() && e != boost::asio::error::operation_aborted)
   {
      t->expires_at(ActiveCalls.begin()->Time);
      t->async_wait(boost::bind(PlayCalls,
                                boost::asio::placeholders::error, t));
   }
}

inline 
boost::posix_time::time_duration 
abs(boost::posix_time::time_duration td) 
{ 
   if (td.is_negative()) 
      return td *= -1; 
   return td; 
} 

void QueueBell(OutCom::RingBell const& B)
{
   if (B.Silent)
   {
      // See if the same row/place is already active, and remove it if it is
      if (B.Place)
      {
         std::set<OutCom::RingBell>::iterator I = ActiveBells.begin();
         while (I != ActiveBells.end())
         {
            if (I->Place && ((I->Place->get<0>() == B.Place->get<0>())
                             && (I->Place->get<1>() == B.Place->get<1>()))
                && !I->Sensor)
            {
               ActiveBells.erase(I);
            }
            else
            {
               ++I;
            }
         }
      }
      return;
   }

   ActiveBells.insert(B);
   if (ActiveBells.size() == 1 || ActiveBells.begin()->Time < BellTimer->expires_at())
   {
      BellTimer->expires_at(ActiveBells.begin()->Time);
      BellTimer->async_wait(boost::bind(PlaySounds,
                                        boost::asio::placeholders::error, BellTimer));
   }

   if (B.Sensor && B.Error)
   {
      ++SensorCount[*B.Sensor];
      SensorTotalError[*B.Sensor] += abs(B.Error->get<0>());
      std::cerr << "Bell " << B.Bell 
                << " sensor " << (*B.Sensor)
                << " row " << (B.Place->get<0>())
                << " place " << (B.Place->get<1>())
                << " error " << B.Error->get<0>().total_milliseconds() << "ms"
                << std::endl;
   }
}

void QueueCall(OutCom::Call const& c)
{
   // ensure that the calls are already loaded
   for (int i = 0; i < c.Calls.size(); ++i)
   {
      LoadCallSound(c.Calls[i]);
   }
   ActiveCalls.insert(c);
   if (ActiveCalls.size() == 1 || ActiveCalls.begin()->Time < CallTimer->expires_at())
   {
      CallTimer->expires_at(ActiveCalls.begin()->Time);
      CallTimer->async_wait(boost::bind(PlayCalls,
                                        boost::asio::placeholders::error, CallTimer));
   }
}

// split a method name into up to 3 parts, <name>, <class>, <stage>
std::vector<std::string>
SplitMethodName(std::string const& Name)
{
   std::vector<std::string> Words;
   boost::algorithm::split(Words, Name, boost::algorithm::is_any_of(" "));
   if (Words.empty())
      return Words;

   // we'll assemble the calls in reverse order so a deque is useful
   std::deque<std::string> Calls;
   // firstly, attempt to split the stage name
   for (int i = 1; i <= 16; ++i)
   {
      if (Words.back() == stage_name(i))
      {
	 Calls.push_front(Words.back());
	 Words.pop_back();
	 break;
      }
   }
   // now attempt to split the class.  This is complicated because some class
   // names are two words.  This is a bit of a hack
   if (Words.size() > 1)
   {
      if (Words[Words.size()-2] == "Treble" && Words[Words.size()-1] == "Bob")
      {
	 Words.pop_back();
	 Words.back() = "Treble Bob";
      }
      else if (Words[Words.size()-2] == "Treble" && Words[Words.size()-1] == "Place")
      {
	 Words.pop_back();
	 Words.back() = "Treble Place";
      }
   }
   for (int c = 0; c <= TwinHunt; ++c)
   {
      if (Words.back() == class_name(class_type(c)))
      {
	 Calls.push_front(Words.back());
	 Words.pop_back();
      }
   }
   // the remaining part is the proper name
   Calls.push_front(boost::algorithm::join(Words, " "));
   return std::vector<std::string>(Calls.begin(), Calls.end());
}

void QueueCall(OutCom::CallGoMethod const& c)
{
   // split the method name as far as we can
   std::vector<std::string> Calls = SplitMethodName(c.Name);
   // ensure that the calls are already loaded
   for (int i = 0; i < Calls.size(); ++i)
   {
      LoadCallSound(Calls[i]);
   }
   OutCom::Call MyCall;
   MyCall.Time = c.Time;
   if (CallSounds[Calls[0]] == NULL)
   {
      // the method name isn't present, use "gonexthandstrke" instead
      MyCall.Calls = std::vector<std::string>(1, "gonexthandstroke");
   }
   else
   {
      // we've got the method name.  Add "go" at the start.
      MyCall.Calls = std::vector<std::string>(1, "go");
      MyCall.Calls.insert(MyCall.Calls.end(), Calls.begin(), Calls.end());
   }
   QueueCall(MyCall);
}

void QueueCall(OutCom::CallSpliceMethod const& c)
{
   // split the method name as far as we can
   std::vector<std::string> Calls = SplitMethodName(c.Name);
   // ensure that the calls are already loaded
   for (int i = 0; i < Calls.size(); ++i)
   {
      LoadCallSound(Calls[i]);
   }
   OutCom::Call MyCall;
   MyCall.Time = c.Time;
   if (CallSounds[Calls[0]] == NULL)
   {
      // the method name isn't present, "splice" instead
      // and hopefully the person on the end of the rope knows what method it is!
      MyCall.Calls = std::vector<std::string>(1, "splice");
   }
   else
   {
      // we've got the method name.  For a splice, just the bare method name
      // is what we want.
      MyCall.Calls = Calls;
   }
   QueueCall(MyCall);
}

void ShowReport()
{
   std::cout << "****REPORT****\n";
   for (std::map<std::string, int>::const_iterator i = SensorCount.begin(); i != SensorCount.end(); ++i)
   {
      std::cout << "Sensor " << i->first << " rang " << i->second << " times, with average error "
                << (SensorTotalError[i->first] / i->second).total_milliseconds()
                << "ms" << std::endl;
   }
   std::cout << "****REPORT FINISHED****\n";
}

void AbortRinging()
{
   ActiveBells.clear();
   BellTimer->cancel();
}  

void SetNumberOfBells(int n)
{
   BellSounds = BellSoundsList[n];
}

class ApplyOutputCommand : public boost::static_visitor<>
{
   public:
      ApplyOutputCommand() {}

      void operator()(OutCom::Finish const& f) const
      {
         ShowReport();
         SensorCount.clear();
         SensorTotalError.clear();
      }

      void operator()(OutCom::NumberOfBells const& f) const
      {
         SetNumberOfBells(f.Number);
      }

      void operator()(OutCom::Division const&) const
      {
      }

      void operator()(OutCom::Abort const& f) const
      {
         AbortRinging();
         ShowReport();
      }

      void operator()(OutCom::RingBell const& b) const
      {
         QueueBell(b);
      }

      void operator()(OutCom::LoadCall const& c) const
      {
         LoadCallSound(c.Call);
      }

      void operator()(OutCom::Call const& c) const
      {
         QueueCall(c);
      }

      void operator()(OutCom::CallGoMethod const& c) const
      {
         QueueCall(c);
      }

      void operator()(OutCom::CallSpliceMethod const& c) const
      {
         QueueCall(c);
      }

      void operator()(OutCom::OutputError const& c) const
      {
         std::cerr << "Parsing error occurred: " << c.What << '\n';
      }

};

class SoundServer
{
   public:
      SoundServer(boost::asio::io_service& io_service)
         : input(io_service, ::dup(STDIN_FILENO)), 
           input_buffer(512) // buffer length 
      {
         // read input
         boost::asio::async_read_until(input, input_buffer, '\n',
                                       boost::bind(&SoundServer::handle_input, this,
                                                   boost::asio::placeholders::error,
                                                   boost::asio::placeholders::bytes_transferred));
      }
    
      void close()
      {
         input.close();
      }

   private:
      void handle_input(const boost::system::error_code& error, std::size_t length)
      {
         if (!error)
         {
            std::vector<char> MyStr(length-1);
            input_buffer.sgetn(&MyStr[0], length-1);
            input_buffer.consume(1); // Remove newline from input.

            std::string Str(MyStr.begin(), MyStr.end());

            OutCom::OutputCommand C = OutCom::ParseOutputCommand(Str);
            boost::apply_visitor(ApplyOutputCommand(), C);
         }
         else if (error == boost::asio::error::not_found)
         {
            // Didn't get a newline. Send whatever we have.
         }
         else
         {
            TRACE("some error, probably EOF");
            // some other error
            exit(0);
         }
         // read the next line
         boost::asio::async_read_until(input, input_buffer, '\n',
                                       boost::bind(&SoundServer::handle_input, this,
                                                   boost::asio::placeholders::error,
                                                   boost::asio::placeholders::bytes_transferred));
      }

      boost::asio::posix::stream_descriptor input;
      boost::asio::streambuf input_buffer;
};



int
main()
{
   Uint32 t;
   int volume=SDL_MIX_MAXVOLUME;

   /* initialize SDL for audio and video */
   if(SDL_Init(SDL_INIT_AUDIO)<0)
   {
      PANIC("cannot initialize SDL");
   }

   int initted=Mix_Init(0);
   
   if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,1,AudioBufferSize)<0)
   {
      PANIC("cannot initialize SDL_Mixer");
   }

   Mix_AllocateChannels(NumAudioChannels);
   Mix_ReserveChannels(ReserveChannels);
   Mix_ChannelFinished(&ChannelFinished);

   // print out some info on the audio device and stream
   int audio_rate,audio_channels;
   Uint16 audio_format;
   Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);

   int bits=audio_format&0xFF;
   printf("Opened audio at %d Hz %d bit %s, %d bytes audio buffer\n", audio_rate,
          bits, audio_channels>1?"stereo":"mono", AudioBufferSize);


   // load the sounds
   for (int i = 0; i < 12; ++i)
   {
      std::string File = std::string(std::getenv("HOME")) 
	 + "/.towersim/bellsounds/bell_" + boost::lexical_cast<std::string>(i+1) + ".wav";
      AllBellSounds.push_back(Mix_LoadWAV(File.c_str()));
      CHECK(AllBellSounds.back() != NULL)(File);
      AllBellSounds.back()->volume = MIX_MAX_VOLUME / 2;
      CHECK(AllBellSounds.back() != NULL)("Cannot load bell sound")(File);
   }

   // load the front 4
   std::deque<Mix_Chunk*> Front4;
   {
      std::string File = std::string(std::getenv("HOME")) + "/.towersim/bellsounds/bell_03.wav";
      Front4.push_back(Mix_LoadWAV(File.c_str()));
      File = std::string(std::getenv("HOME")) + "/.towersim/bellsounds/bell_02.wav";
      Front4.push_back(Mix_LoadWAV(File.c_str()));
      File = std::string(std::getenv("HOME")) + "/.towersim/bellsounds/bell_01.wav";
      Front4.push_back(Mix_LoadWAV(File.c_str()));
      File = std::string(std::getenv("HOME")) + "/.towersim/bellsounds/bell_0.wav";
      Front4.push_back(Mix_LoadWAV(File.c_str()));
   }

   // set up the sound sets

   // singles - back 3 of the front 6
   {
      std::deque<Mix_Chunk*> MySoundSet(3);
      MySoundSet[0] = AllBellSounds[5];
      MySoundSet[1] = AllBellSounds[6];
      MySoundSet[2] = AllBellSounds[7];
      BellSoundsList[3] = MySoundSet;
   }

   // minimus - back 4 of the front 6
   {
      std::deque<Mix_Chunk*> MySoundSet(4);
      MySoundSet[0] = AllBellSounds[4];
      MySoundSet[1] = AllBellSounds[5];
      MySoundSet[2] = AllBellSounds[6];
      MySoundSet[3] = AllBellSounds[7];
      BellSoundsList[4] = MySoundSet;
   }

   // doubles - back 5 of the front 6
   {
      std::deque<Mix_Chunk*> MySoundSet(5);
      MySoundSet[0] = AllBellSounds[3];
      MySoundSet[1] = AllBellSounds[4];
      MySoundSet[2] = AllBellSounds[5];
      MySoundSet[3] = AllBellSounds[6];
      MySoundSet[4] = AllBellSounds[7];
      BellSoundsList[5] = MySoundSet;
   }

   // minor - front 6
   {
      std::deque<Mix_Chunk*> MySoundSet(6);
      MySoundSet[0] = AllBellSounds[2];
      MySoundSet[1] = AllBellSounds[3];
      MySoundSet[2] = AllBellSounds[4];
      MySoundSet[3] = AllBellSounds[5];
      MySoundSet[4] = AllBellSounds[6];
      MySoundSet[5] = AllBellSounds[7];
      BellSoundsList[6] = MySoundSet;
   }

   // triples - back 7
   {
      std::deque<Mix_Chunk*> MySoundSet(7);
      MySoundSet[0] = AllBellSounds[5];
      MySoundSet[1] = AllBellSounds[6];
      MySoundSet[2] = AllBellSounds[7];
      MySoundSet[3] = AllBellSounds[8];
      MySoundSet[4] = AllBellSounds[9];
      MySoundSet[5] = AllBellSounds[10];
      MySoundSet[6] = AllBellSounds[11];
      BellSoundsList[7] = MySoundSet;
   }

   // major - back 8
   {
      std::deque<Mix_Chunk*> MySoundSet(8);
      MySoundSet[0] = AllBellSounds[4];
      MySoundSet[1] = AllBellSounds[5];
      MySoundSet[2] = AllBellSounds[6];
      MySoundSet[3] = AllBellSounds[7];
      MySoundSet[4] = AllBellSounds[8];
      MySoundSet[5] = AllBellSounds[9];
      MySoundSet[6] = AllBellSounds[10];
      MySoundSet[7] = AllBellSounds[11];
      BellSoundsList[8] = MySoundSet;
   }

   // caters - back 9
   {
      std::deque<Mix_Chunk*> MySoundSet(9);
      for (int i = 0; i < 9; ++i)
         MySoundSet[i] = AllBellSounds[i+3];
      BellSoundsList[9] = MySoundSet;
   }

   // royal - back 10
   {
      std::deque<Mix_Chunk*> MySoundSet(10);
      for (int i = 0; i < 10; ++i)
         MySoundSet[i] = AllBellSounds[i+2];
      BellSoundsList[10] = MySoundSet;
   }

   // cinques - back 11
   {
      std::deque<Mix_Chunk*> MySoundSet(11);
      for (int i = 0; i < 11; ++i)
         MySoundSet[i] = AllBellSounds[i+1];
      BellSoundsList[11] = MySoundSet;
   }

   // maximus - back 12
   BellSoundsList[12] = AllBellSounds;

   // thirteen - sixteen - start adding trebles
   BellSoundsList[13] = AllBellSounds;
   BellSoundsList[13].push_front(Front4[3]);

   BellSoundsList[14] = BellSoundsList[13];
   BellSoundsList[14].push_front(Front4[2]);

   BellSoundsList[15] = BellSoundsList[14];
   BellSoundsList[15].push_front(Front4[1]);

   BellSoundsList[16] = BellSoundsList[15];
   BellSoundsList[16].push_front(Front4[0]);

   boost::asio::io_service io;

   boost::asio::deadline_timer Timer(io, boost::posix_time::seconds(1));
   BellTimer = &Timer;
   Timer.async_wait(boost::bind(PlaySounds,
                            boost::asio::placeholders::error, &Timer));

   boost::asio::deadline_timer MyCallTimer(io, boost::posix_time::seconds(1));
   CallTimer = &MyCallTimer;
   MyCallTimer.async_wait(boost::bind(PlayCalls,
                                      boost::asio::placeholders::error, &MyCallTimer));
   
   SoundServer MyServer(io);

   io.run();

   std::cout << "bye...\n";
   Mix_CloseAudio();
   SDL_Quit();


    return 0;
}

