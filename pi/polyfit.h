// -*- C++ -*-

// polynomial fit to a data set

#if !defined(POLYFIT_H)
#define POLYFIT_H

#include <vector>

// return vector Result[n] is coefficient of degree n
std::vector<double>
polynomial_fit(std::vector<double> const& x, std::vector<double> const& y, int Degree);

#endif
