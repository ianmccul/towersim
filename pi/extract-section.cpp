// -*- C++ -*-
//
// Extract a section of a file based on the timestamps
//

#include <vector>
#include <iostream>

int main(int argc, char** argv)
{
   if (argc < 3)
   {
      std::cerr << "usage: " << argv[0] << " <start> <stop>\n";
   }

   uint64_t t0 = std::atol(argv[1]);
   uint64_t t1 = std::atol(argv[2]);

   std::string s;
   while (std::getline(std::cin, s))
   {
      uint64_t t = stoll(s);
      if (t >= t0 && t <= t1)
      {
         std::cout << s << '\n';
      }
   }
}
