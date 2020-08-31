// -*- C++ -*- $Id: change.cc 974 2012-07-04 22:20:50Z uqimccul $

#include <cstring>
#include <algorithm>
#include "common/trace.h"

inline
int num_of_bell(char c)
{
   return num_of_bell(c) + 1;
}

inline
char bell_name_z(int p)
{
   return bell_names[p];
}

inline
char bell_name(int p)
{
   return bell_names[p-1];
}

inline
change::change() : Size(0), Data(NULL) {}

inline
change::change(int Stage) : Size(Stage), Data(new int[Size])
{
   for (int s = 0; s < Size; ++s)
      Data[s] = s;
}

inline
change::change(change const& Other)
   : Size(Other.Size), Data(new int[Size])
{
   std::copy(Other.Data, Other.Data+Size, Data);
}

inline
bool
change::is_rounds() const
{
   for (int s = 0; s < Size; ++s)
   {
      if (Data[s] != s)
         return false;
   }
   return true;
}

inline
change::change(char const* Str)
{
   Size = std::strlen(Str);
   Data = new int[Size];
   for (int s = 0; s < Size; ++s)
   {
      Data[s] = num_of_bell_z(Str[s]);
   }
}

inline
change::change(std::string const& Str)
{
   Size = Str.size();
   Data = new int[Size];
   for (int s = 0; s < Size; ++s)
   {
      Data[s] = num_of_bell_z(Str[s]);
   }
}

inline
change::~change()
{
   delete[] Data;
}

inline
change&
change::operator=(change const& Other)
{
   Size = Other.size();
   int* NewData = new int[Size];
   std::copy(Other.Data, Other.Data+Size, NewData);
   delete[] Data;
   Data = NewData;
   return *this;
}

inline
change operator*(change const& c1, change const& c2)
{
   return change(c1, c2, change::mult_tag());
}

inline
change operator!(change const& c)
{
   return change(c, change::invert_tag());
}

inline
bool operator==(change const& c1, change const& c2)
{
   return c1.Size == c2.Size && std::equal(c1.Data, c1.Data+c1.Size, c2.Data);
}

inline
bool operator!=(change const& c1, change const& c2)
{
   return c1.Size != c2.Size || !std::equal(c1.Data, c1.Data+c1.Size, c2.Data);
}

inline
change::change(change const& c1, change const& c2, change::mult_tag)
{
   Size = std::max(c1.size(), c2.size());
   Data = new int[Size];

   CHECK_EQUAL(c1.size(), c2.size())("not yet supported");

   // c1.size() == c2.size()
   for (int s = 0; s < Size; ++s)
   {
      Data[s] = c1.Data[c2.Data[s]];
   }
}

inline
change::change(change const& c, change::invert_tag)
{
   Size = c.size();
   Data = new int[Size];
   for (int s = 0; s < Size; ++s)
   {
      Data[c.Data[s]] = s;
   }
}

inline
int
change::place_of_z(int Bell) const
{
   int Result = 0;
   while (Result < Size && Data[Result] != Bell)
      ++Result;
   if (Result == Size)
      Result = -1;
   return Result;
}
