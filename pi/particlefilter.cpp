// -*- C++ -*-

// A particle filter for processing gyro readings

#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include "common/randutil.h"
#include <tuple>
#include <algorithm>
#include <string>
#include <deque>

timeval Epoch;

constexpr double pi = M_PI;

constexpr double g = 9.91;

constexpr double NumParticles = 10000;

// period of small oscillations 1.7
// = 2pi * sqrt(l/g).
// Hence (T/(2*pi))^2 * g = l_b
double l_b = 0.738;

double NormalPDF(double mean, double stdev, double x)
{
   return (1.0 / std::sqrt(2*pi*stdev*stdev)) * exp(-(x-mean)*(x-mean)/(2*stdev*stdev));
}

double to_deg(double r)
{
   return r*180.0/pi;
}

double
StochasticForce(double Theta, double Velocity, double Force)
{
   return Force + randutil::randn() * std::max(Force/10, 1*pi/180.0);
}

// returns the (relative) probability of the velocity v
// being consistent with the measured velocity gv
// We sample quickly enough that the actual change from one timestep
// to the next is very small; the expected deviation is then
// of the order of the difference in measurements
double MeasurementProbability(double v, double gv, double last_gv)
{
   double Width = std::min(std::abs(gv - last_gv), 2.0*pi/180);
   return  NormalPDF(gv, Width, v);
}

struct Particle
{
   double Theta;
   double Velocity;
   double Force;
   double Weight;

   void Evolve(double Timestep);

   void UpdateWeight(double v, double Width);
};

std::ostream& operator<<(std::ostream& out, Particle const& p)
{
   out << "Theta: " << p.Theta << " Velocity: " << p.Velocity << " Force: " << p.Force
       << " Weight: " << p.Weight;
}

void
Particle::Evolve(double Timestep)
{
   // update the force with a random component
   Force = StochasticForce(Theta, Velocity, Force);

   // Put some noise on theta
   Theta += std::sqrt(Timestep) * 1 * (pi / 180.0) * randutil::randn();

   // Derivatives
   double ThetaDot = Velocity;
   double VelocityDot = -(g/l_b)*sin(Theta) + Force;

   // evolve according to the equations of motion
   Theta += ThetaDot * Timestep;
   Velocity += VelocityDot * Timestep;
}

void
Particle::UpdateWeight(double v, double vWidth)
{
   double RelWeight = NormalPDF(Velocity, vWidth, v);
   Weight *= RelWeight;
   //   std::cout << Weight << std::endl;
}

std::tuple<double, double, double, double>
StateEstimate(std::vector<Particle> const& Particles)
{
   double SumWeight = 1e-300;  // avoid division by zero
   double SumTheta = 0;
   double SumForce = 0;
   double SumVelocity = 0;
   double SumV2 = 0;
   for (auto const& p : Particles)
   {
      SumWeight += p.Weight;
      SumTheta += p.Theta * p.Weight;
      SumVelocity += p.Velocity * p.Weight;
      SumV2 += p.Velocity * p.Velocity * p.Weight;
      SumForce += p.Force * p.Weight;
   }
   return std::make_tuple(SumTheta/SumWeight, SumVelocity/SumWeight, SumForce/SumWeight,
                          std::sqrt(SumV2/SumWeight - std::pow(SumVelocity/SumWeight,2)));
}

std::vector<Particle> CreateRandomParticles(int N, double Velocity = 0)
{
   std::vector<Particle> Result;
   for (int i = 0; i < N; ++i)
   {
      Result.push_back(Particle({2*pi*randutil::rand() - pi, Velocity + randutil::randn() * 5, 0.0, 1.0}));
   }
   return Result;
}

std::vector<Particle> FilterState;

uint64_t CurrentTime = 0;

void Initialize()
{
   FilterState = CreateRandomParticles(NumParticles);
}

void Resample(std::vector<Particle>& State, int N)
{
   //   std::cerr << "Resampling\n";
   double RunningCumulate = 0.0;
   std::vector<double> Cumulates;
   for (auto const& p : State)
   {
      RunningCumulate += p.Weight;
      Cumulates.push_back(RunningCumulate);
   }
   std::vector<Particle> NewState;
   for (unsigned i = 0; i < N; ++i)
   {
      double r = RunningCumulate * randutil::rand();
      int n = std::lower_bound(Cumulates.begin(), Cumulates.end(), r) - Cumulates.begin();
      //std::cout << "select " << r << ' ' << n << ' ' << Cumulates[n] << ' ' << Cumulates[std::max(n-1,0)] << '\n';
      NewState.push_back(State[n]);
      NewState.back().Weight = 1.0 / N;
   }
   std::swap(State, NewState);
}

std::deque<double> GyroReadings;

template <typename V>
double mean(V const& v)
{
   double sum = 0;
   for (double x : v)
   {
      sum += x;
   }
   return sum / v.size();
}

template <typename V>
double max(V const& v)
{
   double m = v[0];
   for (double x : v)
   {
      m = std::max(m, x);
   }
   return m;
}

template <typename V>
double min(V const& v)
{
   double m = v[0];
   for (double x : v)
   {
      m = std::min(m, x);
   }
   return m;
}

template <typename V>
double stdev(V const& v, double mean)
{
   if (v.size() <= 1)
      return 0;
   double sumsq = 0;
   for (double x : v)
   {
      sumsq += std::pow(x-mean, 2);
   }
   return std::sqrt(sumsq / (v.size()-1));
}

void Process(uint64_t Time, double gyro, double last_gyro)
{
  if (CurrentTime != 0)
   {
      double Timestep = (Time - CurrentTime) * 1e-6;
      if (Timestep < 0.5)
      {
         for (auto& p : FilterState)
         {
            p.Evolve(Timestep);
         }
      }
   }
   CurrentTime = Time;

   GyroReadings.push_back(gyro);
   if (GyroReadings.size() > 10)
   {
      GyroReadings.pop_front();
   }

   double CumulativeWeight = 0;
   double CumulativeWeight2 = 0;
   double GyroWidth = std::max(2.0*pi/180, (max(GyroReadings) - min(GyroReadings))/2.0);
   for (auto& p : FilterState)
   {
      //std::cout << p << std::endl;
      p.UpdateWeight(gyro, GyroWidth);
      CumulativeWeight += p.Weight;
      CumulativeWeight2 += p.Weight * p.Weight;
   }
   for (auto& p : FilterState)
   {
      p.Weight *= 1.0 / CumulativeWeight;
   }
   //   std::cerr << "Effective N: " << ((CumulativeWeight * CumulativeWeight) / CumulativeWeight2) << '\n';
   if (CumulativeWeight2 == 0)
   {
      // catastrophic loss of accuracy, start again
      std::cerr << "Reset!\n";
      FilterState = CreateRandomParticles(FilterState.size(), gyro);
   }
   else if ((CumulativeWeight * CumulativeWeight) / CumulativeWeight2 < (FilterState.size()/2))
   {
      Resample(FilterState, FilterState.size());
   }

   double t, v, f, err;
   std::tie(t,v,f, err) = StateEstimate(FilterState);

   double energy = 0.5*v*v - (g/l_b)*std::cos(t);

   std::cout << Time << ' ' << to_deg(t) << ' ' << to_deg(v) << ' ' << to_deg(f) << ' '
             << to_deg(err) << ' ' << to_deg(GyroWidth) << ' ' << energy << std::endl;
}

double GainFactor = 1.0;

int main(int argc, char** argv)
{
   if (argc >= 2)
   {
      GainFactor = std::stod(std::string(argv[1]));
      std::cerr << "Using GainFactor = " << GainFactor << '\n';
      if (argc >= 3)
      {
         l_b = std::stod(std::string(argv[2]));
         std::cerr << "Using l_b = " << l_b << '\n';
      }
   }
   randutil::seed();
   Initialize();
   uint64_t Time;
   double gyro;
   double last_gyro = -1000;
   while (std::cin >> Time >> gyro)
   {
      gyro *= GainFactor;
      if (last_gyro == -1000)
         last_gyro = gyro;
      Process(Time, gyro * pi / 180.0, last_gyro);
      last_gyro = gyro;
   }
}
