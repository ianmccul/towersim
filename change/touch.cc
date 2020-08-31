// -*- C++ -*- $Id: touch.cc 974 2012-07-04 22:20:50Z uqimccul $

#include <list>
#include <algorithm>

template<class OutputIterator, class ForwardIterator>
ForwardIterator
interpret_pn(int num, ForwardIterator i, ForwardIterator finish, 
	     OutputIterator out)
{
  ForwardIterator j;
  bool symblock;
  change cross = pn(num, "X");
  while(i != finish && isspace(*i)) ++i; // Skip whitespace
  while(i != finish) {
     std::list<change> block;
    // See whether it's a symmetrical block or not
    symblock = (*i == '&'); 
    // Allow MicroSIRIL style '+' prefix
    if(*i == '&' || *i == '+') { 
      ++i; 
      while(i != finish && isspace(*i)) ++i; // Skip whitespace
      if(i != finish && *i == '.') ++i; // Skip a '.' separator
      while(i != finish && isspace(*i)) ++i; // Skip whitespace
    }
    while(i != finish && (isalnum(*i) || *i == '-')) {
      // Get a change
      if(*i == 'X' || *i == 'x' || *i == '-') {
	++i;
	block.push_back(cross);
      }
      else {
	j = i;
	while(j != finish && isalnum(*j) && *j != 'X' && *j != 'x') ++j;
	if(j != i) block.push_back(pn(num, std::string(i, j)));
	i = j;
      }
      while(i != finish && isspace(*i)) ++i; // Skip whitespace
      if(i != finish && *i == '.') ++i; // Skip a '.' separator
      while(i != finish && isspace(*i)) ++i; // Skip whitespace
    }
    // Now output the block
    std::copy(block.begin(), block.end(), out);
    if(symblock) { 
      block.pop_back(); 
      std::copy(block.rbegin(), block.rend(), out);
    }
    if(i != finish) {
       if (*i != ',')
       {
	  // we've got an unexpected character, return
	  return i;
       }
      ++i; // Skip a ',' separator
      while(i != finish && isspace(*i)) ++i; // Skip whitespace
    }
  }
  return i;
}
