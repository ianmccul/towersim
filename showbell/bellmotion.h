// -*- C++ -*- $Id$

#if !defined(BELLMOTION_H_JSDHCJEHYUIWHY435Y23984)
#define BELLMOTION_H_JSDHCJEHYUIWHY435Y23984

#include "common/rungekutta.h"
#include "common/math_const.h"
#include <cmath>

// acceleration due to gravity, SI units
double const g = 9.80665;

// preferred maximum time step for simulating evolution, in seconds
double const max_tstep = 0.001;

struct BellParameters
{
   double lb;       // effective length of the bell = I_b / M b

   double lc;       // effective length of the clapper = I_c / m c
   double alpha;    // angle of the clapper axis from bottom dead centre
   double a;        // distance from the bell axis to the clapper axis

   double phi_min;  // minimum angle where the clapper strikes the bell at backstroke
   double phi_max;  // maximum angle where the clapper strikes the bell at handstroke

   double garter;   // angle of the garter hole

   double kb;       // bell friction / M b
   double kc;       // clapper friction / m c

   double w0() const
   {
      return std::sqrt(g / lb);
   }

   void set_w0(double w0)
   {
      lb = w0 * w0 / g;
   }

   BellParameters();

   BellParameters(double lb_, double lc_, double alpha_=0, double a_=0,
		  double phi_min_=-0.2, double phi_max_ = 0.2,
		  double garter_ = -math_const::pi*0.75);
};

struct BellStayParameters : public BellParameters
{
   double stay_min;  // minimum angle of the stay ( < -pi, at handstroke)
   double stay_max;  // maximum angle of the stay ( > pi, at backstroke)

   double cs;        // coefficient of restitution for bouncing off the stay.  Outgoing velocity = cs * incoming

   BellStayParameters(double lb_, double lc_, double alpha_=0, double a_=0,
                      double phi_min_=-0.2, double phi_max_ = 0.2, double garter_ = -math_const::pi*0.75,
                      double stay_min_ = -math_const::pi - 0.4,
                      double stay_max_ = math_const::pi + 0.4, double cs_ = 0.9)
      : BellParameters(lb_, lc_, alpha_, a_, phi_min_, phi_max_, garter_),
        stay_min(stay_min_),
        stay_max(stay_max_),
        cs(cs_)
   {
   }
};

struct BellPosition
{
   double t;       // time (arbitrary starting point)

   double theta;   // angle of the bell from bottom dead centre
   double v_theta; // angular velocity of the bell

   double phi;     // angle of the clapper from the bell centre
   double v_phi;   // angular velocity of the clapper

   // flags to indicate that the clapper is resting against the bell at handstroke/backstroke
   bool ClapperHand;
   bool ClapperBack;

   // flags to indicate that the clapper has just struck the bell at handstroke/backstroke
   bool StrikeHand;
   bool StrikeBack;

   bool HasBellSounded() const { return StrikeHand | StrikeBack; }

   BellPosition();
};

BellPosition::BellPosition()
   : t(0),
     theta(0),
     v_theta(0),
     phi(0),
     v_phi(0),
     ClapperHand(false),
     ClapperBack(false),
     StrikeHand(false),
     StrikeBack(false)
{
}

// BellStayPosition adds a slider
struct BellStayPosition : public BellPosition
{
   double slider;  // position of the slider; between [stay_min+pi, stay_max-pi]

   BellStayPosition() : BellPosition(), slider(0) {}
};

// Updates the position of the bell given the new theta, velocity and acceleration
// at the advanced time t, and simulate the clapper motion
void UpdatePositionFromBell(BellPosition& Bell, BellParameters const& Parameters,
			    double theta_new, double v_theta_new, double a_theta_new,
			    double t);

// Updates the bell, clapper and slider positions, with the given external force.
// The force is in units of Mb (mass of bell * distance from axis to C.G.)
// meaning that the units of the 'force' is seconds^{-2}
void UpdateBellPosition(BellPosition& Bell, BellParameters const& Parameters,
			double t, double Force = 0);

// Updates the bell, including the slider positions.
void UpdateBellPosition(BellStayPosition& Bell, BellStayParameters const& Parameters,
			double t, double Force = 0);

// snaps the clapper positions to the limits specified by the parameters
void ClampClapperLimits(BellPosition& Bell, BellParameters const& Parameters);

double ReturnSecond(double, double y, double)
{
   return y;
}

struct v_phi_dot
{
   v_phi_dot(BellParameters const& Param, double Theta, double vTheta, double aTheta)
      : a(Param.a),
        lb(Param.lb),
        lc(Param.lc),
        alpha(Param.alpha),
        kc(Param.kc),
        theta(Theta),
        v_theta(vTheta),
        a_theta(aTheta - (g/Param.lb) * sin(Theta))
   {
   }

   v_phi_dot(BellParameters const& Param, double Theta, double vTheta)
      : a(Param.a),
        lb(Param.lb),
        lc(Param.lc),
        alpha(Param.alpha),
        kc(Param.kc),
        theta(Theta),
        v_theta(vTheta),
        a_theta((-g/Param.lb) * sin(Theta))
   {
   }

   double operator()(double phi, double v_phi, double /* t */) const
   {
      return -(a/lc) * (a_theta*cos(phi-alpha) + v_theta*v_theta*sin(phi-alpha))
         - (g/lc)*sin(theta + phi)
         - kc * v_phi
         - a_theta;
   }

   double a, lb, lc, alpha, kc, theta, v_theta, a_theta;

};

void UpdatePositionFromBell(BellPosition& Bell, BellParameters const& Parameters,
			    double theta_new, double v_theta_new, double a_theta_new,
			    double t)
{
   double tstep = t - Bell.t;

   Bell.theta = theta_new;
   Bell.v_theta = v_theta_new;

   RungeKutta(Bell.phi, Bell.v_phi, Bell.t, ReturnSecond,
              v_phi_dot(Parameters, Bell.theta, Bell.v_theta, a_theta_new),
              tstep);

   if (isnan(Bell.phi))
   {
      Bell.phi = 0;
      Bell.v_phi = 0;
   }

   ClampClapperLimits(Bell, Parameters);
}

struct BellEvolution
{
   BellEvolution(BellParameters const& Param_, double Force_ = 0)
      : Param(Param_), Force(Force_) {}

   double f1(double theta, double v_theta, double phi, double v_phi, double t) const
   {
      return v_theta;
   }

   double f2(double theta, double v_theta, double phi, double v_phi, double t) const
   {
       return -(g / Param.lb) * sin(theta) - Param.kb * v_theta + Force;
   }

   double f3(double theta, double v_theta, double phi, double v_phi, double t) const
   {
      return v_phi;
   }

   double f4(double theta, double v_theta, double phi, double v_phi, double t) const
   {
      double a_theta = f2(theta, v_theta, phi, v_phi, t);
      double result =  -(Param.a/Param.lc) * (a_theta*cos(phi-Param.alpha) + v_theta*v_theta*sin(phi-Param.alpha))
         - (g/Param.lc)*sin(theta + phi)
         - Param.kc * v_phi
         - a_theta;
      return result;
   }

   BellParameters Param;
   double Force;
};

void UpdateBellPosition(BellPosition& Bell, BellParameters const& Parameters,
			double t, double Force)
{
   double tstep = t - Bell.t;
   int Steps = 1;

   if (tstep > max_tstep)
   {
      Steps = int(ceil(tstep / max_tstep));
      tstep = tstep / Steps;
   }

   for (int i = 0; i < Steps; ++i)
   {
      RungeKutta(Bell.theta, Bell.v_theta, Bell.phi, Bell.v_phi, Bell.t,
                 BellEvolution(Parameters, Force),
                 tstep);
   }
}


void UpdateBellPosition(BellStayPosition& Bell, BellStayParameters const& Parameters, double t, double Force)
{
   double tstep = t - Bell.t;
   int Steps = 1;

   if (tstep > max_tstep)
   {
      Steps = int(ceil(tstep / max_tstep));
      tstep = tstep / Steps;
   }

   for (int i = 0; i < Steps; ++i)
   {
      RungeKutta(Bell.theta, Bell.v_theta, Bell.phi, Bell.v_phi, Bell.t,
                 BellEvolution(Parameters, Force),
                 tstep);

      // see if we've rebounded off the stay
      if (Bell.theta > Parameters.stay_max)
      {
         Bell.theta = 2*Parameters.stay_max - Bell.theta;
         Bell.v_theta *= -Parameters.cs;
      }
      else if (Bell.theta < Parameters.stay_min)
      {
         Bell.theta = 2*Parameters.stay_min - Bell.theta;
         Bell.v_theta *= -Parameters.cs;
      }
   }

   ClampClapperLimits(Bell, Parameters);

}

void ClampClapperLimits(BellPosition& Bell, BellParameters const& Parameters)
{
   if (Bell.phi >= Parameters.phi_max)
   {
      Bell.phi = Parameters.phi_max;
      Bell.v_phi = 0.0;
      if (!Bell.ClapperHand)
         Bell.StrikeHand = true;
      else
         Bell.StrikeHand = false;
      Bell.ClapperHand = true;
   }
   else
   {
      Bell.ClapperHand = false;
      Bell.StrikeHand = false;
   }

   if (Bell.phi <= Parameters.phi_min)
   {
      Bell.phi = Parameters.phi_min;
      Bell.v_phi = 0.0;
      if (!Bell.ClapperBack)
      {
         Bell.StrikeBack = true;
      }
      else
      {
         Bell.StrikeBack = false;
         Bell.ClapperBack = true;
      }
   }
   else
   {
      Bell.ClapperBack = false;
      Bell.StrikeBack = false;
   }
}

BellParameters::BellParameters()
   : lb(0),
     lc(0),
     alpha(0),
     a(0),
     phi_min(0),
     phi_max(0),
     kb(0),
     kc(0)
{
}

BellParameters::BellParameters(double lb_, double lc_, double alpha_, double a_, double phi_min_, double phi_max_, double garter_)
   : lb(lb_),
     lc(lc_),
     alpha(alpha_),
     a(a_),
     phi_min(phi_min_),
     phi_max(phi_max_),
     garter(garter_)
{
}










#endif
