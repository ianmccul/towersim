// -*- C++ -*-

// A particle filter for processing gyro readings

#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include "common/randutil.h"
#include "common/rungekutta.h"
#include <tuple>
#include <fstream>
#include <algorithm>
#include <string>
#include <cmath>
#include <deque>
#include "bells.h"
#include "common/trace.h"

timeval Epoch;

constexpr double pi = M_PI;
constexpr double g = 9.80665;
constexpr double NumParticles = 2000;

double to_deg(double r)
{
   return r*180.0/pi;
}

double to_rad(double d)
{
   return d*pi/180.0;
}

double RopeKineticEnergy(double l_r, double Gamma, double Theta, double v)
{
   // double l_r = b.WheelRadius * to_rad(b.ThetaR);

   double KE = l_r*v*v/2;
   if (Theta+Gamma > -pi/2 && Theta+Gamma < 0)
      KE *= std::pow((4/pi)*(Theta+Gamma) + 1, 2);
   return KE;
}

double RopeKineticEnergy(BellInfoType const& b, double Theta, double v)
{
   return RopeKineticEnergy(b.WheelRadius * to_rad(b.ThetaR), to_rad(b.Gamma), Theta, v);
}

// vertical height of the rope in angular units (ie, units of the wheel radius)
double RopeHeight(double Gamma, double Theta)
{
   double y = 0;
   if (Theta+Gamma < -pi/2)
   {
      y = -pi/2 - (Theta+Gamma) + 1;
   }
   else if (Theta+Gamma > 0)
   {
      y = 1 + (Theta+Gamma);
   }
   else
   {
      y = (2/pi)*std::pow(Theta+Gamma,2) + (Theta+Gamma) + 1;
   }
   return y;
}

// this is d y / d theta, the change in height corresponding to a change in
// bell angle.
double RopeToBell(double Gamma, double Theta)
{
   if (Theta+Gamma < -pi/2)
   {
      return -1;
   }
   if (Theta+Gamma > 0)
   {
      return 1;
   }
   // cutoff so that we don't give a zero
   double x = (4/pi)*(Theta+Gamma+pi/4);
   constexpr double cutoff = 0.1*pi/180;
   if (abs(x) < cutoff)
   {
      if (x < 0)
         x = -cutoff;
      else x = cutoff;
   }
   return x;
}

// vertical velocity of the rope in angular units (ie, units of the wheel radius)
double RopeVelocity(double Gamma, double Theta, double Velocity)
{
   double v = 0;
   if (Theta+Gamma < -pi/2)
   {
      return -Velocity;
   }
   else if (Theta+Gamma > 0)
   {
      return Velocity;
   }
   // else
   return Velocity * (1 + (4/pi)*(Theta+Gamma));
}

double RopePotentialEnergy(double ThetaR, double Gamma, double Theta, double v)
{
   return g * ThetaR * RopeHeight(Gamma, Theta);
}

double RopePotentialEnergy(BellInfoType const& b, double Theta, double v)
{
   return RopePotentialEnergy(to_rad(b.ThetaR), to_rad(b.Gamma), Theta, v);
}

double NormalPDF(double mean, double stdev, double x)
{
   return (1.0 / std::sqrt(2*pi*stdev*stdev)) * exp(-(x-mean)*(x-mean)/(2*stdev*stdev));
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
   double Weight;

   double Theta;
   double Velocity;
   double StayDeflection;
   double StayVelocity;
   double Force;

   // sensor parameters
   double GyroOffset;

   // bell parameters
   double ThetaR;
   double WheelRadius;
   double l_b;
   double k_b;
   double Gamma;
   double HandstrokeStay;
   double BackstrokeStay;
   double Y;
   double k_s;

   double ForceOffset;
   double ForceWidth;

   double l_r() const { return WheelRadius * ThetaR; }

   Particle(BellInfoType const& b, double t, double v, double f, double w = 1, double go = 0);

   // Some of the parameters are adjustable by the filter
   void Adjust_l_b();
   void Adjust_ThetaR();
   void Adjust_Friction();
   void Adjust_Youngs();

   // essential to do this before evolving
   void Adjust_Force();

   // get the derivative of the velocity, given theta and velocity and all other paramters fixed
   double VelocityDot(double theta, double velocity);

   // get the derivative of theta, given theta and velocity and all other paramters fixed
   double ThetaDot(double theta, double velocity);

   // Evolve the trajectory forward by time=Timestep
   void Evolve(double Timestep);

   // Update the weight of the trajectory, given the observed velocity v with error bar Width
   void UpdateWeight(double v, double Width);
};

std::ostream& operator<<(std::ostream& out, Particle const& p)
{
   out << "Theta: " << p.Theta << " Velocity: " << p.Velocity << " Force: " << p.Force
       << " Weight: " << p.Weight;
}

Particle::Particle(BellInfoType const& b, double t, double v, double f, double w, double go)
   : Weight(w), Theta(t), Velocity(v), StayDeflection(0), StayVelocity(0), Force(f), GyroOffset(go),
     ThetaR(to_rad(b.ThetaR)), WheelRadius(b.WheelRadius), l_b(b.lb), k_b(b.kb), Gamma(to_rad(b.Gamma)),
     HandstrokeStay(b.HandstrokeStay), BackstrokeStay(b.BackstrokeStay),
     Y(b.Y), k_s(b.ks)
{
}

double
Particle::VelocityDot(double theta, double velocity)
{
   double VelocityDot = 0;

   if (theta+Gamma < -pi/2)
   {
      VelocityDot = -(g/(l_b+this->l_r()))*(sin(theta) - ThetaR);
   }
   else if (Theta+Gamma > 0)
   {
      VelocityDot = -(g/(l_b+this->l_r()))*(sin(theta) + ThetaR);
   }
   else // -pi/2 < Theta+Gamma < 0
   {
      double n = 1 + (4/pi) * (theta + Gamma);  // n interpolates between -1 and 1
      VelocityDot = (-this->l_r()*(4/pi)*n*std::pow(velocity,2) - g*(sin(theta) + n*ThetaR))
         / (l_b + this->l_r()*n*n);
   }

   // The stay
   // Force due to bell is -(g/(l_b+this->l_r()))*(sin(HandstrokeStay) + ThetaR)
   // at theta=HandstrokeStay, this is exactly balanced by the restoring force
   // (HandstrokeStayNoForce - HandstrokeStay) * Y
   // Adjust the raw positions of the stay to the Young's modulus
   double hs = (g/(l_b+this->l_r()))*(sin(HandstrokeStay) + ThetaR)/Y + HandstrokeStay;
   double bs = (g/(l_b+this->l_r()))*(sin(BackstrokeStay) - ThetaR)/Y + BackstrokeStay;

   if (theta > hs)
   {
      // restoring force and friction from the stay
      VelocityDot -= (Theta - hs) * Y + velocity*k_s;
   }
   else
   {
      if (theta < bs)
      {
         VelocityDot -= (-Theta + bs) * Y + velocity*k_s;
      }
   }

   return VelocityDot - k_b*velocity + Force*RopeToBell(Gamma,Theta);
}

double
Particle::ThetaDot(double theta, double velocity)
{
   return velocity;
}

void
Particle::Adjust_l_b()
{
   l_b += 0.0001*randutil::randn();
}

void
Particle::Adjust_ThetaR()
{
   ThetaR += to_rad(0.001)*randutil::randn();
}

void
Particle::Adjust_Friction()
{
   k_b += 0.0001*randutil::randn();
   if (k_b < 0)
      k_b = 0;
}

void
Particle::Adjust_Force()
{
   Force += randutil::randn() * 2*pi/180.0;
}

void
Particle::Adjust_Youngs()
{
   Y += randutil::randn() * 1;
}


void
Particle::Evolve(double Timestep)
{
   // gyro offset
   // this should principally be temperature dependent, at max 0.04 degrees per second per degree.
   // If the temperature is changing 1 degree in 10 minutes then this is a change of 0.04 in 10 minutes,
   // so a variance of (0.04)^2 / (10*60) per second
   // = 2.67e-6, so rate 0.0016 degrees per sqrt(t)

   GyroOffset += sqrt(Timestep) * (0.01 * pi / 180) * randutil::randn();

   // Put some noise on theta - if this is too small then we can't correct without
   // crazy forces.
   // Noise 0.011 dps / sqrt(Hz) translates into a random walk
   // standard deviation of 0.011 degrees per sqrt(seconds)
   // or variance of (0.011)^2 per second
   // Hence our noise should be 0.011 degrees
   // However this seems to be tricky to handle in practice, if the noise is too small
   // then it doesn't cope, but too big and we lose fidelity
   //   Theta += std::sqrt(Timestep) * 0.08 * (pi / 180.0) * randutil::randn();
   // Theta += std::sqrt(Timestep) * 1 * (pi / 180.0) * randutil::randn();

   // IDEA: adjust the force to compensate when we adjust theta.
   double ThetaAdjust = std::sqrt(Timestep) * 0.08 * (pi / 180.0) * randutil::randn();
   double OldForce = this->VelocityDot(Theta, Velocity);
   Theta += ThetaAdjust;
   Force = 0;
   double NewForce = this->VelocityDot(Theta, Velocity);

   // Adjust force such that NewForce + Force*RopeToBell(Gamma, Theta) = OldForce;
   Force = (OldForce - NewForce) / RopeToBell(Gamma,Theta);

   // adjust the Young's modulus
#if 1
   double HandstrokeStayNoForce = (g/(l_b+this->l_r()))*(sin(HandstrokeStay) + ThetaR)/Y + HandstrokeStay;
   double BackstrokeStayNoForce = (g/(l_b+this->l_r()))*(sin(BackstrokeStay) - ThetaR)/Y + BackstrokeStay;
   if (Theta > HandstrokeStayNoForce || Theta < BackstrokeStayNoForce)
   {
      this->Adjust_Youngs();
   }
#endif

   // evolve according to the equations of motion

   constexpr int N = 1;
   for (int i = 0; i < N; ++i)
   {
      RungeKutta(Theta, Velocity,
                 [this](double t, double v){return this->ThetaDot(t,v);},
                 [this](double t, double v){return this->VelocityDot(t,v);},
                 Timestep/N);
   }
}

void
Particle::UpdateWeight(double v, double vWidth)
{
   double RelWeight = NormalPDF(Velocity, vWidth, v-GyroOffset);

   // Default value for the force arbitarily chosen to be 200
   ForceOffset = 0;
   ForceWidth = 500*pi/180;
   // if the velocity is bigger than 300, then we expect that the force will be small
   if (std::abs(v) > (1000.0*pi/180.0))  // this doesn't seem to work, probably needs better calibration
   {
      ForceWidth = 50*pi/180;
   }

   RelWeight *= NormalPDF(Force, ForceWidth, ForceOffset);

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
   double Offset = 0;
   double EulerError = 0;
   for (auto const& p : Particles)
   {
      SumWeight += p.Weight;
      SumTheta += p.Theta * p.Weight;
      SumVelocity += p.Velocity * p.Weight;
      SumV2 += p.Velocity * p.Velocity * p.Weight;
      SumForce += p.Force * p.Weight;
   }
   return std::make_tuple(SumTheta/SumWeight, SumVelocity/SumWeight, SumForce/SumWeight,
                          std::sqrt(std::max(0.0, SumV2/SumWeight - std::pow(SumVelocity/SumWeight,2))));
}

template <double Particle::*Member>
double
StateEstimate(std::vector<Particle> const& Particles)
{
   double SumWeight = 1e-300;  // avoid division by zero
   double s = 0;
   for (auto const& p : Particles)
   {
      SumWeight += p.Weight;
      s += p.Weight * p.*Member;
   }
   return s / SumWeight;
}

double
StateEstimate_l_b(std::vector<Particle> const& Particles)
{
   double SumWeight = 1e-300;  // avoid division by zero
   double s = 0;
   for (auto const& p : Particles)
   {
      SumWeight += p.Weight;
      s += p.Weight * p.l_b;
   }
   return s / SumWeight;
}

double
StateEstimate_k_b(std::vector<Particle> const& Particles)
{
   double SumWeight = 1e-300;  // avoid division by zero
   double s = 0;
   for (auto const& p : Particles)
   {
      SumWeight += p.Weight;
      s += p.Weight * p.k_b;
   }
   return s / SumWeight;
}

double
StateEstimate_ThetaR(std::vector<Particle> const& Particles)
{
   double SumWeight = 1e-300;  // avoid division by zero
   double s = 0;
   for (auto const& p : Particles)
   {
      SumWeight += p.Weight;
      s += p.Weight * p.ThetaR;
   }
   return s / SumWeight;
}

double
StateEstimate_ForceOffset(std::vector<Particle> const& Particles)
{
   double SumWeight = 1e-300;  // avoid division by zero
   double s = 0;
   for (auto const& p : Particles)
   {
      SumWeight += p.Weight;
      s += p.Weight * p.ForceOffset;
   }
   return s / SumWeight;
}

std::vector<Particle> CreateRandomParticles(BellInfoType const& b, int N, double Velocity = 0, double Offset = 0)
{
   std::vector<Particle> Result;
   for (int i = 0; i < N; ++i)
   {
      Result.push_back(Particle(b, randutil::rand()*2*pi - pi,
                                //randutil::rand(to_rad(b.BackstrokeStay), to_rad(b.HandstrokeStay)),
                                Velocity + randutil::randn() * 1, 0.0, 1.0, Offset));
   }
   return Result;
}

std::vector<Particle> FilterState;
BellInfoType Bell;

uint64_t CurrentTime = 0;

void Initialize(BellInfoType const& b)
{
   for (int i = 0; i < NumParticles; ++i)
   {
      FilterState.push_back(Particle(b, 0, 0, 1, 0));
   }
   FilterState = CreateRandomParticles(b, NumParticles);
   Bell = b;
}

void Resample(std::vector<Particle>& State, int N)
{
   // TODO: Systematic resampling
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

bool Adjust_l_b = true;
//bool Adjust_l_b = false;

//bool Adjust_Friction = true;
bool Adjust_Friction = false;

//bool Adjust_ThetaR = true;
bool Adjust_ThetaR = false;

void Process(uint64_t Time, double gyro, double last_gyro)
{
  if (CurrentTime != 0)
   {
      double Timestep = (int64_t(Time) - int64_t(CurrentTime)) * 1e-6;
      if (Timestep < 0.5 && Timestep >= 0)
      {
         for (auto& p : FilterState)
         {
            if (Adjust_l_b)
               p.Adjust_l_b();
            if (Adjust_Friction)
               p.Adjust_Friction();
            if (Adjust_ThetaR)
               p.Adjust_ThetaR();

            p.Adjust_Force();

            p.Evolve(Timestep);
         }
      }
#if 0
      else if (Timestep >= 0.5)
      {
         // force a resample if we didn't get any data for > half a second
         std::cerr << "forced reset! " << Timestep << ' ' << Time << "\n";
         CurrentTime = Time;
         FilterState = CreateRandomParticles(Bell, FilterState.size(), gyro);
         return;
      }
#endif
   }
   CurrentTime = Time;

   GyroReadings.push_back(gyro);
   if (GyroReadings.size() > 20)
   {
      GyroReadings.pop_front();
   }

   double GyroMaxShift = 0;
   for (int i = 0; i < int(GyroReadings.size())-1; ++i)
   {
      GyroMaxShift = std::max(GyroMaxShift, std::abs(GyroReadings[i]-GyroReadings[i+1]));
   }

   double CumulativeWeight = 0;
   double CumulativeWeight2 = 0;
   double GyroWidth = std::max(0.5*pi/180, 2*GyroMaxShift);
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
   if (CumulativeWeight2 < 1e-100)
   {
      // catastrophic loss of accuracy, start again
      std::cerr << "Reset!\n";
      // update the bell parameters
      Bell.lb = StateEstimate<&Particle::l_b>(FilterState);
      Bell.kb = StateEstimate<&Particle::k_b>(FilterState);
      Bell.ThetaR = to_deg(StateEstimate<&Particle::ThetaR>(FilterState));
      TRACE(Bell);
      FilterState = CreateRandomParticles(Bell, FilterState.size(), gyro);
   }
   else if ((CumulativeWeight * CumulativeWeight) / CumulativeWeight2 < (FilterState.size()/2))
   {
      Resample(FilterState, FilterState.size());
   }

   double t, v, f, err;
   std::tie(t,v,f, err) = StateEstimate(FilterState);

   double energy = 0.5*v*v*Bell.lb - g*std::cos(t);

   double KE = 0.5*v*v*Bell.lb;
   double PE = -g*std::cos(t);

   double RKE = RopeKineticEnergy(Bell, t, v);
   double RPE = RopePotentialEnergy(Bell, t, v);

   double Lb = StateEstimate_l_b(FilterState);
   double Kb = StateEstimate_k_b(FilterState);
   double Tr = StateEstimate_ThetaR(FilterState);
   double GOffset = StateEstimate<&Particle::GyroOffset>(FilterState);

   double YY = StateEstimate<&Particle::Y>(FilterState);

   std::cout << Time << ' ' << to_deg(t) << ' ' << to_deg(v) << ' ' << to_deg(gyro) << ' ' << to_deg(f) << ' '
             << to_deg(err) << ' ' << to_deg(GyroWidth) << ' ' << (energy+RKE+RPE)
             << ' ' << (KE+PE) << ' ' << (RKE+RPE) << ' '
             << ' ' << Lb << ' ' << Kb << ' ' << to_deg(Tr) << ' ' << to_deg(GOffset)
             << ' ' << YY
      //             << ' ' << KE << ' ' << PE << ' ' << RKE << ' ' << RPE
             << std::endl;
}

double GainFactor = 1.0;
double GyroQuadraticCorrection = 0;

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
   Initialize(ThisBell);
   uint64_t Time;
   double gyro;
   double last_gyro = -1000;
   while (std::cin >> Time >> gyro)
   {
      gyro *= GainFactor;
      gyro += GyroQuadraticCorrection * gyro*gyro;
      if (last_gyro == -1000)
         last_gyro = gyro;
      Process(Time, gyro * pi / 180.0, last_gyro);
      last_gyro = gyro;
   }
}
