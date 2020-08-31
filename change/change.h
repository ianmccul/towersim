// -*- C++ -*- $Id: change.h 1658 2015-08-09 13:26:37Z uqimccul $

#if !defined(CHANGE_H_SAHJKASH349872389UP9)
#define CHANGE_H_SAHJKASH349872389UP9

#include <string>

// Returns the zero-based integer number of a bell, given a character 1234567890ETABCD
int num_of_bell_z(char c);

// Returns the zero-based integer number of a bell, given a character 1234567890ETABCD
int num_of_bell(char c);

extern char const bell_names[];

char bell_name_z(int p);

char bell_name(int p);

class change
{
   public:
      change();
      change(int Stage);
      change(char const* Str);
      change(std::string const& Str);
      change(change const& Other);

      ~change();

      int size() const { return Size; }

      change& operator=(change const& Other);

      // zero-based numbering
      int& operator[](int c) { return Data[c]; }
      int const& operator[](int c) const { return Data[c]; }

      // This is the regular usage, with 1-based numbering
      int operator()(int c) const { return Data[c-1]+1; }

      int place_of_z(int Bell) const;  // returns the place number of the given bell, zero based

      int place_of(int Bell) const { return this->place_of_z(Bell-1)+1; }

      bool is_rounds() const;

      std::string to_string() const;

   private:
      int Size;
      int* Data;

      struct mult_tag {};
      change(change const& c1, change const& c2, mult_tag);

      struct invert_tag {};
      change(change const& c, invert_tag);

   friend change operator*(change const& c1, change const& c2);
   friend change operator!(change const& c);
   friend bool operator==(change const& c1, change const& c2);
   friend bool operator!=(change const& c1, change const& c2);
   friend change extend(change const& c, int n);
};

// construct a change from place notation
change pn(int Size, std::string const& px);

// comparison operator on change, for convenience.  The ordering is stable but not documented.
bool operator<(change const& c1, change const& c2);

bool operator==(change const& c1, change const& c2);

bool operator!=(change const& c1, change const& c2);

// extend the change by adding n bells (tenors) at the back
change extend(change const& c, int n);

// Write a change to a stream
std::ostream& operator<<(std::ostream& out, change const& c);

#include "change.cc"

#endif
