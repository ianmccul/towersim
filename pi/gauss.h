// -*- C++ -*-

// Solve equations using Gaussian elimination
//
// Assumes an N x (N+1) matrix, as a vector of vectors.
// Define the matrix as
//
// [ A11 A12 A13 ... A1N b1 ]
// [ A21 A22 A23 ... A2N b2 ]
// [ ...                 ...]
// [ AN1 AN2 AN3 ... ANN bN ]
//
// to solve the problem Ax=b

#if !defined(GAUSS_H)
#define GAUSS_H

#include <vector>

inline
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

#endif
