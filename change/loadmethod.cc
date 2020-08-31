
#include <cctype>

std::vector<std::string>
Split(std::string const& q, char Delim)
{
   std::vector<std::string> Result;
   // [q_beg, q_end) is a subsequence terminated by Delim
   std::string::const_iterator q_beg = q.begin();
   std::string::const_iterator q_end = std::find(q_beg, q.end(), Delim);

   // loop until our subsequence is terminated by q.end()
   while (q_end != q.end())
   {
      Result.push_back(std::string(q_beg, q_end));

      q_beg = q_end;
      ++q_beg;          // skip over Delim

      q_end = std::find(q_beg, q.end(), Delim);
   }

   // output the final subsequence
   if (q_beg != q_end)
   {
      Result.push_back(std::string(q_beg, q_end));
   }
   return Result;
}

bool is_digit_string(std::string const& s)
{
   for (std::string::const_iterator I = s.begin(); I != s.end(); ++I)
   {
      if (!std::isdigit(*I))
         return false;
   }
   return true;
}

template <typename OutIter>
void load_methods(std::istream& in, int Stage, std::string const& Class, 
                  OutIter Iter)
{
   int HasHalfLead = -1;
   int HasLeadEnd = -1;
   while (in)
   {
      std::string S;
      getline(in, S);

      std::vector<std::string> Strings = Split(S, '\t');

      if (Strings.size() == 0)
         continue;

      if (Strings[0] == "No.")
      {
         // header.  Determine whether it is palindromic or not.
         if (Strings[1] != "Name")
         {
            TRACE("Name expected");
            return;
         }
         if (Strings[2] != "Notation")
         {
            TRACE("Notation expected");
            return;
         }

         HasHalfLead = -1;
         HasLeadEnd = -1;

         if (Strings.size() > 3 && Strings[3] == "hl")
            HasHalfLead = 3;
         
         if (Strings.size() > 3 && Strings[3] == "le")
            HasLeadEnd = 3;

         if (Strings.size() > 4 && Strings[4] == "hl")
            HasHalfLead = 4;
         
         if (Strings.size() > 4 && Strings[4] == "le")
            HasLeadEnd = 4;

      }
      else if (is_digit_string(Strings[0]))
      {
         // we have a method
         std::string Name = Strings[1];
         std::string FullName = Name + ' ';
         if (Class != "")
            FullName = FullName + Class + ' ';
         FullName = FullName + stage_name(Stage);

         std::string Notation = Strings[2];
         std::string LeadEnd;
         if (HasHalfLead != -1)
         {
            Notation = '&' + Notation + Strings[HasHalfLead];
         }

         if (HasLeadEnd != -1)
            LeadEnd = Strings[HasLeadEnd];
         else
            LeadEnd = "";

         //TRACE("Got a method")(Name)(FullName)(Notation)(LeadEnd);

         method MyMethod = method::create_regular(Name, Stage, Notation, LeadEnd);
         MyMethod.set_full_name(FullName);
         *Iter = MyMethod;
         ++Iter;
      }
   }
}

         
