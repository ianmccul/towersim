// -*- C++ -*- $Id: labels.h 912 2012-02-19 12:28:24Z ianmcc $
//
// 'loose' labels, from Graphics Gems, Andrew S Glassner

#if !defined(LABELS_H_JDFHUIWERYO)
#define LABELS_H_JDFHUIWERYO

#include <vector>
#include <string>
#include <utility>

// calculate 'nice' labels for a linear scale.
// precondition: MaxTicks >= 2 && Max > Min
std::vector<std::pair<double, std::string> >
   GraphLabels(double Min, double Max, int MaxTicks);

#endif
