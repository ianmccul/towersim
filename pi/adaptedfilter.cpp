// -*- C++ -*-

#include "adaptedfilter.h"

#include "common/randutil.h"
#include "common/rungekutta.h"
#include "common/trace.h"

constexpr double InitialForceVar = 10000;

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
   : Weight(w), Theta(t), Velocity(v), Force(f), ForceDot(0), GyroOffset(go),
     ForceDynamical(0), ForceStay(0), ForceRope(0), ForceExternal(0), ForceSmoothed(0),
     ForceVar(InitialForceVar),
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
Particle::Adjust_Youngs()
{
   Y += randutil::randn() * 0.1;
}

double
Particle::ForceMean()
{
   return ForceSmoothed/2;
}

double
Particle::ForceWidth()
{
   // once we incorporate the stay we could reduce this
   return std::max(std::abs(ForceSmoothed/2), to_rad(100));
}

double
Particle::DeltaForceMean()
{
   return -ForceExternal;
}

double
Particle::DeltaForceWidth()
{
   //   return std::max(50.0, std::abs(ForceExternal));
   return 100000;
   //   ForceVar = 0.999*ForceVar + (0.001)*100;
   //   return ForceVar;
   //   return 10000;
   //return std::abs(ForceExternal) + 10;
   //100.0;
}

void
Particle::Evolve(double Timestep, double GyroVelocity, double GyroWidth, double ForceWidth)
{
   // Random walk on the offset
   //   double OffsetAdj = sqrt(Timestep) * (0.01 * pi / 180) * randutil::randn();
   //   GyroOffset += OffsetAdj;

   // dynamical forces - we probably should solve this implicitly
   ForceDynamical = -(g/l_b) * std::sin(Theta) - Velocity*k_b;

   // Force required to match the measured velocity
   double ForceRequired = (GyroVelocity-GyroOffset - Velocity)/Timestep - ForceDynamical;

   // The force is a product of 3 gaussians.
   // The force itself has a mean of ForceMean() and stdev of ForceWidth().
   // The dF/dt has a mean of DeltaForceMean() and a stdev of DeltaForceWidth().
   // The force must also match the gyro reading, which has a mean of ForceRequired
   // and a stdev of GyroWidth/Timestep.

   double FMean = this->ForceMean();
   double FPrecision = std::pow(this->ForceWidth(), -2);
   double DeltaFMean = this->DeltaForceMean();
   double DeltaFPrecision = std::pow(this->DeltaForceWidth()*Timestep, -2);
   double ForceRequriredPrecision = std::pow(Timestep/GyroWidth, 2);

   //   DeltaFMean /= Timestep;
   //   DeltaFPrecision *= Timestep*Timestep;

   double P = FPrecision + DeltaFPrecision + ForceRequriredPrecision;

   double m = (FMean*FPrecision + (ForceExternal+DeltaFMean*Timestep)*DeltaFPrecision
               + ForceRequired*ForceRequriredPrecision) / P;

   // choose the force
   double NewExternalForce = m + randutil::randn() / std::sqrt(P);

#if 0
   std::cerr << "precisions: " << FPrecision << ' ' << DeltaFPrecision << ' ' << ForceRequriredPrecision << '\n';

   std::cerr << ForceExternal << ' ' << NewExternalForce << '\n';

    std::cerr << "required: " << ForceRequired << " mean: " << m << " stdev " << std::sqrt(1.0/P)
                << " chosen: " << NewExternalForce
              << " velocity : " << Velocity << " Gyro: " << (GyroVelocity - GyroOffset) << " Theta " << Theta
              << '\n';
#endif

   // evolve
   double NewVelocity = Velocity + (ForceDynamical+NewExternalForce)*Timestep;
   //double NewVelocity = Velocity + (ForceDynamical+ForceRequired)*Timestep;
   double NewTheta = Theta + 0.5*(NewVelocity+Velocity)*Timestep;

   //   std::cerr << Timestep << ' ' << to_deg(GyroVelocity) << ' ' << to_deg(Velocity) << ' ' << to_deg(NewVelocity) << ' ' << to_deg(Theta) << ' ' << to_deg(NewTheta) << '\n';

   ForceRope = 0;
   ForceStay = 0;

   // update dynamical state
   Theta = NewTheta;
   Velocity = NewVelocity;
   Force = NewExternalForce + ForceDynamical;
   ForceExternal = NewExternalForce;

   ForceSmoothed = 0.99*ForceSmoothed + 0.01*ForceExternal;
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
   Weight *= RelWeight;
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
                               gyro + 0.1*randutil::randn()*pi/180, 0.0, 1.0, Offset));
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
   double GyroWidth = std::max(0.15*pi/180, GyroErrorMax);
   //double GyroWidth = std::max(1.0*pi/180, GyroErrorMax);

   bool Reset = false;
   OldState = State;
   if (CurrentTime != 0)
   {
      // Do nothing if our samples are out of order - it is too late already.
      if (Time <= CurrentTime)
         return false;

      double Timestep = (int64_t(Time) - int64_t(CurrentTime)) * 1e-6;
      if (1) // (Timestep < 0.5)
      {
         for (auto& p :State)
         {
            if (Adjust_l_b)
               p.Adjust_l_b();

            p.Evolve(Timestep, gyro, GyroWidth, ForceWidth);
         }
      }
      else
      {
         // no data for half a second, either severe packet loss or the sensor went to sleep.
         // Doing nothing is probably a good option, although we could add some random noise to theta too
      }
   }

   CurrentTime = Time;

   double CumulativeWeight = 0;
   double CumulativeWeight2 = 0;
   for (auto& p : State)
   {
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
      double Theta = ::StateEstimate<&Particle::Theta>(State);
      double ThetaErr = ::StateEstimateErr<&Particle::Theta>(State, Theta);
      std::cerr << "Resample! " << Time
                << ' ' << CumulativeWeight << ' '
                << ' ' << ((CumulativeWeight * CumulativeWeight) / CumulativeWeight2)
                << ' ' << to_deg(ThetaErr)
                << '\n';
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
      double Theta = ::StateEstimate<&Particle::Theta>(State);
      double ThetaErr = ::StateEstimateErr<&Particle::Theta>(State, Theta);
      std::cerr << "Resample! " << Time
                << ' ' << CumulativeWeight << ' '
                << ' ' << ((CumulativeWeight * CumulativeWeight) / CumulativeWeight2)
                << ' ' << to_deg(ThetaErr)
                << '\n';
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
