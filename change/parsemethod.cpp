// -*- C++ -*- $Id: parsemethod.cpp 1782 2016-09-08 15:24:56Z uqimccul $

#include "method.h"
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

struct MakeChange : boost::static_visitor<change>
{
   MakeChange(int Stage_) : Stage(Stage_) {}

   change operator()(char c) const
   {
      CHECK(c == 'x' || c == 'X' || c == '-');
      return pn(Stage, "x");
   }

   change operator()(std::vector<char> const& c) const
   {
      std::string s(c.begin(), c.end());
      return pn(Stage, s);
   }

   int Stage;
};

change
make_change(int Stage, boost::variant<char, std::vector<char> > const& c)
{
   return boost::apply_visitor(MakeChange(Stage), c);
}

// Mirrors a block, but doesn't set the splice or division flags.
// If SkipLast is true, then the final change is removed.
block
mirror_block(block const& b, bool SkipLast)
{
   changelist Plain = b.swaps();
   if (SkipLast)
      Plain.pop_back();
   std::reverse(Plain.begin(), Plain.end());
   block Result(Plain);
   std::vector<call> Calls = b.calls_available();
   for (std::vector<call>::const_iterator I = Calls.begin(); I != Calls.end(); ++I)
   {
      changelist C = b.swaps(*I);
      if (SkipLast)
	 C.pop_back();
      std::reverse(C.begin(), C.end());
      Result.add_call(*I, C);
   }
   return Result;
}

// mirror a block; that is, append the reflection, without repeating the final change
std::vector<block>
mirror_block_list(std::vector<block> const& b)
{
   std::vector<block> Result(b);
   int i = b.size();
   if (--i < 0)
      return Result; // no blocks

   bool NextSplice = b[i].is_splice_point();
   bool NextDiv = b[i].is_division();

   if (b[i].num_rows() > 1)
   {
      Result.push_back(mirror_block(b[i], true));
   }

   while (--i >= 0)
   {
      block NextBlock = (mirror_block(b[i], false));
      NextBlock.set_splice_point(NextSplice);
      NextBlock.set_division(NextDiv);
      Result.push_back(NextBlock);
   }
   return Result;
}

typedef boost::tuple<boost::optional<std::string>, changelist> compound_block_type;
typedef std::vector<compound_block_type> call_block_type;

block
make_call_block(call_block_type const& c)
{
   if (c.empty())
      return block();

   block Result(c[0].get<1>());
   for (unsigned i = 1; i < c.size(); ++i)
   {
      call ThisCall;
      if (c[i].get<0>())
      {
	 ThisCall.Name = *c[i].get<0>();
	 ThisCall.Abbrev = ThisCall.Name[0];
	 ThisCall.Name = std::string(ThisCall.Name.begin()+1, ThisCall.Name.end());
      }
      else
      {
	 if (i == 1)
	    ThisCall = call::bob();
	 else if (i == 2)
	    ThisCall = call::single();
	 else if (i == 3)
	    ThisCall = call::extreme();
	 else
	    PANIC("Too many unnamed calls");
      }
      if (!c[i].get<1>().empty())
	 Result.add_call(ThisCall, c[i].get<1>());
   }
   return Result;
}

block
make_simple_block(changelist const& c)
{
   return block(c);
}

block
add_splice_tag(block const& b)
{
   block Result(b);
   Result.set_splice_point(true);
   return Result;
}

block
add_div_tag(block const& b)
{
   block Result(b);
   Result.set_division(true);
   return Result;
}

block
add_splice_div_tag(block const& b)
{
   block Result(b);
   Result.set_splice_point(true);
   Result.set_division(true);
   return Result;
}

method
make_method(std::string const& Name,
	    std::string const& FullName,
	    class_type Class,
	    std::string const& Comment,
	    std::vector<block> const& b)
{
   method Result(Name, FullName, Class, Comment);

   // if there is no explicit splice point or division, then add one at the start
   bool HasSplicePoint = false;
   bool HasDivision = false;
   for (unsigned i = 0; i < b.size(); ++i)
   {
      if (b[i].is_splice_point())
	 HasSplicePoint = true;
      if (b[i].is_division())
	 HasDivision = true;
   }

   for (unsigned i = 0; i < b.size(); ++i)
   {
      if (i == 0)
      {
	 block B = b[i];
	 if (!HasSplicePoint)
	    B.set_splice_point(true);
	 if (!HasDivision)
	    B.set_division(true);
	 Result.add_block(B);
      }
      else
      {
	 Result.add_block(b[i]);
      }
   }
   return Result;
}

method
make_method_from_list(std::string const& Name,
		      std::string const& FullName,
		      class_type Class,
		      std::string const& Comment,
		      std::vector<std::vector<block> > const& b)
{
   // collapse the nested array
   std::vector<block> Result;
   for (unsigned i = 0; i < b.size(); ++i)
   {
      for (unsigned j = 0; j < b[i].size(); ++j)
      {
	 Result.push_back(b[i][j]);
      }
   }
   return make_method(Name, FullName, Class, Comment, Result);
}

template <typename Iterator>
struct MethodGrammar : qi::grammar<Iterator,
				   method(std::string const&, std::string const&,
					  class_type, int, std::string const&),
				   ascii::space_type>
{
   MethodGrammar() : MethodGrammar::base_type(method_t)
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
      using boost::spirit::qi::_r1;
      using boost::spirit::qi::_r2;
      using boost::spirit::qi::_r3;
      using boost::spirit::qi::_r4;
      using boost::spirit::qi::_r5;

      bell = char_('0', '9') | char_("etETabcdABCD");

      swap = (char_("xX-") | +bell)
	 [_val = boost::phoenix::bind(&make_change, _r1, _1)];

      swaplist = *(swap(_r1) >> -lit('.'));

      swaplist_not_empty = +(swap(_r1) >> -lit('.'));

      call_name = lexeme['"' >> (char_-'"') >> lit(',') >> +(char_ - '"') >> '"'];

      compound_block = lit('(') >> -call_name >> swaplist(_r1) >> lit(')') >> -lit('.');

      call_block = (+compound_block(_r1))[_val = boost::phoenix::bind(&make_call_block, _1)];

      simple_block = swaplist_not_empty(_r1)
	 [_val = boost::phoenix::bind(&make_simple_block, _1)];

      basic_block = simple_block(_r1) | call_block(_r1);

      splice_block = (lit('*') >> basic_block(_r1))
	 [_val = boost::phoenix::bind(add_splice_tag, _1)];

      div_block = (lit('|') >> basic_block(_r1))
	 [_val = boost::phoenix::bind(add_div_tag, _1)];

      div_splice_block = (((lit('|') >> lit('*')) | (lit('*') >> lit('|'))) >> basic_block(_r1))
	 [_val = boost::phoenix::bind(add_splice_div_tag, _1)];

      block_t = div_splice_block(_r1) | div_block(_r1) | splice_block(_r1) | basic_block(_r1);

      blockarray %= *block_t(_r1);

      symmetricblock = (lit('&') >> blockarray(_r1))
	 [_val =  boost::phoenix::bind(&mirror_block_list, _1)];

      method_t = ((symmetricblock(_r4) | blockarray(_r4)) % lit(','))
	 [_val = boost::phoenix::bind(&make_method_from_list, _r1, _r2, _r3, _r5, _1)];
   }

   qi::rule<Iterator, char(), ascii::space_type> bell;
   qi::rule<Iterator, change(int), ascii::space_type> swap;
   qi::rule<Iterator, changelist(int), ascii::space_type> swaplist;
   qi::rule<Iterator, changelist(int), ascii::space_type> swaplist_not_empty;
   qi::rule<Iterator, std::string(), ascii::space_type> call_name;
   qi::rule<Iterator, boost::tuple<boost::optional<std::string>, changelist>(int),
      ascii::space_type> compound_block;

   qi::rule<Iterator, block(int), ascii::space_type> call_block;
   qi::rule<Iterator, block(int), ascii::space_type> simple_block;
   qi::rule<Iterator, block(int), ascii::space_type> basic_block;
   qi::rule<Iterator, block(int), ascii::space_type> splice_block;
   qi::rule<Iterator, block(int), ascii::space_type> div_block;
   qi::rule<Iterator, block(int), ascii::space_type> div_splice_block;
   qi::rule<Iterator, block(int), ascii::space_type> block_t;

   qi::rule<Iterator, std::vector<block>(int), ascii::space_type> blockarray;
   qi::rule<Iterator, std::vector<block>(int), ascii::space_type> symmetricblock;
   qi::rule<Iterator, method(std::string const&, std::string const&,
			     class_type, int, std::string const&), ascii::space_type> method_t;
};

method
ParseMethod(std::string const& Name, std::string const& FullName,
	    class_type Class, int Stage, std::string const& Comment, std::string const& MyStr)
{
   typedef std::string::const_iterator iterator_type;
   typedef MethodGrammar<iterator_type> MethodGrammarType;
   MethodGrammarType MyMethodGrammar;

   iterator_type iter = MyStr.begin();
   iterator_type end = MyStr.end();

   method m;
   bool r = boost::spirit::qi::phrase_parse(iter, end, MyMethodGrammar(Name, FullName,
								       Class, Stage, Comment),
                                            boost::spirit::ascii::space, m);

   if (r && iter == end)
   {
      return m;
   }

   std::string rest(iter, end);
   PANIC("Parsing failed")(MyStr)(rest);
   return m;
}

template <typename Iterator>
struct FullMethodGrammar : qi::grammar<Iterator,
				       method(),
				       qi::locals<std::string, std::string,
						  class_type, int, std::string>,
				       ascii::space_type>
{
   FullMethodGrammar() : FullMethodGrammar::base_type(start)
   {
      using qi::lexeme;
      using qi::lit;
      using qi::int_;
      using qi::_1;
      using ascii::char_;
      using ascii::no_case;
      using qi::_a;
      using qi::_b;
      using qi::_c;
      using qi::_d;
      using qi::_e;

      class_symbols.add
	 ("principle", Principle)
	 ("plain", Plain)
	 ("treblebob", TrebleBob)
	 ("surprise", Surprise)
	 ("delight", Delight)
	 ("trebleplace", TreblePlace)
	 ("alliance", Alliance)
	 ("hybrid", Hybrid)
	 ("differential", Differential)
	 ("twinhunt", TwinHunt)
	 ;

      quoted_string %= lexeme['"' >> *(char_ - '"') >> '"'];

      class_ %= no_case[class_symbols];

      start %= qi::omit[quoted_string[_a = _1]] > ','
						> qi::omit[quoted_string[_b = _1]] > ','
						> qi::omit[class_[_c = _1]] > ','
						> qi::omit[int_[_d = _1]] > ','
						> qi::omit[quoted_string[_e = _1]] > ','
						> method_parser(_a, _b, _c, _d, _e);
   }

   qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
   qi::rule<Iterator, class_type(), ascii::space_type> class_;
   qi::rule<Iterator, method(),
      qi::locals<std::string, std::string, class_type, int, std::string>,
      ascii::space_type> start;
   MethodGrammar<Iterator> method_parser;

   qi::symbols<char, class_type> class_symbols;

};

method
ParseMethod(std::string const& MyStr)
{
   typedef std::string::const_iterator iterator_type;
   typedef FullMethodGrammar<iterator_type> MethodGrammarType;
   MethodGrammarType MyMethodGrammar;

   iterator_type iter = MyStr.begin();
   iterator_type end = MyStr.end();

   bool r = true;
   method m;
   try
   {
      r = boost::spirit::qi::phrase_parse(iter, end, MyMethodGrammar, boost::spirit::ascii::space, m);
   }
   catch (boost::spirit::qi::expectation_failure<iterator_type> const& c)
   {
      std::string Msg = "Parsing method failed after: " + std::string(MyStr.begin(), c.first)
	 + "remainder of string is: " + std::string(c.first, c.last);
      throw ParseFailure(Msg);
   }

   if (!r)
   {
      std::string Msg = "Parsing method failed after: " + std::string(MyStr.begin(), iter)
	 + " Remainder of string is: " + std::string(iter, end);
      throw ParseFailure(Msg);
   }

   return m;
}
