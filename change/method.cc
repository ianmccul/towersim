// -*- C++ -*- $Id: method.cc 974 2012-07-04 22:20:50Z uqimccul $

//
// call
//

inline
call::call()
   : Abbrev(' '), Name()
{
}

inline
call::call(char Abbrev_, std::string const& Name_)
   : Abbrev(Abbrev_), Name(Name_)
{
}

inline
bool
call::is_null() const
{
   return Name.empty();
}

inline
call call::bob()
{
   return call('-', "Bob");
}

inline
call call::single()
{
   return call('s', "Single");
}

inline
call call::extreme()
{
   return call('x', "Extreme");
}

inline
bool operator<(call const& c1, call const& c2)
{
   return c1.Name < c2.Name;
}

inline
bool operator==(call const& c1, call const& c2)
{
   return c1.Name == c2.Name;
}

inline
bool operator!=(call const& c1, call const& c2)
{
   return c1.Name != c2.Name;
}

