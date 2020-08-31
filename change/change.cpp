// -*- C++ -*- $Id: change.cpp 974 2012-07-04 22:20:50Z uqimccul $

#include "change.h"


int num_of_bell_z(char c)
{
   c = toupper(c);
   switch (c)
   {
      case '1' : return 0;
      case '2' : return 1;
      case '3' : return 2;
      case '4' : return 3;
      case '5' : return 4;
      case '6' : return 5;
      case '7' : return 6;
      case '8' : return 7;
      case '9' : return 8;
      case '0' : return 9;
      case 'E' : return 10;
      case 'T' : return 11;
      case 'A' : return 12;
      case 'B' : return 13;
      case 'C' : return 14;
      case 'D' : return 15;
   }
   return -1;
}

extern char const bell_names[] = "1234567890ETABCD";

change pn(int Size, std::string const& px)
{
   std::string::const_iterator p = px.begin();
   std::vector<int> swaps;  // list of pairs to swap
   int b,c;
   if (px.size() == 1 && (*p == 'X' || *p == 'x' || *p == '-'))
   {
      c = 0;
   }
  else
  {
     b = num_of_bell_z(*p);
     if (b & 1) // odd bell position?
        c = 1; // then keep the bell at the front fixed implicitly
     else
        c = 0;
     while (p != px.end())
     {
         b = num_of_bell_z(*p);
         if (b >= c)
         {
            for (int d = c; d < b-1; d += 2)
               swaps.push_back(d);
            c = b+1;
         }
         ++p;
     }
  }
  for(int d = c; d < Size-1; d += 2) 
     swaps.push_back(d);

  change result(Size);
  for (unsigned i = 0; i < swaps.size(); ++i)
  {
     std::swap(result[swaps[i]], result[swaps[i]+1]);
  }
  return result;
}

change
extend(change const& c, int n)
{
   change Result(c.size()+n);
   for (int i = 0; i < c.size(); ++i)
   {
      Result[i] = c[i];
   }
   return Result;
}

std::string
change::to_string() const
{
   std::string c(this->Size, ' ');
   for (int i = 0; i < this->Size; ++i)
      c[i] = bell_name_z(this->Data[i]);
   return c;
}

bool operator<(change const& c1, change const& c2)
{
   return c1.to_string() < c2.to_string();
}

std::ostream& operator<<(std::ostream& out, change const& c)
{
   return out << c.to_string();
}
