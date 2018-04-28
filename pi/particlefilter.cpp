// -*- C++ -*-

// A particle filter for processing gyro readings

#include "filter.h"
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include "common/randutil.h"
#include <fstream>

timeval Epoch;

double GainFactor = 1; // 0.99844; // 1.0;
double GyroQuadraticCorrection = 0; // -0.000164831; //0;

int main(int argc, char** argv)
{
   std::ifstream BellsConfig("bells.json");
   json Bells;
   BellsConfig >> Bells;
   LoadBellsJSON(Bells);

   BellInfoType ThisBell = BellByName("5");
   //ThisBell.kb = 0;

   if (argc >= 2)
   {
      GainFactor = std::stod(std::string(argv[1]));
      std::cerr << "Using GainFactor = " << GainFactor << '\n';
      if (argc >= 3)
      {
         double l_b = std::stod(std::string(argv[2]));
         std::cerr << "Using l_b = " << l_b << '\n';
         ThisBell.lb = l_b;
      }
   }
   randutil::seed();

   ParticleFilter Filter;
   Filter.Initialize(ThisBell, 2000, to_rad(0), 0, 0);
   Filter.Adjust_l_b = true;
   Filter.ForceWidth = 150;

   int64_t Time;
   double gyro;
   while (std::cin >> Time >> gyro)
   {
      double g = to_rad(gyro);
      g *= GainFactor;
      g += GyroQuadraticCorrection * g*g;

      Filter.ProcessGyro(Time, g);

      double t = Filter.StateEstimate<&Particle::Theta>();
      double v = Filter.StateEstimate<&Particle::Velocity>();
      double f = Filter.StateEstimate<&Particle::Force>();
      double lb = Filter.StateEstimate<&Particle::l_b>();
      double energy = 0.5*v*v*lb - g*std::cos(t);

      double offset = Filter.StateEstimate<&Particle::GyroOffset>();

      double KE = 0.5*v*v*lb;
      double PE = -g*std::cos(t);

      double RKE = RopeKineticEnergy(ThisBell, t, v);
      double RPE = RopePotentialEnergy(ThisBell, t, v);

      std::cout << Time << ' ' << to_deg(t) << ' ' << to_deg(v) << ' ' << to_deg(g-offset) << ' ' << to_deg(f)
                << ' ' << (energy+RKE+RPE)
                << ' ' << to_deg(offset)
                << ' ' << lb
                << std::endl;
   }
}
