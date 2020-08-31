// -*- C++ -*- $Id: method.h 992 2012-07-16 06:58:38Z uqimccul $

#if !defined(METHOD_H_SDHFHY84574839RPLJFJ)
#define METHOD_H_SDHFHY84574839RPLJFJ

#include "change.h"
#include "touch.h"
#include <vector>
#include <map>
#include <set>
#include <stdexcept>

struct call
{
   char Abbrev;
   std::string Name;

   call();
   call(char Abbrev_, std::string const& Name_);

   bool is_null() const; 

   static call bob();
   static call single();
   static call extreme();
};

bool operator<(call const& c1, call const& c2);
bool operator==(call const& c1, call const& c2);
bool operator!=(call const& c1, call const& c2);

// the standard types of methods
enum class_type 
   { Principle, Plain, TrebleBob, Surprise, Delight, TreblePlace, Alliance, Hybrid, Differential,
   TwinHunt };

// return the friendly name of each class type
std::string class_name(class_type t);

// return the name of each class type, as a single word
std::string class_string(class_type t);

// A 'block' is a section of a method, that consists
// of a changelist of swaps (NOT a list of rows as previous versions used!).
// Each block contains a 'plain' section and any number of calls.
// The number of rows in each call variant need not
// be the same (although for normal methods they are!).
// A block also contains a flag, which if true indicates
// that the first row of the block is a splice point.
class block
{
   public:
      block() {}

      // initializes a block with the given plain lead, assumes the
      // start of this block isn't a splicing point
      explicit block(changelist const& c);

      // initializes a block with the given plain lead, and splice point flag
      block(changelist const& c, bool Splice);

      // initializes a block with the given plain lead, and splice point flag, and division flag
      block(changelist const& c, bool Splice, bool Div);

      // adds a block for the specified call.  By convention,
      // a plain lead corresponds to call 'p'
      void add_call(call const& c, changelist const& cl);

      // Sets the SplicePoint flag
      void set_splice_point(bool Splice = true);

      // Sets the Division flag
      void set_division(bool Div = true);

      // returns the list of swaps for the plain block
      changelist swaps() const;

      // returns the list of swaps for the given call
      changelist swaps(call const& c) const;

      // returns the list of swaps for the given call
      changelist swaps(char c) const;

      bool is_splice_point() const { return SplicePoint; }

      bool is_division() const { return Division; }

      int stage() const { return PlainBlock.begin()->size(); }

      // returns the number of rows in the plain block
      int num_rows() const { return PlainBlock.size(); }

      // returns the number of rows in the given block
      int num_rows(call const& c) const;

      // returns the set of available calls
      std::vector<call> calls_available() const;

      // returns the number of calls available - 0 means no calls (plain block only)
      int num_calls() const { return CallBlock.size(); }

   private:
      bool SplicePoint;
      bool Division;
      changelist PlainBlock;
      std::vector<std::pair<call, changelist> > CallBlock;
};

//
// Place notation for a method:
// Standard place notation, but for calls we can use (a)(b)(c) meaning
// use a for the plain block, b for bob, s for single.  So for example
// Norwich minor is
// &x34x14x12x36x34x16,(16)(14)(1456)
// If we don't specify a splice point then the first row is taken to be a splice point.
// But we can add alternative splice points with a *, as in Stedman doubles,
// 5.1.3.(1)()(145).*3.1|5.3.1.(3)()(345).1.3|
// which makes the starred row also the default start row.
// We can put multiple splice points, the effect of which isn't entirely specified ;-)
// Note that in the stedman example the . before and after the brackets are optional.
// If we don't specify the locations of the block divisions, the default division
// is immediately before the call locations.
class method
{
   public:
      method();

      method(std::string const& Name_);

      method(std::string const& Name_,
	     std::string const& FullName_,
	     class_type Class_,
	     std::string const& Comment_);

      // Construct a method from the place notation.  If the FullName is empty,
      // it is constructed automatically from the name, class and stage.
      // The Notation accepted by this constructor is rather limited - it allows
      // one block using the interpret_pn() function.
      method(std::string const& Name_,
	     class_type Class_,
	     int Stage_,
	     std::string const& Notation_,
	     std::string const& FullName = "");

      // The shortened name, eg "Cambridge"
      std::string const& name() const { return Name; }

      // The name including the class, eg "Cambridge Surprise"
      std::string const& full_name() const { return FullName; }

      // The full name plus the class, for example "Cambridge Surprise Major"
      std::string full_name_stage() const;

      // The full name plus stage and comment (in brackets), for example
      // "Yorkshire Surprise Major (half-lead calls)"
      std::string full_name_stage_comment() const;

      std::string const& comment() const { return Comment; }

      class_type type() const { return Class; }

      void set_name(std::string const& Name_);

      void set_full_name(std::string const& FullName_) { FullName = FullName_; }
   
      void set_type(class_type c);

      void set_comment(std::string const& Comment_) { Comment = Comment_; }

      int stage() const { return Data.empty() ? 0 : Data[0].stage(); }

      // methods don't have to start at the first row of a section,
      // this returns the first row that is a valid splice point
      int default_start_row() const;

      int num_blocks() const { return Data.size(); }

      block const& get_block(int n) const { return Data[n]; }

      block& get_block(int n) { return Data[n]; }

      void add_block(block const& b)
      {
         Data.push_back(b);
      }

      // the number of changes in the plain lead(s) of the method
      int length() const;

      // returns the set of all possible calls in the method
      std::vector<call> all_calls() const;

   private:
      std::string Name;
      std::string FullName;
      class_type Class;
      std::string Comment;
      std::vector<block> Data;

   // The sort comparison is based on the name
   friend bool operator==(method const& m1, method const& m2);
   friend bool operator!=(method const& m1, method const& m2);
   friend bool operator<(method const& m1, method const& m2);
};

//
// method_cursor
// This is the normal way of iterating through a method.
// The current row (from dereferencing the cursor) iterates through the method.
// the next row is obtained from the .next() member function, and the
// ++ operator advances such that the current row becomes the next() row.
// Calls, or splicing, affects the next row (NOT the current row, as in past versions of the cursor).
//
class method_cursor
{
   public:
      method_cursor();

      method_cursor(method const& m);

      void reset(method const& m);

      void splice(method const& m);

      // returns the current change.
      change const& operator*() const { return CurrentChange; }

      change const* operator->() const { return &CurrentChange; }

      // returns the next change.  This is affected by calls, splices etc
      change next() const;

      bool is_rounds() const 
      {
         return CurrentChange.is_rounds();
      }

      int stage() const { return Method.stage(); }

      method const& get_method() const { return Method; }

      // returns true if there is one or more calls available that would
      // affect the next row
      bool is_call_available_now() const;

      // returns true if the current change is a splice point,
      // where a splice would affect the next row
      bool is_splice_point() const;

      // returns true if there is a division line between the current row and the next row
      bool is_division() const;

      // returns the set of calls available in the method
      std::vector<call> calls_available() const;
      std::set<call> calls_available_as_set() const;

      // returns the set of calls available in the current block that
      // would affect the current row.  If we're not at the start of a block
      // then this set is empty.
      std::vector<call> calls_available_now() const;
      std::set<call> calls_available_now_as_set() const;

      // returns which call is currently active, or a null call if no call is active.
      // A call is active if the next() row would be modified by the presence of the call
      call current_call() const;

      // switch to the given call block.
      // Precondition: c is a member of calls_available_now()
      void set_call(call const& c);

      // advances to the next row
      method_cursor& operator++();

   private:
      method Method;
      int CurrentBlock;     // the index into the method list of blocks
      call CurrentCall;     // the currently active call
      int CurrentRow;       // the row number of the current block
      change CurrentChange;
};

// returns the name of the given stage, capitalized first letter
std::string stage_name(int Bells);

// an exception that is thrown if parsing a method fails
struct ParseFailure : public std::runtime_error
{
   ParseFailure(std::string const& msg_) : std::runtime_error(msg_) {}
};

// Parse the method, given the name, class, stage, comment, and place notation
method
ParseMethod(std::string const& Name, std::string const& FullName,
	    class_type Class, int Stage, std::string const& Comment,
	    std::string const& Notation);

// Parse a full method, of the form "Name", "Full Name", Class, Stage, "Comment", Notation
method
ParseMethod(std::string const& Line);

#include "method.cc"

#endif
