// -*- C++ -*-

// fully-adapted particle filter
//
// How to handle the gyro offset?  Do we use another filter to manage this?
// Partially adapted filter?  Can we include the gyro offset in the
// fully-adapted filter?
// For version one, we don't allow updating the gyro offset.

#if !defined(TOWERSIM_ADAPTEDFILTER_H)
#define TOWERSIM_ADAPTEDFILTER_H

#include "bells.h"
#include <boost/circular_buffer.hpp>
#include <cmath>

constexpr double pi = M_PI;
constexpr double g = 9.80665;

inline
double to_deg(double r)
{
   return r*180.0/pi;
}

inline
double to_rad(double d)
{
   return d*pi/180.0;
}

// kinetic energy of the rope, angular units
double RopeKineticEnergy(double l_r, double Gamma, double Theta, double v);
double RopeKineticEnergy(BellInfoType const& b, double Theta, double v);

// vertical height of the rope in angular units (ie, units of the wheel radius)
double RopeHeight(double Gamma, double Theta);

// this is d y / d theta, the change in height corresponding to a change in
// bell angle.
double RopeToBell(double Gamma, double Theta);

// version that has a cutoff to avoid dividing by something small
double RopeToBellCutoff(double Gamma, double Theta);

// vertical velocity of the rope in angular units (ie, units of the wheel radius)
double RopeVelocity(double Gamma, double Theta, double Velocity);

double RopePotentialEnergy(double ThetaR, double Gamma, double Theta, double v);

double RopePotentialEnergy(BellInfoType const& b, double Theta, double v);

// force from the stay, angular units
double ForceFromStay(BellInfoType const& b, double Theta, double Velocity);

struct Particle
{
   double Weight;

   double Theta;
   double Velocity;
   double Force;
   double ForceDot;  // rate of change of force

   // After evolving, the Force is split up into 4 components, the sum is equal to Force.
   // The dynamical force is from (g/l) sin theta.  The stay force is from the stay.
   // The rope force is from the rope itself (ie the weight of the rope
   // and the external force covers everything else (ie, the ringer, etc).
   double ForceDynamical;
   double ForceStay;
   double ForceRope;
   double ForceExternal;
   double ForceSmoothed;
   double ForceVar;

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

   double l_r() const { return WheelRadius * ThetaR; }

   // the mean and standard deviation of the expected force
   double ForceMean();
   double ForceWidth();

   // the mean and standard deviation of the expected Delta F
   double DeltaForceMean();
   double DeltaForceWidth();

   Particle(BellInfoType const& b, double t, double v, double f, double w = 1, double go = 0);

   // Some of the parameters are adjustable by the filter
   void Adjust_l_b();
   void Adjust_ThetaR();
   void Adjust_Friction();
   void Adjust_Youngs();

   // get the derivative of the velocity, given theta and velocity and all other paramters fixed
   double VelocityDot(double theta, double velocity);

   // get the derivative of theta, given theta and velocity and all other paramters fixed
   double ThetaDot(double theta, double velocity);

   // Evolve the trajectory forward by time=Timestep.  We don't need to explicitly
   // update the weight, this is now done implicitly
   void Evolve(double Timestep, double GyroVelocity, double GyroWidth, double ForceWidth);

   // Update the weights given a thera measurement from tilt sensing
   void UpdateWeightTheta(double theta, double width);
};

std::ostream& operator<<(std::ostream& out, Particle const& p);

class ParticleFilter
{
   public:
      ParticleFilter();

      void Initialize(BellInfoType const& b_, int N_, double Offset = 0);
      void Initialize(BellInfoType const& b_, int N_, double Offset, double gyro);
      void Initialize(BellInfoType const& b_, int N_, double Offset, double gyro, double theta);

      // Process a sample.
      // Returns true if a state reset was required.
      bool ProcessGyro(uint64_t Time, double gyro);

      bool ProcessAccel(uint64_t Time, double theta);

      template <double Particle::*Member>
      double
      StateEstimate() const;

      template <double Particle::*Member>
      double
      StateEstimateErr(double Mean) const;

      double ForceWidth;

      bool Adjust_l_b;

   private:
      void Reset(BellInfoType const& b_, int N_, double Offset = 0);
      void Reset(BellInfoType const& b_, int N_, double Offset, double gyro);
      void Reset(BellInfoType const& b_, int N_, double Offset, double gyro, double theta);

      void Resample();

      BellInfoType b;
      int N;

      // to estimate the variance of the gyro, we use the maximum error
      // of the last few samples, and the difference between the current and
      // last readings.
      static constexpr int GyroBufferSize = 20;
      double LastGyro;
      boost::circular_buffer<double> GyroError;

      int64_t CurrentTime;

      std::vector<Particle> OldState;
      std::vector<Particle> State;
};

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

template <double Particle::*Member>
double
StateEstimateErr(std::vector<Particle> const& Particles, double Mean)
{
   double SumWeight = 1e-300;  // avoid division by zero
   double s = 0;
   for (auto const& p : Particles)
   {
      SumWeight += p.Weight;
      s += p.Weight * std::pow(Mean - p.*Member, 2);
   }
   return std::sqrt(s / SumWeight);
}

template <double Particle::*Member>
double
ParticleFilter::StateEstimate() const
{
   return ::StateEstimate<Member>(State);
}

template <double Particle::*Member>
double
ParticleFilter::StateEstimateErr(double Mean) const
{
   return ::StateEstimateErr<Member>(State, Mean);
}

#endif
