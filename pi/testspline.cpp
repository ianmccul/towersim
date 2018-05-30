
#include "spline.h"
#include <iostream>

int main()
{
   uint64_t t;
   double y;
   uint64_t Epoch = 0;
   double a = 0;
   double lastx=0, lasty=0;
   std::vector<std::pair<double,double>> Data;
   while (std::cin >> t >> y)
   {
      if (Epoch == 0)
         Epoch = t;
      double x = (t-Epoch) * 1e-6;
      a += 0.5*(lasty+y) * (x-lastx);
      Data.push_back({x,a});
      lastx = x;
      lasty = y;
   }

   double x0 = Data.front().first;
   double x1 = Data.back().first;

   // adjust the offset
   std::cerr << "Final angle is " << a << '\n';
   for (unsigned i = 0; i < Data.size(); ++i)
   {
      Data[i].second -= a * (Data[i].first - x0) / (x1-x0);
   }

   CubicSpline Spline(x0, x1, 50);

   Spline.LoadData(Data);

   Spline.Fit();

   int N = 1000;
   for (int i = 0; i <= N; ++i)
   {
      double x = x0 + (x1-x0)*double(i)/double(N);
      std::cout << x << ' ' << Spline[x] << ' ' << Spline.deriv(x) << ' ' << Spline.deriv2(x) << '\n';
   }
}
