// -*- C++ -*-

// quadratic fit to a data set

#if !defined(QUADFIT_H)
#define QUADFIT_H

#include <vector>
#include <tuple>

struct quadratic_fit_result
{
   // Quadratic fit is a*x^2 + b*x + c
   double a, b, c;
   //double r; // residual
};

// returns the best fit quadratic
quadratic_fit_result
quadratic_fit(std::vector<double> const& x, std::vector<double> const& y);

inline
quadratic_fit_result
quadratic_fit(std::vector<double> const& x, std::vector<double> const& y)
{
   // matrix equations
   // [ X4 X3 X2 ] [ a ]   [ Y2 ]
   // [ X3 X2 X1 ] [ b ] = [ Y1 ]
   // [ X2 X1 X0 ] [ c ]   [ Y0 ]

   // The residual is ??

   double X0=x.size(), X1=0, X2=0, X3=0, X4=0;
   double Y0=0, Y1=0, Y2=0;

   for (unsigned i = 0; i < x.size(); ++i)
   {
      X1 += x[i];
      X2 += std::pow(x[i],2);
      X3 += std::pow(x[i],3);
      X4 += std::pow(x[i],4);
      Y0 += y[i];
      Y1 += y[i]*x[i];
      Y2 += y[i]*std::pow(x[i],2);
   }

   // From Cramer's rule, the solutions are given by the determinant ratios
   // a=A/D
   // b=B/D
   // c=C/D
   //           | X4 X3 X2 |
   // where D = | X3 X2 X1 |
   //           | X2 X1 X0 |
   //
   //     | Y2 X3 X2 |
   // A = | Y1 X2 X1 |
   //     | Y0 X1 X0 |
   //
   //     | X4 Y2 X2 |
   // B = | X3 Y1 X1 |
   //     | X2 Y0 X0 |
   //
   //     | X4 X3 Y2 |
   // C = | X3 X2 Y1 |
   //     | X2 X1 Y0 |

   double D = X0*X2*X4 - X1*X1*X4 - X0*X3*X3 + 2*X1*X2*X3 - X2*X2*X2;

   double a = (Y0*X1*X3 - Y1*X0*X3 - Y0*X2*X2 + Y1*X1*X2 + Y2*X0*X2 - Y2*X1*X1) / D;
   double b = (Y1*X0*X4 - Y0*X1*X4 + Y0*X2*X3 - Y2*X0*X3 - Y1*X2*X2 + Y2*X1*X2) / D;
   double c = (Y0*X2*X4 - Y1*X1*X4 - Y0*X3*X3 + Y1*X2*X3 + Y2*X1*X3 - Y2*X2*X2) / D;

#if 0
   double r = 0;
   for (unsigned i = 0; i < x.size(); ++i)
   {
      r += std::pow(a*x[i]*x[i] + b*x[i] + c - y[i], 2);
   }
#endif

   return {a,b,c};
}



#endif
