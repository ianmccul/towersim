// -*- C++ -*- $Id: solver.cpp 1843 2016-09-22 14:11:12Z uqimccul $

#include "rungekutta.h"
#include <cmath>
#include <iostream>
#include <iomanip>

double const pi = M_PI;

double const g = 9.81;    // acceleration due to gravity
double const lb = 0.640;  // effective length of the bell
double const r = 0.5;     // radius of the accelerometer (distance from the axis)
double beta = pi+1*pi/180;          // angular position of the accelerometer

double
thetadot(double theta, double omega, double t)
{
   return omega;
}

double
omegadot(double theta, double omega, double t)
{
   return -(g / lb) * std::sin(theta);
}

int main()
{
   // initial conditions
   double theta = pi - 0.05;
   double omega = 0;

   // timestep
   double t = 0;
   double timestep = 0.01;
   double tfinal = 10;
   int N = int(ceil((tfinal - t) / timestep));

   std::cout << "#t   #theta   #omega   #xddot    #yddot\n";
   for (int i = 0; i < N; ++i)
   {
      RungeKutta(theta, omega, t, thetadot, omegadot, timestep);

      // forces on acceleromater
      double xddot = -r*(g/lb)*std::sin(theta) + g*std::sin(theta+beta);
      double yddot = -r*omega*omega - g*std::cos(theta+beta);

      std::cout << t << ' ' << theta << ' ' << omega << ' ' << xddot << ' ' << yddot << '\n';
   }
}
