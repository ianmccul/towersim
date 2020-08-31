// -*- C++ -*- $Id: touch.h 974 2012-07-04 22:20:50Z uqimccul $

#if !defined(TOUCH_H_HJKSGJHG5676)
#define TOUCH_H_HJKSGJHG5676

#include "change.h"
#include <vector>
#include <list>

typedef std::vector<change> changelist;

//
// interpret_pn
// Interprets place notation.  '&' character is used to denote a symmetrical block.
// Commas separate distinct sections.  Eg, Stedman slow six can be written "1.3.1.3.1.5" or "&1.3.1,5"
//

// Generic version takes an input sequence which is a list of characters, and writes the output list
// of swaps (as changes).  Parsing stops if we get an unexpected character,
template<class OutputIterator, class ForwardIterator>
ForwardIterator
interpret_pn(int num, ForwardIterator i, ForwardIterator finish, 
	     OutputIterator out);

// This version takes a string of place notation and returns a changelist of
// swaps
changelist interpret_pn(int Size, std::string const& pn);

// Transform cl by acting on it with c from the left
changelist
operator*(change const& c, changelist const& cl);

// Given a list of swaps, accumulates them into a sequence of changes, starting from rounds.
// Rounds is added as the first change in the changelist
changelist
accumulate_from_rounds(changelist const& c);

// Given a list of swaps, and a starting row, accumulates them into a list.
// the starting row isn't included in the result.
changelist
accumulate(changelist const& c, change ch);

// equivalent accumulate(c, change(c[0].size()))
changelist
accumulate(changelist const& c);

// Write a changelist to a stream
std::ostream& operator<<(std::ostream& out, changelist const& c);

#include "touch.cc"

#endif
