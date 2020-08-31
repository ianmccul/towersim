// -*- C++ -*- $Id: alphanum.cpp 974 2012-07-04 22:20:50Z uqimccul $

#include "alphanum.h"
#include <cstdlib>

int alphanum_compare(const char *l, const char *r)
{
   enum mode_t { STRING, NUMBER } mode=STRING;
   
   while(*l && *r)
   {
      if(mode == STRING)
      {
         char l_char, r_char;
         while((l_char=*l) && (r_char=*r))
         {
            // check if this are digit characters
            const bool l_digit=std::isdigit(l_char), r_digit=std::isdigit(r_char);
            // if both characters are digits, we continue in NUMBER mode
            if(l_digit && r_digit)
            {
               mode=NUMBER;
               break;
            }
            // if only the left character is a digit, we have a result
            if(l_digit) return -1;
            // if only the right character is a digit, we have a result
            if(r_digit) return +1;
            // compute the difference of both characters
            const int diff=l_char - r_char;
            // if they differ we have a result
            if(diff != 0) return diff;
            // otherwise process the next characters
            ++l;
            ++r;
         }
      }
      else // mode==NUMBER
      {
         // get the left number
         char *end;
         unsigned long l_int=std::strtoul(l, &end, 0);
         l=end;

         // get the right number
         unsigned long r_int=std::strtoul(r, &end, 0);
         r=end;

         // if the difference is not equal to zero, we have a comparison result
         const long diff=l_int-r_int;
         if(diff != 0)
            return diff;

         // otherwise we process the next substring in STRING mode
         mode=STRING;
      }
   }
   
   if(*r) return -1;
   if(*l) return +1;
   return 0;
}

