// -*- C++ -*- $Id: method.cpp 992 2012-07-16 06:58:38Z uqimccul $

#include "method.h"

std::string class_name(class_type t)
{
   switch (t)
   {
      case Principle    : return "Principle";
      case Plain        : return "Plain";
      case TrebleBob    : return "Treble Bob";
      case Surprise     : return "Surprise";
      case Delight      : return "Delight";
      case TreblePlace  : return "Treble Place";
      case Alliance     : return "Alliance";
      case Hybrid       : return "Hybrid";
      case Differential : return "Differential";
      case TwinHunt     : return "Twin Hunt";
   }
   return "Unknown";
}

std::string class_string(class_type t)
{
   switch (t)
   {
      case Principle    : return "principle";
      case Plain        : return "plain";
      case TrebleBob    : return "treblebob";
      case Surprise     : return "surprise";
      case Delight      : return "delight";
      case TreblePlace  : return "trebleplace";
      case Alliance     : return "alliance";
      case Hybrid       : return "hybrid";
      case Differential : return "differential";
      case TwinHunt     : return "twinhunt";
   }
   return "unknown";
}

//
// block
//

block::block(changelist const& c)
   : SplicePoint(false), Division(false), PlainBlock(c)
{
}

block::block(changelist const& c, bool Splice)
   : SplicePoint(Splice), Division(false), PlainBlock(c)
{
}

block::block(changelist const& c, bool Splice, bool Div)
   : SplicePoint(Splice), Division(Div), PlainBlock(c)
{
}

void
block::add_call(call const& c, changelist const& cl)
{
   CallBlock.push_back(std::make_pair(c, cl));
}

      
void 
block::set_splice_point(bool Splice)
{
   SplicePoint = Splice;
}

void 
block::set_division(bool Div)
{
   Division = Div;
}

changelist
block::swaps() const
{
   return PlainBlock;
}

changelist
block::swaps(call const& c) const
{
   if (c.is_null())
      return this->swaps();
   std::vector<std::pair<call, changelist> >::const_iterator I = CallBlock.begin();
   while (I != CallBlock.end() && I->first != c)
      ++I;
   if (I == CallBlock.end())
      return changelist();
   return I->second;
}

changelist
block::swaps(char c) const
{
   if (c == ' ')
      return this->swaps();
   std::vector<std::pair<call, changelist> >::const_iterator I = CallBlock.begin();
   while (I != CallBlock.end() && I->first.Abbrev != c)
      ++I;
   if (I == CallBlock.end())
      return changelist();
   return I->second;
}

int
block::num_rows(call const& c) const
{
   if (c.is_null())
      return PlainBlock.size();
   std::vector<std::pair<call, changelist> >::const_iterator I = CallBlock.begin();
   while (I != CallBlock.end() && I->first != c)
      ++I;
   CHECK(I != CallBlock.end())("Call not found")(c.Name);
   return I->second.size();
}

std::vector<call>
block::calls_available() const
{
   std::vector<call> Result;
   for (std::vector<std::pair<call, changelist> >::const_iterator I = CallBlock.begin(); 
	I != CallBlock.end(); ++I)
   {
      Result.push_back(I->first);
   }
   return Result;
 }

//
// method
//

method::method()
{
}

method::method(std::string const& Name_)
   : Name(Name_)
{
}

method::method(std::string const& Name_,
	       std::string const& FullName_,
	       class_type Class_,
	       std::string const& Comment_)
   : Name(Name_), FullName(FullName_), Class(Class_), Comment(Comment_)
{
}

method::method(std::string const& Name_,
	       class_type Class_,
	       int Stage_,
	       std::string const& Notation_,
	       std::string const& FullName_)
   : Name(Name_), 
     FullName(FullName_),
     Class(Class_),
     Data(1, block(interpret_pn(Stage_, Notation_)))
{
   if (FullName.empty())
   {
      FullName = Name + ' ' + stage_name(Stage_) + ' ' + class_name(Class);
   }
}

std::string
method::full_name_stage() const
{ 
   return FullName + ' ' + stage_name(this->stage()); 
}

std::string
method::full_name_stage_comment() const
{ 
   std::string Result = FullName + ' ' + stage_name(this->stage()); 
   if (!Comment.empty())
   {
      Result += " (" + Comment + ')';
   }
   return Result;
}

void
method::set_name(std::string const& Name_)
{
   Name = Name_;
}

void
method::set_type(class_type c)
{
   Class = c;
}

int
method::default_start_row() const
{
   unsigned b = 0;
   int Row = 0;
   while (b < Data.size() && !Data[b].is_splice_point())
   {
      Row += Data[b].num_rows();
      ++b;
   }
   // If we didn't find a splice point then default to the first row
   return (b == Data.size()) ? 0 : Row;
}

std::vector<call>
method::all_calls() const
{
   std::vector<call> Result;
   for (unsigned i = 0; i < Data.size(); ++i)
   {
      std::vector<call> Next = Data[i].calls_available();
      for (unsigned j = 0; j < Next.size(); ++j)
      {
	 if (std::find(Result.begin(), Result.end(), Next[j]) == Result.end())
	    Result.push_back(Next[j]);
      }
   }
   return Result;
}

int
method::length() const
{
   int Result = 0;
   for (unsigned b = 0; b < Data.size(); ++b)
   {
      Result += Data[b].num_rows();
   }
   return Result;
}

bool operator==(method const& m1, method const& m2)
{
   return m1.FullName == m2.FullName && m1.Comment == m2.Comment && m1.stage() == m2.stage();
}

bool operator!=(method const& m1, method const& m2)
{
   return !(m1 == m2);
}

bool operator<(method const& m1, method const& m2)
{
   return (m1.stage() < m2.stage()) 
      || (m1.stage() == m2.stage() 
          && (m1.FullName < m2.FullName 
	      || (m1.FullName == m2.FullName && m1.Comment < m2.Comment)));
}

//
// method_cursor
//

method_cursor::method_cursor()
   : CurrentBlock(0),
     CurrentRow(0)
{
}

method_cursor::method_cursor(method const& m)
   : Method(m),
     CurrentBlock(0),
     CurrentRow(0),
     CurrentChange(change(m.stage()))
{
   // advance to the first splice point
   if (m.default_start_row() != 0)
   {
      while (!m.get_block(CurrentBlock).is_splice_point())
	 ++CurrentBlock;
   }
}

bool
method_cursor::is_splice_point() const
{
   return CurrentRow == 0 && Method.get_block(CurrentBlock).is_splice_point();
}

bool
method_cursor::is_division() const
{
   return CurrentRow == 0 && Method.get_block(CurrentBlock).is_division();
}

void
method_cursor::splice(method const& m)
{
   CHECK(this->is_splice_point());


   // If the new method has a splice point at the current location, then switch to it
   if (m.num_blocks() == Method.num_blocks() 
       && m.length() == Method.length()
       && m.get_block(CurrentBlock).is_splice_point())
   {
      Method = m;
      // keep the current block and row
   }
   else
   {
      // switch to the method and advance to the first splice point
      Method = m;
      CurrentBlock = 0;
      CurrentCall = call();
      CurrentRow = 0;

      // advance to the first splice point
      if (m.default_start_row() != 0)
      {
	 while (!m.get_block(CurrentBlock).is_splice_point())
	    ++CurrentBlock;
      }
   }

   // It would be unusual to have a splice cancelling out a call, but
   // it could happen (eg, with Erin spliced into something else at backstroke).
   CurrentCall = call();
}

void
method_cursor::reset(method const& m)
{
   Method = m;
   CurrentChange = change(Method.stage());
   CurrentRow = 0;
   CurrentBlock = 0;
   CurrentCall = call();

   // advance to the first splice point
   if (m.default_start_row() != 0)
   {
      while (!m.get_block(CurrentBlock).is_splice_point())
	 ++CurrentBlock;
   }
}

change
method_cursor::next() const
{
   return CurrentChange * Method.get_block(CurrentBlock).swaps(CurrentCall)[CurrentRow];
}

bool
method_cursor::is_call_available_now() const
{
   // we can't have a call available if we're already in a call
   if (!CurrentCall.is_null())
      return false;

   // To have a call available we need to be at the first row of a block
   if (CurrentRow != 0)
      return false;

   return Method.get_block(CurrentBlock).num_calls() > 0;
}

std::vector<call>
method_cursor::calls_available_now() const
{
   // we can't have a call available if we're already in a call
   if (!CurrentCall.is_null())
      return std::vector<call>();

   // To have a call available we need to be at the first row of a block
   if (CurrentRow != 0)
      return std::vector<call>();

   return Method.get_block(CurrentBlock).calls_available();
}

std::set<call>
method_cursor::calls_available_now_as_set() const
{
   // we can't have a call available if we're already in a call
   if (!CurrentCall.is_null())
      return std::set<call>();

   // To have a call available we need to be at the first row of a block
   if (CurrentRow != 0)
      return std::set<call>();

   std::vector<call> R = Method.get_block(CurrentBlock).calls_available();
   return std::set<call>(R.begin(), R.end());
}

std::vector<call>
method_cursor::calls_available() const
{
   std::vector<call> Result;
   for (int b = 0; b < Method.num_blocks(); ++b)
   {
      std::vector<call> Next = Method.get_block(b).calls_available();
      for (unsigned j = 0; j < Next.size(); ++j)
      {
	 if (std::find(Result.begin(), Result.end(), Next[j]) == Result.end())
	    Result.push_back(Next[j]);
      }
   }
   return Result;
}

std::set<call>
method_cursor::calls_available_as_set() const
{
   std::set<call> Result;
   for (int b = 0; b < Method.num_blocks(); ++b)
   {
      std::vector<call> Next = Method.get_block(b).calls_available();
      for (unsigned j = 0; j < Next.size(); ++j)
      {
	 Result.insert(Next[j]);
      }
   }
   return Result;
}

call
method_cursor::current_call() const
{
   return CurrentCall;
}

void
method_cursor::set_call(call const& c)
{
   CHECK(this->calls_available_as_set().count(c) > 0);
   CurrentCall = c;
}

method_cursor&
method_cursor::operator++()
{
   CurrentChange = this->next();
   if (++CurrentRow == Method.get_block(CurrentBlock).num_rows(CurrentCall))
   {
      CurrentRow = 0;
      CurrentCall = call();
      if (++CurrentBlock == Method.num_blocks())
      {
         CurrentBlock = 0;
      }
   }
   return *this;
}

std::string
stage_name(int Bells)
{
   switch (Bells)
   {
      case 3 : return "Singles";
      case 4 : return "Minimus";
      case 5 : return "Doubles";
      case 6 : return "Minor";
      case 7 : return "Triples";
      case 8 : return "Major";
      case 9 : return "Caters";
      case 10 : return "Royal";
      case 11 : return "Cinques";
      case 12 : return "Maximus";
      case 13 : return "Sextuples";
      case 14 : return "Fourteen";
      case 15 : return "Septuples";
      case 16 : return "Sixteen";
   }
   return boost::lexical_cast<std::string>(Bells);
}
