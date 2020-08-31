
#include "loadmethod.h"

#include <fstream>

int main()
{
   std::ifstream In("/home/ian/ringinglib/surp6.txt");

   std::list<method> MyLib;
   
   load_methods(In, 6, "Surprise", std::back_inserter(MyLib));
}
