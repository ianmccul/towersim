// ----------------------------------------------------------------------------
// Numerical diagonalization of 3x3 matrcies
// Copyright (C) 2006  Joachim Kopp
// Copyright (C) 2016  Ian McCulloch
// ----------------------------------------------------------------------------
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#include <cmath>

// ----------------------------------------------------------------------------
// Reduces a symmetric 3x3 matrix to tridiagonal form by applying
// (unitary) Householder transformations:
//            [ d[0]  e[0]       ]
//    A = Q . [ e[0]  d[1]  e[1] ] . Q^T
//            [       e[1]  d[2] ]
// The function accesses only the diagonal and upper triangular parts of
// A. The access is read-only.
// ---------------------------------------------------------------------------
template <typename T>
void dsytrd3(matrix3<T> const& A, matrix3<T>& Q, vector3<T>& d, vector3<T>& e)
{
   using std::abs;

   const int n = 3;
   vector3<T> u;

   // Initialize Q to the identitity matrix
   Q = matrix3<T>::identity();

   // Bring first row and column to the desired form
   T h = A(0,1)*A(0,1) + A(0,2)*A(0,2);
   T g = sqrt(h);
   if (A(0,1) > 0)
      g = -g;

   e[0] = g;
   T f = g * A(0,1);
   u[1] = A(0,1) - g;
   u[2] = A(0,2);

   T omega = h - f;
   if (omega > 0.0)
   {
      omega = 1.0 / omega;
      T K = 0.0;
      vector3<T> q;
      for (int i=1; i < n; i++)
      {
         f = A(1,i) * u[1] + A(i,2) * u[2];
         q[i] = omega * f;                  // p
         K += u[i] * f;                   // u* A u
      }
      K *= 0.5 * omega * omega;

      for (int i=1; i < n; i++)
         q[i] = q[i] - K * u[i];

      d[0] = A(0,0);
      d[1] = A(1,1) - 2.0*q[1]*u[1];
      d[2] = A(2,2) - 2.0*q[2]*u[2];

      // Store inverse Householder transformation in Q
      for (int j=1; j < n; j++)
      {
         f = omega * u[j];
         for (int i=1; i < n; i++)
            Q(i,j) -= f*u[i];
      }

      // Calculate updated A[1][2] and store it in e[1]
      e[1] = A[1][2] - q[1]*u[2] - u[1]*q[2];
   }
   else
   {
      for (int i=0; i < n; i++)
         d[i] = A(i,i);
      e[1] = A(1,2);
   }
}

// ----------------------------------------------------------------------------
// Calculates the eigenvalues and normalized eigenvectors of a symmetric 3x3
// matrix A using the QL algorithm with implicit shifts, preceded by a
// Householder reduction to tridiagonal form.
// The function accesses only the diagonal and upper triangular parts of A.
// The access is read-only.
// ----------------------------------------------------------------------------
// Parameters:
//   A: The symmetric input matrix
//   Q: Storage buffer for eigenvectors
//   w: Storage buffer for eigenvalues
// ----------------------------------------------------------------------------
// Return value:
//   0: Success
//  -1: Error (no convergence)
// ----------------------------------------------------------------------------
template <typename T>
int eigenvectors_symmetric(matrix3<T> const& A, matrix3<T>& Q, vector3<T>& w)
{
   using std::abs;

   const int n = 3;
   vector3<T> e;    // The third element is used only as temporary workspace
   T g, r, p, f, b, s, c, t; // Intermediate storage
   int nIter;
   int m;

   // Transform A to real tridiagonal form by the Householder method
   dsytrd3(A, Q, w, e);

   // Calculate eigensystem of the remaining real symmetric tridiagonal matrix
   // with the QL method
   //
   // Loop over all off-diagonal elements
   for (int l=0; l < n-1; l++)
   {
      nIter = 0;
      while (1)
      {
         // Check for convergence and exit iteration loop if off-diagonal
         // element e(l) is zero
         for (m=l; m <= n-2; m++)
         {
            g = abs(w[m])+abs(w[m+1]);
            if (abs(e[m]) + g == g)
               break;
         }
         if (m == l)
            break;

         if (nIter++ >= 30)
            return -1;

         // Calculate g = d_m - k
         g = (w[l+1] - w[l]) / (e[l] + e[l]);
         r = sqrt(g*g + 1.0);
         if (g > 0)
            g = w[m] - w[l] + e[l]/(g + r);
         else
            g = w[m] - w[l] + e[l]/(g - r);

         s = c = 1.0;
         p = 0.0;
         for (int i=m-1; i >= l; i--)
         {
            f = s * e[i];
            b = c * e[i];
            if (abs(f) > abs(g))
            {
               c      = g / f;
               r      = sqrt(c*c + 1.0);
               e[i+1] = f * r;
               c     *= (s = 1.0/r);
            }
            else
            {
               s      = f / g;
               r      = sqrt(s*s + 1.0);
               e[i+1] = g * r;
               s     *= (c = 1.0/r);
            }

            g = w[i+1] - p;
            r = (w[i] - g)*s + 2.0*c*b;
            p = s * r;
            w[i+1] = g + p;
            g = c*r - b;

            // Form eigenvectors
            for (int k=0; k < n; k++)
            {
               t = Q[k][i+1];
               Q[k][i+1] = s*Q[k][i] + c*t;
               Q[k][i]   = c*Q[k][i] - s*t;
            }
         }
         w[l] -= p;
         e[l]  = g;
         e[m]  = 0.0;
      }
   }

   return 0;
}
