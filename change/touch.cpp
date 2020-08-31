// -*- C++ -*- $Id: touch.cpp 974 2012-07-04 22:20:50Z uqimccul $

#include "touch.h"
#include <stdexcept>

changelist interpret_pn(int Size, std::string const& pn)
{
   changelist result;
   std::string::const_iterator I = interpret_pn(Size, pn.begin(), pn.end(), 
						std::back_inserter(result));
   if (I != pn.end())
      throw std::runtime_error("Invalid place notation: " + pn);
   return result;
}

changelist
operator*(change const& c, changelist const& cl)
{
   changelist result(cl.size());
   for (unsigned i = 0; i < result.size(); ++i)
   {
      result[i] = c * cl[i];
   }
   return result;
}

changelist
accumulate_from_rounds(changelist const& c)
{
   changelist result;
   result.push_back(change(c[0].size()));
   for (unsigned i = 0; i < c.size(); ++i)
   {
      result.push_back(result.back() * c[i]);
   }
   return result;
}

changelist
accumulate(changelist const& c, change const& ch)
{
   changelist result;
   if (c.size() > 0)
      result.push_back(ch * c[0]);
   for (unsigned i = 1; i < c.size(); ++i)
   {
      result.push_back(result.back() * c[i]);
   }
   return result;
}

changelist
accumulate(changelist const& c)
{
   changelist result;
   if (c.size() > 0)
      result.push_back(c[0]);
   for (unsigned i = 1; i < c.size(); ++i)
   {
      result.push_back(result.back() * c[i]);
   }
   return result;
}

std::ostream& operator<<(std::ostream& out, changelist const& c)
{
   for (unsigned i = 0; i < c.size(); ++i)
   {
      out << c[i].to_string() << '\n';
   }
   return out;
}
