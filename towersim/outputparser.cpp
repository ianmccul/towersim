// -*- C++ -*- $Id: outputparser.cpp 1782 2016-09-08 15:24:56Z uqimccul $

#include "outputparser.h"
#include "common/trace.h"
#include <boost/bind.hpp>
#include <boost/variant.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

// argh - we need to include these to get vector<> -> tuple conversions working
#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/include/boost_tuple.hpp>


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

BOOST_FUSION_ADAPT_STRUCT(OutCom::RingBell,
                          (int, Bell)
                          (boost::posix_time::ptime, Time)
                          (boost::optional<OutCom::RingBell::ErrorType>, Error)
                          (boost::optional<OutCom::RingBell::PlaceType>, Place)
                          (boost::optional<std::string>, Sensor)
                          (bool, Silent)
                          )

BOOST_FUSION_ADAPT_STRUCT(OutCom::NumberOfBells, (int, Number))

BOOST_FUSION_ADAPT_STRUCT(OutCom::Division, (int, Row))

BOOST_FUSION_ADAPT_STRUCT(OutCom::Call,
                          (boost::posix_time::ptime, Time)
                          (std::vector<std::string>, Calls)
                          )

BOOST_FUSION_ADAPT_STRUCT(OutCom::CallGoMethod,
                          (boost::posix_time::ptime, Time)
                          (std::string, Name)
                          )

BOOST_FUSION_ADAPT_STRUCT(OutCom::CallSpliceMethod,
                          (boost::posix_time::ptime, Time)
                          (std::string, Name)
                          )

BOOST_FUSION_ADAPT_STRUCT(OutCom::LoadCall,
                          (std::string, Call)
                          )

namespace OutCom
{

template <typename Iterator>
struct OutputGrammar : qi::grammar<Iterator, OutputCommand(), ascii::space_type>
{
   OutputGrammar() : OutputGrammar::base_type(start)
   {
      using qi::eps;
      using qi::lit;
      using qi::int_;
      using qi::lexeme;
      using ascii::char_;
      using qi::as_string;
      using qi::_val;
      using qi::double_;
      using boost::phoenix::ref;
      using boost::spirit::qi::_1;

      quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];

      datetime = as_string[lexeme[+char_("-:T.0-9")]]
         [_val = boost::phoenix::bind(&boost::posix_time::from_iso_string,_1)];

      timeduration = as_string[lexeme[+char_("-:T.0-9")]]
         [_val = boost::phoenix::bind(&boost::posix_time::duration_from_string,_1)];

      numberofbells %= lit("BELLS") >> int_;

      finish %= lit("FINISH")[_val = Finish()];

      abort %= lit("ABORT")[_val = Abort()];

      place %= lit("ROW") >> int_ >> lit("PLACE") >> int_ >> qi::eps;

      percent %= double_[_val = _1 * 0.01];

      error %= -lit('+') >> timeduration >> eps >> percent >> lit('%') >> qi::eps;

      silent %= lit("SILENT")[_val = true] | qi::eps[_val = false];

      ringbell %= (lit("BELL")
                   >> int_
                   >> datetime
                   >> -error
                   >> -place
                   >> -(lit("SENSOR") >> quoted_string)
                   >> silent
                   >> eps);

      call_method %= lit("CALL") >> datetime >> lit("GO") >> quoted_string;

      call_splice %= lit("CALL") >> datetime >> lit("SPLICE") >> quoted_string;

      call %= lit("CALL") >> datetime >> (*quoted_string);

      division %= lit("DIVISION") >> int_;

      loadcall %= lit("LOADCALL") >> quoted_string;

      start %= numberofbells | ringbell | call_method | call_splice | call | loadcall | division | finish | abort;
   }

   qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
   qi::rule<Iterator, boost::posix_time::ptime(), ascii::space_type> datetime;
   qi::rule<Iterator, boost::posix_time::time_duration(), ascii::space_type> timeduration;

   qi::rule<Iterator, RingBell::ErrorType(), ascii::space_type> error;
   qi::rule<Iterator, boost::tuple<int,int>(), ascii::space_type> place;
   qi::rule<Iterator, bool(), ascii::space_type> silent;
   qi::rule<Iterator, double(), ascii::space_type> percent;

   qi::rule<Iterator, NumberOfBells(), ascii::space_type> numberofbells;
   qi::rule<Iterator, Finish(), ascii::space_type> finish;
   qi::rule<Iterator, Abort(), ascii::space_type> abort;
   qi::rule<Iterator, RingBell(), ascii::space_type> ringbell;
   qi::rule<Iterator, Call(), ascii::space_type> call;
   qi::rule<Iterator, CallGoMethod(), ascii::space_type> call_method;
   qi::rule<Iterator, CallSpliceMethod(), ascii::space_type> call_splice;
   qi::rule<Iterator, Division(), ascii::space_type> division;
   qi::rule<Iterator, LoadCall(), ascii::space_type> loadcall;

   qi::rule<Iterator, OutputCommand(), ascii::space_type> start;
};

OutputCommand
ParseOutputCommand(std::string const& MyStr)
{
   typedef std::string::const_iterator iterator_type;
   typedef OutputGrammar<iterator_type> OutputGrammarType;
   OutputGrammarType MyOutputGrammar;

   iterator_type iter = MyStr.begin();
   iterator_type end = MyStr.end();

   OutputCommand Command;
   bool r = boost::spirit::qi::phrase_parse(iter, end, MyOutputGrammar,
                                            boost::spirit::ascii::space, Command);

   if (r && iter == end)
   {
      return Command;
   }
   std::string rest(iter, end);
   return OutputError("Parsing failed, stopped at \"" + rest + "\"");
}

//
// RingBell
//

RingBell::RingBell()
   : Bell(0), Silent(false)
{
}

RingBell::RingBell(int Bell_, boost::posix_time::ptime Time_)
   : Bell(Bell_), Time(Time_), Silent(false)
{
}

RingBell::RingBell(int Bell_, boost::posix_time::ptime Time_, std::string const& Sensor_)
   : Bell(Bell_), Time(Time_), Sensor(Sensor_), Silent(false)
{
}

RingBell::RingBell(int Bell_, boost::posix_time::ptime Time_,
                   boost::posix_time::time_duration Error_, double FracError_,
                   bool Silent_)
   : Bell(Bell_), Time(Time_), Error(ErrorType(Error_, FracError_)), Silent(Silent_)
{
}

RingBell::RingBell(int Bell_, boost::posix_time::ptime Time_,
                   boost::posix_time::time_duration Error_, double FracError_,
                   std::string const& Sensor_,
                   bool Silent_)
   : Bell(Bell_), Time(Time_), Error(ErrorType(Error_, FracError_)),
     Sensor(Sensor_), Silent(Silent_)
{
}

RingBell::RingBell(int Bell_, boost::posix_time::ptime Time_,
                   boost::posix_time::time_duration Error_, double FracError_,
                   int Row_, int Place_, bool Silent_)
   : Bell(Bell_), Time(Time_), Error(ErrorType(Error_, FracError_)),
     Place(PlaceType(Row_, Place_)), Silent(Silent_)
{
}

RingBell::RingBell(int Bell_, boost::posix_time::ptime Time_,
            boost::posix_time::time_duration Error_, double FracError_,
            int Row_, int Place_,
            std::string const& Sensor_,
            bool Silent_)
   : Bell(Bell_), Time(Time_), Error(ErrorType(Error_, FracError_)),
   Place(PlaceType(Row_, Place_)),
   Sensor(Sensor_),
   Silent(Silent_)
{
}

std::ostream&
operator<<(std::ostream& out, RingBell const& t)
{
   out << "BELL " << t.Bell
       << ' '
       << to_iso_string(t.Time);
   if (t.Error)
   {
      out << (t.Error->get<0>().is_negative() ? " " : " +")
          << (t.Error->get<0>()) << ' ' << std::setprecision(2) << (t.Error->get<1>() * 100.0) << '%';
   }
   if (t.Place)
   {
      out << " ROW " << t.Place->get<0>() << " PLACE " << t.Place->get<1>();
   }
   if (t.Sensor)
   {
      out << " SENSOR \""
          << (*t.Sensor)
          << '\"';
   }
   if (t.Silent)
   {
      out << " SILENT";
   }
   out << std::endl;
   return out;
}

Call::Call(boost::posix_time::ptime const& Time_, std::vector<std::string> const& Calls_)
   : Time(Time_), Calls(Calls_)
{
}

Call::Call(boost::posix_time::ptime const& Time_, std::string const& Call_)
   : Time(Time_), Calls(1, Call_)
{
}

Call::Call(boost::posix_time::ptime const& Time_,
	   std::string const& Call1,
	   std::string const& Call2)
   : Time(Time_)
{
   Calls.push_back(Call1);
   Calls.push_back(Call2);
}

Call::Call(boost::posix_time::ptime const& Time_,
	   std::string const& Call1,
	   std::string const& Call2,
	   std::string const& Call3)
   : Time(Time_)
{
   Calls.push_back(Call1);
   Calls.push_back(Call2);
   Calls.push_back(Call3);
}

Call::Call(boost::posix_time::ptime const& Time_,
	   std::string const& Call1,
	   std::string const& Call2,
	   std::string const& Call3,
	   std::string const& Call4)
   : Time(Time_)
{
   Calls.push_back(Call1);
   Calls.push_back(Call2);
   Calls.push_back(Call3);
   Calls.push_back(Call4);
}

CallGoMethod::CallGoMethod(boost::posix_time::ptime const& Time_, std::string const& Name_)
   : Time(Time_), Name(Name_)
{
}

CallSpliceMethod::CallSpliceMethod(boost::posix_time::ptime const& Time_, std::string const& Name_)
   : Time(Time_), Name(Name_)
{
}

std::ostream& operator<<(std::ostream& out, Division const& d)
{
   out << "DIVISION " << d.Row << std::endl;
   return out;
}

std::ostream& operator<<(std::ostream& out, Finish const&)
{
   out << "FINISH" << std::endl;
   return out;
}

std::ostream& operator<<(std::ostream& out, Abort const&)
{
   out << "ABORT" << std::endl;
   return out;
}

std::ostream& operator<<(std::ostream& out, NumberOfBells const& f)
{
   out << "BELLS " << f.Number << std::endl;
   return out;
}

std::ostream& operator<<(std::ostream& out, Call const& f)
{
   out << "CALL " << to_iso_string(f.Time);
   for (unsigned i = 0; i < f.Calls.size(); ++i)
   {
      out << " \"" << f.Calls[i] << '\"';
   }
   out << std::endl;
   return out;
}

std::ostream& operator<<(std::ostream& out, CallGoMethod const& f)
{
   out << "CALL " << to_iso_string(f.Time) << " GO \"" << f.Name << '\"' << std::endl;
   return out;
}

std::ostream& operator<<(std::ostream& out, CallSpliceMethod const& f)
{
   out << "CALL " << to_iso_string(f.Time) << " SPLICE \"" << f.Name << '\"' << std::endl;
   return out;
}

std::ostream& operator<<(std::ostream& out, LoadCall const& f)
{
   out << "LOADCALL \"" << f.Call << '\"' << std::endl;
   return out;
}

std::ostream& operator<<(std::ostream& out, OutputError const& f)
{
   out << "ERROR \"" << f.What << '\"' << std::endl;
   return out;
}

struct WriteToStream : public boost::static_visitor<>
{
   WriteToStream(std::ostream& out_) : out(out_)
   {
   }

   template <typename T>
   void operator()(T const& x) const
   {
      out << x;
   }

   std::ostream& out;
};

std::ostream& operator<<(std::ostream& out, OutputCommand const& c)
{
   boost::apply_visitor(WriteToStream(out), c);
   return out;
}

} // namespace OutCom
