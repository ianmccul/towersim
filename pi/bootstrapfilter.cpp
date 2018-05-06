// -*- C++ -*-

#include "bootstrapfilter.h"

#include "common/randutil.h"
#include "common/rungekutta.h"
#include "common/trace.h"

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
   return x;
}

// version that has a cutoff to avoid dividing by something small
double RopeToBellCutoff(double Gamma, double Theta)
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
   constexpr double cutoff = 0.9;
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

double ForceFromStay(BellInfoType const& b, double Theta, double Velocity)
{
   double hs = (g/(b.lb+b.WheelRadius*to_rad(b.ThetaR)))*(sin(to_rad(b.HandstrokeStay)) + to_rad(b.ThetaR))/b.Y
      + to_rad(b.HandstrokeStay);
   double bs = (g/(b.lb+b.WheelRadius*to_rad(b.ThetaR)))*(sin(to_rad(b.BackstrokeStay)) - to_rad(b.ThetaR))/b.Y
      + to_rad(b.BackstrokeStay);

   //std::cerr << hs << ' ' << bs << ' ' << Theta << '\n';

   if (Theta > hs)
   {
      return - ((Theta - hs) * b.Y + Velocity*b.ks);
   }
   else if (Theta < bs)
   {
      return - ((Theta - bs) * b.Y + Velocity*b.ks);
   }
   return 0;
}

double NormalPDF(double mean, double stdev, double x)
{
   return (1.0 / std::sqrt(2*pi*stdev*stdev)) * exp(-(x-mean)*(x-mean)/(2*stdev*stdev));
}



//
// Particle definitions
//

std::ostream& operator<<(std::ostream& out, Particle const& p)
{
   out << "Theta: " << p.Theta << " Velocity: " << p.Velocity << " Force: " << p.Force
       << " Weight: " << p.Weight;
}

Particle::Particle(BellInfoType const& b, double t, double v, double f, double w, double go)
   : Weight(w), Theta(t), Velocity(v), StayDeflection(0), StayVelocity(0), Force(f), GyroOffset(go),
     ThetaR(to_rad(b.ThetaR)), WheelRadius(b.WheelRadius), l_b(b.lb), k_b(b.kb), Gamma(to_rad(b.Gamma)),
     HandstrokeStay(to_rad(b.HandstrokeStay)), BackstrokeStay(to_rad(b.BackstrokeStay)),
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
   else if (theta+Gamma > 0)
   {
      VelocityDot = -(g/(l_b+this->l_r()))*(sin(theta) + ThetaR);
   }
   else // -pi/2 < theta+Gamma < 0
   {
      double n = 1 + (4/pi) * (theta + Gamma);  // n interpolates between -1 and 1
      VelocityDot = (-this->l_r()*(4/pi)*n*std::pow(velocity,2) - g*(sin(theta) + n*ThetaR))
         / (l_b + this->l_r()*n*n);
   }

#if 0
   // The stay
   // Force due to bell is -(g/(l_b+this->l_r()))*(sin(HandstrokeStay) + ThetaR)
   // at theta=HandstrokeStay, this is exactly balanced by the restoring force
   // (HandstrokeStayNoForce - HandstrokeStay) * Y
   // Adjust the raw positions of the stay to the Young's modulus
   double hs = (g/(l_b+this->l_r()))*(sin(HandstrokeStay) + ThetaR)/Y + HandstrokeStay;
   double bs = (g/(l_b+this->l_r()))*(sin(BackstrokeStay) - ThetaR)/Y + BackstrokeStay;

   if (theta > hs)
   {
      //std::cerr << "stay " << theta << ' ' << hs << ' ' << bs << ' ' << Y << ' ' << l_b << ' ' << this->l_r() << '\n';
      // restoring force and friction from the stay
      VelocityDot -= (theta - hs) * Y + velocity*k_s;
   }
   else
   {
      if (theta < bs)
      {
         VelocityDot -= (theta - bs) * Y + velocity*k_s;
      }
   }
#endif

   return VelocityDot - k_b*velocity + Force;
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
Particle::Adjust_Force(double v, double Timestep)
{
   //   double ExtraForce = (v-Velocity); // Timestep;
   //   if (std::abs(Force) > 50.0*pi/180)
   //      std::cerr << to_deg(ExtraForce) << ' ' << to_deg(Force) << ' ' << to_deg(v) << '\n';
   //      Force += ExtraForce*0.01 + std::max(std::abs(ExtraForce),0.5*pi/180.0)  * randutil::randn(); // * 2*pi/180.0;

   Force += 20 * Timestep * randutil::randn();
   //   Force += (2*pi/180.0) * randutil::randn();
}

void
Particle::Adjust_Youngs()
{
   Y += randutil::randn() * 0.1;
}


void
Particle::Evolve(double Timestep)
{
   // gyro offset
   // this should principally be temperature dependent, at max 0.04 degrees per second per degree.
   // If the temperature is changing 1 degree in 10 minutes then this is a change of 0.04 in 10 minutes,
   // so a variance of (0.04)^2 / (10*60) per second
   // = 2.67e-6, so rate 0.0016 degrees per sqrt(t)

   double OffsetAdj = sqrt(Timestep) * (0.01 * pi / 180) * randutil::randn();
   GyroOffset += OffsetAdj;
   Velocity -= OffsetAdj;

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

#if 0
   // IDEA: adjust the force to compensate when we adjust theta.
   double ThetaAdjust = std::sqrt(Timestep) * 0.1 * (pi / 180.0) * randutil::randn();
   double OldForce = this->VelocityDot(Theta, Velocity);
   Theta += ThetaAdjust;

   Force = 0;
   double NewForce = this->VelocityDot(Theta, Velocity);

   // Adjust force such that NewForce + Force*RopeToBell(Gamma, Theta) = OldForce;
   Force = (OldForce - NewForce); // / RopeToBell(Gamma,Theta);
#endif

   // adjust the Young's modulus
#if 0
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
Particle::UpdateWeight(double v, double vWidth, double fWidth)
{
   double RelWeight = NormalPDF(Velocity, vWidth, v-GyroOffset);

   Weight *= RelWeight;
   return;

   // Default value for the force arbitarily chosen to be 200
#if 0
   ForceOffset = 0;
   ForceWidth = 500*pi/180;
   // if the velocity is bigger than 300, then we expect that the force will be small
   if (std::abs(v) > (1000.0*pi/180.0))  // this doesn't seem to work, probably needs better calibration
   {
      ForceWidth = 50*pi/180;
   }
#endif

   RelWeight *= NormalPDF(Force, fWidth, 0);


      // cutoff rb at some value otherwise we get a divergence as the rope bounces
   double rb = RopeToBellCutoff(Gamma, Theta);
   double ForceOnRope = Force / rb;

   // weightings based on f*v^2
   RelWeight *= 1.0 / std::pow(std::max(std::abs(ForceOnRope * Velocity * Velocity), 1.0), 0.25);

   Weight *= RelWeight;
   //   std::cerr << Weight << std::endl;
}

void
Particle::UpdateWeightTheta(double theta, double ThetaWidth)
{
   // we only have theta moduo 2 pi here
   while (theta > Theta+pi)
      theta -= 2*pi;
   while (theta < Theta-pi)
      theta += 2*pi;
   double RelWeight = NormalPDF(Theta, ThetaWidth, theta);
   //   Weight *= RelWeight;
   Weight = RelWeight;
   //   std::cerr << Weight << std::endl;
}

double
StateEstimateError(std::vector<Particle> const& Particles)
{
   double SumWeight = 1e-300;  // avoid division by zero
   double SumVelocity = 0;
   double SumV2 = 0;
   for (auto const& p : Particles)
   {
      SumWeight += p.Weight;
      SumVelocity += p.Velocity * p.Weight;
      SumV2 += p.Velocity * p.Velocity * p.Weight;
   }
   return std::sqrt(std::max(0.0, SumV2/SumWeight - std::pow(SumVelocity/SumWeight,2)));
}

//
// ParticleFilter definitions
//

ParticleFilter::ParticleFilter()
   : ForceWidth(300), Adjust_l_b(false), N(0), CurrentTime(0)
{
}

void
ParticleFilter::Initialize(BellInfoType const& b_, int N_, double Offset)
{
   CurrentTime = 0;
   this->Reset(b_, N_, Offset);
}

void
ParticleFilter::Initialize(BellInfoType const& b_, int N_, double Offset, double gyro)
{
   CurrentTime = 0;
   this->Reset(b_, N_, Offset, gyro);
}

void
ParticleFilter::Initialize(BellInfoType const& b_, int N_, double Offset, double gyro, double theta)
{
   CurrentTime = 0;
   this->Reset(b_, N_, Offset, gyro, theta);
}

void
ParticleFilter::Reset(BellInfoType const& b_, int N_, double Offset)
{
   b = b_;
   N = N_;

   OldState.clear();
   State.clear();

   for (int i = 0; i < N; ++i)
   {
      State.push_back(Particle(b, randutil::rand(to_rad(b.BackstrokeStay), to_rad(b.HandstrokeStay)),
                               randutil::randn()*300*pi/180, 0.0, 1.0, Offset));
   }
}


void
ParticleFilter::Reset(BellInfoType const& b_, int N_, double Offset, double gyro)
{
   b = b_;
   N = N_;

   OldState.clear();
   State.clear();

   for (int i = 0; i < N; ++i)
   {
      State.push_back(Particle(b, randutil::rand(to_rad(b.BackstrokeStay), to_rad(b.HandstrokeStay)),
                               gyro + 1.0*randutil::randn()*pi/180, 0.0, 1.0, Offset));
   }
}

void
ParticleFilter::Reset(BellInfoType const& b_, int N_, double Offset, double gyro, double theta)
{
   b = b_;
   N = N_;

   OldState.clear();
   State.clear();

   for (int i = 0; i < N; ++i)
   {
      State.push_back(Particle(b, theta + 0.5*randutil::randn()*pi/180,
                               gyro + 0.5*randutil::randn()*pi/180, 0.0, 1.0, Offset));
   }
}

void
ParticleFilter::Resample()
{
   // TODO: Systematic resampling
   //   std::cerr << "Resampling\n";
#if 0
   double RunningCumulate = 0.0;
   std::vector<double> Cumulates;
   for (auto const& p : State)
   {
      RunningCumulate += p.Weight;
      Cumulates.push_back(RunningCumulate);
   }
   std::swap(OldState, State);
   State.clear();
   for (unsigned i = 0; i < N; ++i)
   {
      double r = RunningCumulate * randutil::rand();
      int n = std::lower_bound(Cumulates.begin(), Cumulates.end(), r) - Cumulates.begin();
      State.push_back(OldState[n]);
      State.back().Weight = 1.0 / N;
   }
#else
   // Stratified sampling
   double RunningCumulate = 0.0;
   std::vector<double> Cumulates;
   for (auto const& p : State)
   {
      RunningCumulate += p.Weight;
      Cumulates.push_back(RunningCumulate);
   }
   double TotalCumulate = RunningCumulate;
   std::swap(OldState, State);
   State.clear();
   int p = 0;
   RunningCumulate = Cumulates[0];
   for (unsigned i = 0; i < N; ++i)
   {
      double r = TotalCumulate * (i + randutil::rand()) / double(N);
      while (RunningCumulate < r)
      {
         ++p;
         RunningCumulate = Cumulates[p];
      }
      int n = std::lower_bound(Cumulates.begin(), Cumulates.end(), r) - Cumulates.begin();
      State.push_back(OldState[p]);
      State.back().Weight = 1.0 / N;
   }
#endif
}

bool
ParticleFilter::ProcessGyro(uint64_t Time, double gyro)
{
   bool Reset = false;
   OldState = State;
   if (CurrentTime != 0)
   {
      // Do nothing if our samples are out of order - it is too late already.
      if (Time < CurrentTime)
         return false;

      double Timestep = (int64_t(Time) - int64_t(CurrentTime)) * 1e-6;
      if (Timestep < 0.5)
      {
         for (auto& p :State)
         {
            if (Adjust_l_b)
               p.Adjust_l_b();

            p.Adjust_Force(LastGyro, Timestep);
            p.Evolve(Timestep);
         }
      }
      else
      {
         // no data for half a second, either severe packet loss or the sensor went to sleep.
         // Doing nothing is probably a good option, although we could add some random noise to theta too
      }
   }

   CurrentTime = Time;

   // get the max error from the gyro
   if (GyroError.size() == GyroBufferSize)
      GyroError.pop_front();

   GyroError.push_back(std::abs(gyro - LastGyro));
   LastGyro = gyro;

   double GyroErrorMax = 0;
   for (auto x : GyroError)
   {
      GyroErrorMax = std::max(GyroErrorMax, x);
   }

   double CumulativeWeight = 0;
   double CumulativeWeight2 = 0;
   double GyroWidth = std::max(0.5*pi/180, GyroErrorMax);
   for (auto& p : State)
   {
      p.UpdateWeight(gyro, GyroWidth, ForceWidth);
      CumulativeWeight += p.Weight;
      CumulativeWeight2 += p.Weight * p.Weight;
   }

   //std::cerr << CumulativeWeight << ' ' << CumulativeWeight2 << '\n';
   if (CumulativeWeight2 < 1e-100)
   {
      // catastrophic loss of accuracy, start again
      Reset = true;
      std::cerr << "Reset!\n";
      // update bell parameters based on last known-good state
      b.lb = ::StateEstimate<&Particle::l_b>(OldState);
      b.kb = ::StateEstimate<&Particle::k_b>(OldState);
      b.ThetaR = to_deg(::StateEstimate<&Particle::ThetaR>(OldState));
      double Offset = ::StateEstimate<&Particle::GyroOffset>(OldState);
      this->Reset(b, N, Offset, gyro-Offset);
   }
   else if ((CumulativeWeight * CumulativeWeight) / CumulativeWeight2 < (State.size()/2))
   {
      //std::cerr << "Resample! " << (CumulativeWeight) << ' ' << CumulativeWeight2 << "\n";
      this->Resample();
   }
   else
   {
      //std::cerr << "OK!\n";
      // normalize the weights
      for (auto& p : State)
      {
         p.Weight *= 1.0 / CumulativeWeight;
      }
   }
   return Reset;
}

bool
ParticleFilter::ProcessAccel(uint64_t Time, double theta)
{
   bool Reset = false;
   OldState = State;

   // using tilt sensing, theta is either going to be near zero, or
   // near the balance/stay side.

   if (theta > pi/2 && theta < pi)
      theta -= 2*pi;
   else if (theta < pi/2 && theta > -pi)
   {
      theta += 2*pi;
   }

   double CumulativeWeight = 0;
   double CumulativeWeight2 = 0;
   for (auto& p : State)
   {
      p.UpdateWeightTheta(theta, to_rad(0.3));
      CumulativeWeight += p.Weight;
      CumulativeWeight2 += p.Weight * p.Weight;
   }

   //std::cerr << CumulativeWeight << ' ' << CumulativeWeight2 << '\n';
   if (CumulativeWeight2 < 1e-100)
   {
      // catastrophic loss of accuracy, start again
      Reset = true;
      std::cerr << "Reset!\n";
      // update bell parameters based on last known-good state
      b.lb = ::StateEstimate<&Particle::l_b>(OldState);
      b.kb = ::StateEstimate<&Particle::k_b>(OldState);
      b.ThetaR = to_deg(::StateEstimate<&Particle::ThetaR>(OldState));
      double Offset = ::StateEstimate<&Particle::GyroOffset>(OldState);
      this->Reset(b, N, Offset, LastGyro-Offset, theta);
   }
   else if ((CumulativeWeight * CumulativeWeight) / CumulativeWeight2 < (State.size()/2))
   {
      //std::cerr << "Resample! " << (CumulativeWeight) << ' ' << CumulativeWeight2 << "\n";
      this->Resample();
   }
   else
   {
      //std::cerr << "OK!\n";
      // normalize the weights
      for (auto& p : State)
      {
         p.Weight *= 1.0 / CumulativeWeight;
      }
   }

   double t = ::StateEstimate<&Particle::Theta>(State);
   while (theta > t+pi)
      theta -= 2*pi;
   while (theta < t-pi)
      theta += 2*pi;
   double Offs = to_deg(::StateEstimate<&Particle::GyroOffset>(State));
   std::cerr << "Accel data " << to_deg(theta) << " filter " << to_deg(t) << " offset " << Offs << '\n';

   return Reset;
}
