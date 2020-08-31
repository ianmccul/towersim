// -*- C++ -*- $Id: labels.cpp 913 2012-02-19 12:32:07Z ianmcc $

#include "labels.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <numeric>

double nicenum_round(double x)
{
   double e = std::floor(std::log10(x));
   double f = x / std::pow(10.0, e);
   double nf;
   if (f < 1.5)
      nf = 1.0;
   else if (f < 3.0)
      nf = 2.0;
   else if (f < 7.0)
      nf = 5.0;
   else
      nf = 10.0;
   return nf * std::pow(10, e);
}

double nicenum_ceil(double x)
{
   double e = std::floor(std::log10(x));
   double f = x / std::pow(10.0, e);
   double nf;
   if (f < 1.0)
      nf = 1.0;
   else if (f < 2.0)
      nf = 2.0;
   else if (f < 5.0)
      nf = 5.0;
   else
      nf = 10.0;
   return nf * std::pow(10, e);
}

std::vector<std::pair<double, std::string> >
GraphLabels(double Min, double Max, int nticks)
{
   std::vector<std::pair<double, std::string> > Result;

   double Range = nicenum_ceil(Max-Min);
   double D = nicenum_round(Range / (nticks-1));
   double GraphMin = std::floor(Min/D)*D;
   double GraphMax = std::ceil(Max/D)*D;
   int nfrac = std::max(int(-floor(std::log10(D))), 0);

   for (double x = GraphMin; x < GraphMax+(D*0.5); x += D)
   {
      std::ostringstream Out;
      Out << std::fixed << std::setprecision(nfrac) << x;
      Result.push_back(std::make_pair(x, Out.str()));
   }

   return Result;
}
