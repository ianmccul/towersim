// -*- C++ -*-

#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include "common/randutil.h"
#include <tuple>
#include <algorithm>
#include <string>
#include <deque>

constexpr double g = 9.80665;

std::tuple<double, double>
Correction(double ax, double ay)
{
   double rax = -1.026667e-1*ax + -1.951888e-4*ay + -2.9487651e-3;
   double ray =  4.200339e-5*ax + -1.025088e-1*ay +  2.2330396e-2;

   //   double rax = -1.026667e-1*ax - 1.951888e-4*ay - 2.9487651e-3;
   //   double ray =  4.200339e-5*ax - 1.025088e-1*ay + 2.2330396e-2;


   return std::make_tuple(rax, ray);
}

int main(int argc, char** argv)
{
   randutil::seed();

   uint64_t Time;
   double ax, ay;
   double last_gyro = -1000;
   while (std::cin >> Time >> ax >> ay)
   {
      std::tie(ax, ay) = Correction(ax, ay);
      std::cout << Time << ' ' << std::atan2(-ax,-ay) * 180.0 / M_PI << std::endl;
   }
}
