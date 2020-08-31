
#include "labels.h"
#include <iostream>

int main()
{
   double Min  = 0.005;
   double Max = 0.032;
   for (int n = 2; n <= 10; ++n)
   {
      std::cout << "\nn=" << n << '\n';
      std::vector<std::pair<double, std::string> > Labels
	 = GraphLabels(Min, Max, n);

      for (int i = 0; i < Labels.size(); ++i)
      {
	 std::cout << Labels[i].first << ' ' << Labels[i].second << '\n';
      }
   }

}
