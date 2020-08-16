// -*- C++ -*- $Id: outputparser.h 987 2012-07-15 01:58:28Z uqimccul $
//
// Definition and parser for the output commands
//

#if !defined(OUTPUTPARSER_H_SHDCWEIUTYWUINLSD)
#define OUTPUTPARSER_H_SHDCWEIUTYWUINLSD

#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/tuple/tuple.hpp>

namespace OutCom
{

// Command to actually ring a bell (which may be silent).
// Includes optional sensor, and error (fractional and absolute).
struct RingBell
{
   // ErrorType has (error duration, fractional error (1.0 = one place))
   typedef boost::tuple<boost::posix_time::time_duration, double> ErrorType;

   // PlaceType has (row,place) pair
   typedef boost::tuple<int, int> PlaceType;

   int Bell;
   boost::posix_time::ptime Time;
   boost::optional<ErrorType> Error;
   boost::optional<PlaceType> Place;
   boost::optional<std::string> Sensor;
   bool Silent;

   RingBell();

   RingBell(int Bell_, boost::posix_time::ptime Time_);

   RingBell(int Bell_, boost::posix_time::ptime Time_, std::string const& Sensor_);

   RingBell(int Bell_, boost::posix_time::ptime Time_, 
            boost::posix_time::time_duration Error_, double FracError_,
            bool Silent = false);

   RingBell(int Bell_, boost::posix_time::ptime Time_, 
            boost::posix_time::time_duration Error_, double FracError_,
            std::string const& Sensor,
            bool Silent = false);

   RingBell(int Bell_, boost::posix_time::ptime Time_, 
            boost::posix_time::time_duration Error_, double FracError_,
            int Row_, int Place_, bool Silent = false);

   RingBell(int Bell_, boost::posix_time::ptime Time_, 
            boost::posix_time::time_duration Error_, double FracError_,
            int Row_, int Place_,
            std::string const& Sensor_,
            bool Silent = false);
};

inline
bool operator==(RingBell const& t1, RingBell const& t2)
{
   return t1.Bell == t2.Bell && t1.Time == t2.Time && t1.Sensor == t2.Sensor;
}

inline
bool operator<(RingBell const& t1, RingBell const& t2)
{
   return t1.Time < t2.Time || (t1.Time == t2.Time && 
                                (t1.Bell < t2.Bell || 
                                 (t1.Bell == t2.Bell && t1.Sensor < t2.Sensor)));
}

std::ostream& operator<<(std::ostream& out, RingBell const& b);

// Indicates that ringing has stopped.
struct Finish
{
};

std::ostream& operator<<(std::ostream& out, Finish const& f);

// Stop ringing and flush all pending events
struct Abort
{
};

std::ostream& operator<<(std::ostream& out, Abort const& f);

// Set the number of bells
struct NumberOfBells
{
   NumberOfBells() : Number(0) {}
   NumberOfBells(int n) : Number(n) {}

   int Number;
};

std::ostream& operator<<(std::ostream& out, NumberOfBells const& f);

struct Division
{
   Division() : Row(0) {}
   Division(int n) : Row(n) {}

   int Row;
};

std::ostream& operator<<(std::ostream& out, Division const& d);

// A 'call' in this context is a set of strings, which are activated
// at some specific time.
struct Call
{   
   boost::posix_time::ptime Time;
   std::vector<std::string> Calls;

   Call() {}

   Call(boost::posix_time::ptime const& Time_, std::vector<std::string> const& Calls_);
   Call(boost::posix_time::ptime const& Time_, std::string const& Call_);
   Call(boost::posix_time::ptime const& Time_, std::string const& Call1, std::string const& Call2);
   Call(boost::posix_time::ptime const& Time_, std::string const& Call1, std::string const& Call2,
	std::string const& Call3);
   Call(boost::posix_time::ptime const& Time_, std::string const& Call1, std::string const& Call2,
	std::string const& Call3, std::string const& Call4);
};

inline
bool
operator==(Call const& c1, Call const& c2)
{
   return c1.Time == c2.Time && c1.Calls == c2.Calls;
}

inline
bool
operator<(Call const& c1, Call const& c2)
{
   return c1.Time < c2.Time || (c1.Time == c2.Time && c1.Calls < c2.Calls);
}

std::ostream& operator<<(std::ostream& out, Call const& f);

// A 'go' <method> call
struct CallGoMethod
{
   boost::posix_time::ptime Time;
   std::string Name;

   CallGoMethod() {}
   CallGoMethod(boost::posix_time::ptime const& Time_, std::string const& Name_);
};

inline
bool operator==(CallGoMethod const& c1, CallGoMethod const& c2)
{
   return c1.Time == c2.Time && c1.Name == c2.Name;
}

inline
bool operator!=(CallGoMethod const& c1, CallGoMethod const& c2)
{
   return c1.Time != c2.Time || c1.Name != c2.Name;
}

// A 'splice <method> call
struct CallSpliceMethod
{
   boost::posix_time::ptime Time;
   std::string Name;

   CallSpliceMethod() {}
   CallSpliceMethod(boost::posix_time::ptime const& Time_, std::string const& Name_);
};

inline
bool operator==(CallSpliceMethod const& c1, CallSpliceMethod const& c2)
{
   return c1.Time == c2.Time && c1.Name == c2.Name;
}

inline
bool operator!=(CallSpliceMethod const& c1, CallSpliceMethod const& c2)
{
   return c1.Time != c2.Time || c1.Name != c2.Name;
}

// a LoadCall event is an optional signal to the back end that it should
// prepare for a call at some stage in the future.
struct LoadCall
{
   LoadCall() {}
   LoadCall(std::string const& c) : Call(c) {}

   std::string Call;
};

std::ostream& operator<<(std::ostream& out, LoadCall const& f);

// Some error occurred, for example a parse error if we were constructing
// the command from a string.
struct OutputError
{
   OutputError() {}
   OutputError(std::string const& What_) : What(What_) {}
   std::string What;
};

std::ostream& operator<<(std::ostream& out, OutputError const& f);

//
// OutputCommand - a variant that contains any of the valid output commands
//

typedef boost::variant<NumberOfBells, RingBell, 
                       Call, CallGoMethod, CallSpliceMethod, LoadCall, Division, Finish, Abort,
                       OutputError> OutputCommand;

// Write an OutputCommand to a stream
std::ostream& operator<<(std::ostream& out, OutputCommand const& c);

// Parse an OutputCommand from a string
OutputCommand
ParseOutputCommand(std::string const& Str);

} // namespace OutCom

#endif
