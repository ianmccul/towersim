// -*- C++ -*- $Id: solver.cpp 1120 2013-01-11 12:12:58Z uqimccul $

#include "rungekutta.h"
#include <cmath>
#include <iostream>
#include <iomanip>

double const pi = M_PI;

double const g = 9.81;    // acceleration due to gravity
double const lb = 0.640;  // effective length of the bell
double const lw = 1.5;    // radius of the wheel
double const r = 0.5;     // radius of the accelerometer
double beta = pi;          // angular position of the accelerometer

// angular position of the garter hole
double const g_gamma = 3 * pi / 4;

// height of the pully relative to the bottom of the wheel
double const PullyHeight = 0;

double const theta_0 = 0.02 * pi / 180.0;
double const lr = 0.001; //lw * theta_0;

#if 0
double RopeHeight(double theta)
{
   if (theta > pi / 2)
   {
      return lw - PullyHeight + lw*(theta - pi/2);
   }
   if (theta < 0)
   {
      // TODO: this doesn't allow for PullyHeight != 0
      return lw - lw*theta;
   }
   // here the rope is travelling in a straight line from the garter hole to the pully
   // get the coordinates of the garter hole, relative to the pully location
   double gx = lw * (1 - sin(theta));
   double gy = lw * (1-cos(theta)) - PullyHeight;
   return std::hypot(gx,gy);
}

double RopeVelocity(double thetad, double theta)
{
   if (theta > pi / 2)
   {
      return lw*thetad;
   }
   if (theta < 0)
   {
      // TODO: this doesn't allow for PullyHeight != 0
      return -lw*thetad;
   }
   double gx = lw * (1 - sin(theta));
   double gy = lw * (1 - cos(theta)) - PullyHeight;
   double length = std::hypot(gx,gy);

   // d(length)/dt = d(length)/dx dx/dt + d(length)/dy dy/dt
   // double dx_dt = -lw*cos(theta) * thetad;
   // double dy_dt = lw*sin(theta) * thetad;
   //double Numerator = lw*lw*(sin(theta) - cos(theta)) -lw*sin(theta)*PullyHeight;
   double Numerator = lw*lw*std::sqrt(2.0)*sin(theta-pi/4) -lw*sin(theta)*PullyHeight;
   return (thetad * Numerator) / length;
}
#endif

// partial derivative of the rope velocity with respect to theta)
double RopeVelocity2_dtheta(double thetad, double theta)
{
   if (theta > pi / 2)
   {
      return 0;
   }
   if (theta < 0)
   {
      return 0;
   }
}


double
thetadot(double theta, double omega, double t)
{
   return omega;
}

double n(double theta)
{
   return (theta+g_gamma)*(4/pi) - 1;
}

double
omegadot(double theta, double omega, double t)
{
   if (theta+g_gamma > pi/2)
   {
      return (-g / (lb + lr)) * (std::sin(theta) + theta_0);
   }
   else if (theta+g_gamma < 0)
   {
      return (-g / (lb + lr)) * (std::sin(theta) - theta_0);
   }
   double numerator = -lr*(4.0 / pi)*n(theta)*omega*omega - g*(std::sin(theta) + theta_0*n(theta));
   double denominator = lb + lr*std::pow(n(theta),2);

   return numerator/denominator;
}

int main()
{
   double theta = -pi+0.01;
   //double theta = 1;
   double omega = 0;
   double t = 0;
   double timestep = 0.001;
   double tfinal = 30;
   int N = int(ceil((tfinal - t) / timestep));

   double xddot = -r*(g/lb)*std::sin(theta) + g*std::sin(theta+beta);
   double yddot = -r*omega*omega - g*std::cos(theta+beta);

   // force on the bell due to the wheel
   double force = omegadot(theta, omega, t) + (g/(lb+lr))*std::sin(theta);
   double Energy = 0.5*(lb+lr)*omega*omega -g*cos(theta);

   std::cout << t << ' ' << theta << ' ' << omega << ' ' << xddot << ' ' << yddot << ' ' << Energy << ' ' << force <<  '\n';
   for (int i = 0; i < N; ++i)
   {
      RungeKutta(theta, omega, t, thetadot, omegadot, timestep);
      xddot = r*omegadot(theta, omega, t) + g*std::sin(theta+beta);
      yddot = -r*omega*omega - g*std::cos(theta+beta);
      force = omegadot(theta, omega, t) + (g/(lb+lr))*std::sin(theta);
      Energy = 0.5*(lb+lr)*omega*omega -g*cos(theta);
      std::cout << t << ' ' << theta << ' ' << omega << ' ' << xddot << ' ' << yddot << ' ' << Energy << ' ' << force << '\n';
   }
}
