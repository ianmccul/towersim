// -*- C++ -*-

// quartic fit to a data set

#if !defined(QUARTICFIT_H)
#define QUARTICFIT_H

#include "gauss.h"
#include <vector>
#include <tuple>

struct quartic_fit_result
{
   // Quartic fit is a*x^4 + b*x^3 + c*x^2 + d*x + e;
   double a, b, c, d, e;
};

// returns the best fit quartic
quartic_fit_result
quartic_fit(std::vector<double> const& x, std::vector<double> const& y);

inline
quartic_fit_result
quartic_fit(std::vector<double> const& x, std::vector<double> const& y)
{
   // matrix equations
   // [ X8 X7 X6 X5 X4 ] [ a ]   [ Y4 ]
   // [ X7 X6 X5 X4 X3 ] [ b ] = [ Y3 ]
   // [ X6 X5 X4 X3 X2 ] [ c ]   [ Y2 ]
   // [ X5 X4 X3 X2 X1 ] [ d ]   [ Y1 ]
   // [ X4 X3 X2 X1 X0 ] [ e ]   [ Y0 ]

   double X0=x.size(), X1=0, X2=0, X3=0, X4=0, X5=0, X6=0, X7=0, X8=0;
   double Y0=0, Y1=0, Y2=0, Y3=0, Y4=0;

   for (unsigned i = 0; i < x.size(); ++i)
   {
      X1 += x[i];
      X2 += std::pow(x[i],2);
      X3 += std::pow(x[i],3);
      X4 += std::pow(x[i],4);
      X5 += std::pow(x[i],5);
      X6 += std::pow(x[i],6);
      X7 += std::pow(x[i],7);
      X8 += std::pow(x[i],8);
      Y0 += y[i];
      Y1 += y[i]*x[i];
      Y2 += y[i]*std::pow(x[i],2);
      Y3 += y[i]*std::pow(x[i],3);
      Y4 += y[i]*std::pow(x[i],4);
   }

   // Gaussian elimination

   std::vector<std::vector<double>> Mat = {{X8, X7, X6, X5, X4, Y4},
                                           {X7, X6, X5, X4, X3, Y3},
                                           {X6, X5, X4, X3, X2, Y2},
                                           {X5, X4, X3, X2, X1, Y1},
                                           {X4, X3, X2, X1, X0, Y0}};

   std::vector<double> R = gauss(Mat);

   return {R[0], R[1], R[2], R[3], R[4]};
}

#endif
