// -*- C++ -*- $Id: gyrofit-reader.cpp 1876 2016-10-02 13:46:09Z uqimccul $
//
// daemon for a 'bottom dead centre' sensor via gyro, using
// a quadratic fit.  By inspection, we judge a reasonable fitting
// procedure is about 0.15 seconds of data, approx 100 - 130 samples.
//
// We use a circular_buffer that can hold BufMax samples (around 100 - 130).
// If the maximum velocity (absolute value) is greater than Threshold (around 1000 - 2000),
// and occurs in the middle of the buffer (by time), then we treat this as a local extremum
// and do a quadratic fit to find the location of the extremum and the velocity.
//
// After testing, we note that it sometimes false-triggers on a stay incident.
// It seems that this is difficult to detect from first principles,
// the curvature / velocity doesn't
// differ significantly from BDC when raising/lowering the bell.  We could make a check
// based on a minimum time between BDC, and/or the change in velocity at BDC.
// It is reasonable to assume that the change in squared velocity (ie, the kinetic energy)
// is smaller than say 2e8.

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <cmath>
#include <boost/circular_buffer.hpp>
#include "gyro-bdc.h"

int main(int argc, char** argv)
{
   std::cout.precision(16);
   std::cerr.precision(16);
   int64_t Epoch = 0;
   int64_t Tm;
   std::string G;
   float z;

   GyroBDC BDC;

   while (std::cin >> Tm >> z)
   {
      if (Epoch == 0)
         Epoch = Tm;

      if (BDC.Process(Tm-Epoch, z))
      {
         //         std::cout << double(BDC.BDCPoints.front().first)/1E6 << ' ' <<  BDC.BDCPoints.front().second << '\n';
         std::cout << (BDC.BDCPoints.front().first+Epoch) << ' ' <<  BDC.BDCPoints.front().second << '\n';
         BDC.BDCPoints.pop_front();
      }
   }
}
