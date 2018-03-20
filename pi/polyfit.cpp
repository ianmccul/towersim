// -*- C++ -*-

#include "polyfit.h"
#include <gsl/gsl_multifit.h>
#include <cmath>
#include <cassert>

std::vector<double>
polynomial_fit(std::vector<double> const& dx, std::vector<double> const& dy, int Degree)
{
   int Size = dx.size();
   assert(Size == dy.size());

   gsl_matrix* X = gsl_matrix_alloc(Size, Degree);
   gsl_vector* y = gsl_vector_alloc(Size);
   gsl_vector* c = gsl_vector_alloc(Degree);
   gsl_matrix* cov = gsl_matrix_alloc(Degree, Degree);

   for(unsigned i = 0; i < Size; i++)
   {
      for(unsigned j = 0; j < Degree; j++)
      {
         gsl_matrix_set(X, i, j, std::pow(dx[i], j));
      }
      gsl_vector_set(y, i, dy[i]);
   }

   gsl_multifit_linear_workspace* ws = gsl_multifit_linear_alloc(Size, Degree);
   double chisq;
   gsl_multifit_linear(X, y, c, cov, &chisq, ws);

   // Store result
   std::vector<double> Result(Degree);
   for(unsigned i = 0; i < Degree; i++)
   {
      Result[i] = gsl_vector_get(c, i);
   }

   // Free the arrays
   gsl_multifit_linear_free(ws);
   gsl_matrix_free(X);
   gsl_matrix_free(cov);
   gsl_vector_free(y);
   gsl_vector_free(c);
}
