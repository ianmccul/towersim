// -*- C++ -*-

//
// Cubic spline.  A cubic spline is defined over 4 control points (knots).
// The basis function, for 0 <= x <= 4 is
// f(x) = x^3 / 6                 ; 0 < x < 1
//      = 2/3 - 1/2 x (x-2)^2     ; 1 < x < 2
//      = 2/3 - 1/2 (4-x) (x-2)^2 ; 2 < x < 3
//      = (4-x)^3/6               ; 3 < x < 4
//
// (This can be derived from recursive formulas, see eg de Boor or Eilers/Marx
// The cubic spline is continuous, and has continuous first and second derivatives.
// The third derivative is a step function, +1 between (0,1), -3 between (1,2),
// +3 between (2,3) and -1 between (3,4).
//
// Normalizing between [0,1], we have 4 functions.
// f_0(x) = x^3/6
// f_1(x) = 2/3 - 1/2 (x+1) (x-1)^2
// f_2(x) = 2/3 - 1/2 x^2 (2-x)
// f_3(x) = (1-x)^3 / 6
//
// For a spline centered around x=0, the functional form is
//
// f(x) = (x+2)^3 / 6            ; -2 < x < -1
//      = 2/3 - 1/2 (x+2) x^2    ; -1 < x <  0
//      = 2/3 - 1/2 x^2 (2-x)    ;  0 < x <  1
//      = (2-x)^3 / 6
//
// The CubicSpline class is for fitting functions to cubic B-splines over a
// regularly spaced grid.  The fitting interval is N intervals from x0 to x1
// (so N+1 knots within the fitting interval).  The cubic fit also requires
// knots at -2,-1 and N+1,N+2, hence the interval of t is (-2, N+2),
// and the spline basis is defined at discrete points t=-2,-1,0,...,N,N+1,N+2
//
// To fit data against the B-spline we minimise the residual.  The spline
// is represented as an N+5-dimension vector 'a'.  The fitted function is
// fitted(x) = sum_{i=-2}^{N+2} a[i] * BasisFunction(i,t)
// where t = N*(x-x0)/(x1-x0)
//
// Alternatively, find i = floor(t).  Then
// fitted(x) = a[i]   * BasisFunction(t-i+1)
//           + a[i+1] * BasisFunction(t-i)
//           + a[i+2] * BasisFunction(t-i-1)
//           + a[i+3] * BasisFunction(t-i-2)
//
// remembering that a[0] represents the knot at t=-2.
// Thus for each function value (x,y) we associate a row of the matrix M
// as ( 0, 0, ..., BasisFunction(t-i-2), BasisFunction(t-i-1),
//      BasisFunction(t-i), BasisFunction(t-i+1), ... )
// where the first non-zero entry is at position 'i'
// Hence we have r = M*a - y, and minimising the residual is equivalent to the least-squares
// solution for a of M*a = y.

#if !defined(SPLINE_H)
#define SPLINE_H

#include <vector>
#include <utility>
#include <cmath>

class CubicSpline
{
   public:
      // N is the number of intervals.  Number of knots is N+5,
      // with values t = -2,-1,...,N,N+1,N+2
      // x = (x1-x0)*t/N + x0
      CubicSpline(double x0_, double x1_, int N_);

      // The data to be fitted, as pairs (x,y).
      void LoadData(std::vector<std::pair<double, double>> Data_);

      // Do the actual fit.
      // precondition: Data has been loaded
      void Fit();

      // return the value of the spline at x
      // precondition: x0 <= x <= x1 && Fit() has been called
      double operator[](double x) const;

      // returns the derivative of the spline at x
      // precondition: x0 <= x <= x1 && Fit() has been called
      double deriv(double x) const;

      // returns the second derivative of the spline at x
      // precondition: x0 <= x <= x1 && Fit() has been called
      double deriv2(double x) const;

      // returns the value of the spline at the n'th basis point,
      // -2 <= n <= N+2
      // precondition: Fit() has been called
      double BasisValue(int n) const
      {
         return a[n+2];
      }

      // The k'th basis function, where the peak of the function is at t=k,
      // and has non-zero support on (k-2,k+2)
      double BasisFunction(double k, double t) const;

      // the basis function with peak at t=0
      double BasisFunction(double t) const;

      // The basis function split out into 4 sectors.
      // precondition: 0 <= t <= 1
      double BasisFunction0(double t) const;
      double BasisFunction1(double t) const;
      double BasisFunction2(double t) const;
      double BasisFunction3(double t) const;

      double BasisFunctionDeriv0(double t) const;
      double BasisFunctionDeriv1(double t) const;
      double BasisFunctionDeriv2(double t) const;
      double BasisFunctionDeriv3(double t) const;

      double BasisFunctionSecondDeriv0(double t) const;
      double BasisFunctionSecondDeriv1(double t) const;
      double BasisFunctionSecondDeriv2(double t) const;
      double BasisFunctionSecondDeriv3(double t) const;

   private:
      double x0;
      double x1;
      int N;
      std::vector<std::pair<double, double>> Data;
      // vector of fited values of the B-spline basis functions.
      // Length is N+5
      std::vector<double> a;
};

inline
double
CubicSpline::BasisFunction(double k, double t) const
{
   return this->BasisFunction(t-k);
}

inline
double
CubicSpline::BasisFunction0(double t) const
{
   return std::pow(t,3) / 6.0;
}

inline
double
CubicSpline::BasisFunction1(double t) const
{
   return 2.0/3.0 - 0.5*(t+1) * std::pow(t-1,2);
}

inline
double
CubicSpline::BasisFunction2(double t) const
{
   return 2.0/3.0 - 0.5*std::pow(t,2) * (2-t);
}

inline
double
CubicSpline::BasisFunction3(double t) const
{
   return std::pow(1-t, 3) / 6.0;
}

inline
double
CubicSpline::BasisFunctionDeriv0(double t) const
{
   return std::pow(t,2) / 2.0;
}

inline
double
CubicSpline::BasisFunctionDeriv1(double t) const
{
   return (t+1)*(1-t) - 0.5*std::pow(t-1,2);
}

inline
double
CubicSpline::BasisFunctionDeriv2(double t) const
{
   return -t*(2-t) + 0.5*std::pow(t,2);
}

inline
double
CubicSpline::BasisFunctionDeriv3(double t) const
{
   return -std::pow(1-t,2) / 2.0;
}

inline
double
CubicSpline::BasisFunctionSecondDeriv0(double t) const
{
   return t;
}

inline
double
CubicSpline::BasisFunctionSecondDeriv1(double t) const
{
   return 1-3*t;
}

inline
double
CubicSpline::BasisFunctionSecondDeriv2(double t) const
{
   return 3*t-2;
}

inline
double
CubicSpline::BasisFunctionSecondDeriv3(double t) const
{
   return 1-t;
}

inline
double
CubicSpline::BasisFunction(double t) const
{
   // which sector are we in?
   int Sector = std::floor(t);
   if (Sector < -2)
      return 0.0;
   else if (Sector < -1)
      return std::pow(t-2, 3) / 6.0;
   else if (Sector < 0)
      return 2.0/3.0 - 0.5*(t+2.0) * std::pow(t,2);
   else if (Sector < 1)
      return 2.0/3.0 - 0.5*std::pow(t,2) * (2.0-t);
   else if (Sector < 2)
      return std::pow(2-t, 3) / 6.0;
   // else
   return 0.0;
}

#endif
