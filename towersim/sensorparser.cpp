// -*- C++ -*- $Id: sensorparser.cpp 1782 2016-09-08 15:24:56Z uqimccul $

#include "sensorparser.h"
#include "common/trace.h"
#include <boost/bind.hpp>
#include <boost/variant.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/tuple/tuple.hpp>

// argh - we need to include these to get vector<> -> tuple conversions working
#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/include/boost_tuple.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

BOOST_FUSION_ADAPT_STRUCT(TriggerSensor,
                          (std::string, Sensor)
                          (boost::posix_time::ptime, At)
                          )

BOOST_FUSION_ADAPT_STRUCT(AttachSensor,
                          (std::string, Sensor)
                          (int, HandAdjust)
                          (int, BackAdjust)
                          )

template <typename Iterator>
struct SensorGrammar : qi::grammar<Iterator, SensorInputCommand(), ascii::space_type>
{
   SensorGrammar() : SensorGrammar::base_type(start)
   {
      using qi::eps;
      using qi::lit;
      using qi::int_;
      using qi::lexeme;
      using ascii::char_;
      using qi::as_string;
      using qi::_val;
      using qi::attr;
      using boost::phoenix::ref;
      using boost::spirit::qi::_1;

      quoted_string = lexeme['"' >> +(char_ - '"') >> '"'];

      datetime = as_string[lexeme[+char_("-:T.0-9")]]
         [_val = boost::phoenix::bind(&boost::posix_time::from_iso_string,_1)];

      attach %= lit("ATTACH") >> quoted_string >>
	 ((lit("ADJUST") >> lit("HAND") >> int_ >> lit("BACK") >> int_) | (eps >> attr(0) >> attr(0)));

      detach = lit("DETACH") >> quoted_string;

      sensor = lit("SENSOR") >> (quoted_string >> datetime);

      start = attach | sensor;
   }

      qi::rule<Iterator, SensorInputCommand(), ascii::space_type> start;
      qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
      qi::rule<Iterator, AttachSensor(), ascii::space_type> attach;
      qi::rule<Iterator, DetachSensor(), ascii::space_type> detach;
      qi::rule<Iterator, TriggerSensor(), ascii::space_type> sensor;
      qi::rule<Iterator, boost::posix_time::ptime(), ascii::space_type> datetime;
};

SensorInputCommand
ParseSensorCommand(std::string const& MyStr)
{
   typedef std::string::const_iterator iterator_type;
   typedef SensorGrammar<iterator_type> SensorGrammarType;
   SensorGrammarType MySensorGrammar;

   iterator_type iter = MyStr.begin();
   iterator_type end = MyStr.end();

   SensorInputCommand Command;
   bool r = boost::spirit::qi::phrase_parse(iter, end, MySensorGrammar,
                                            boost::spirit::ascii::space, Command);

   if (r && iter == end)
   {
      return Command;
   }
   std::string rest(iter, end);
   return CommandError("Parsing failed, stopped at \"" + rest + "\"");
}
