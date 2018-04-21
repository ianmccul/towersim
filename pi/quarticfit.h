// -*- C++ -*-

// quartic fit to a data set

#if !defined(QUARTICFIT_H)
#define QUARTICFIT_H

#include <vector>
#include <tuple>

struct quartic_fit_result
{
   // Quartic fit is a*x^4 + b*x^3 + c*x^2 + d*x + e;
   double a, b, c, d, e;
};


std::vector<double>
gauss(std::vector<std::vector<double>> A)
{
   int n = A.size();

   for (int i=0; i<n; i++) {
      // Search for maximum in this column
      double maxEl = abs(A[i][i]);
      int maxRow = i;
      for (int k=i+1; k<n; k++)
      {
         if (abs(A[k][i]) > maxEl)
         {
            maxEl = abs(A[k][i]);
            maxRow = k;
         }
      }

      // Swap maximum row with current row
      for (int k=i; k<n+1;k++)
      {
         std::swap(A[maxRow], A[i]);
      }

      // Make all rows below this one 0 in current column
      for (int k=i+1; k<n; k++) {
         double c = -A[k][i]/A[i][i];
         for (int j=i; j<n+1; j++)
         {
            if (i==j)
            {
               A[k][j] = 0;
            } else {
               A[k][j] += c * A[i][j];
            }
         }
      }
   }

    // Solve equation Ax=b for an upper triangular matrix A
    std::vector<double> x(n);
    for (int i=n-1; i>=0; i--)
    {
        x[i] = A[i][n]/A[i][i];
        for (int k=i-1;k>=0; k--)
        {
            A[k][n] -= A[k][i] * x[i];
        }
    }
    return x;
}

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