// -*- C++ -*- $Id: alphanum.h 974 2012-07-04 22:20:50Z uqimccul $
//
// alphanumeric compare, adapted from http://www.davekoelle.com/alphanum.html

#if !defined(ALPHANUM_H_SDHJCSDKHFKJLS)
#define ALPHANUM_H_SDHJCSDKHFKJLS

#include <functional>
#include <string>

/**
   compare l and r with strcmp() semantics, but using
   the "Alphanum Algorithm". This function is designed to read
   through the l and r strings only one time, for
   maximum performance. It does not allocate memory for
   substrings. It can either use the C-library functions isdigit()
   and atoi() to honour your locale settings, when recognizing
   digit characters when you "#define ALPHANUM_LOCALE=1" or use
   it's own digit character handling which only works with ASCII
   digit characters, but provides better performance.

   @param l NULL-terminated C-style string
   @param r NULL-terminated C-style string
   @return negative if l<r, 0 if l equals r, positive if l>r
*/
int alphanum_compare(const char *l, const char *r);

inline
int 
alphanum_compare(std::string const& l, std::string const& r)
{
   return alphanum_compare(l.c_str(), r.c_str());
}

inline
bool 
alphanum_less(char const* l, char const* r)
{
   return alphanum_compare(l, r) < 0;
}

inline
bool 
alphanum_less(std::string const& l, std::string const& r)
{
   return alphanum_compare(l.c_str(), r.c_str()) < 0;
}

struct AlphanumLess : public std::binary_function<std::string, std::string, bool>
{
   bool operator()(std::string const& l, std::string const& r) const
   {
      return alphanum_compare(l.c_str(), r.c_str()) < 0;
   }
};

#endif
