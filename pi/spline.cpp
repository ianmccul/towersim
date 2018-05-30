// -*- C++ -*-

#include "spline.h"
#include <Eigen/Dense>

CubicSpline::CubicSpline(double x0_, double x1_, int N_)
   : x0(x0_), x1(x1_), N(N_), a(N+5)
{
}

void
CubicSpline::LoadData(std::vector<std::pair<double, double>> Data_)
{
   Data = std::move(Data_);
}

void CubicSpline::Fit()
{
   Eigen::MatrixXf M = Eigen::MatrixXf::Zero(Data.size(), N+5);
   Eigen::VectorXf y(Data.size());

   // Assemble the matrix and right-hand side vector
   for (unsigned n = 0; n < Data.size(); ++n)
   {
      y[n] = Data[n].second;
      double t = N*(Data[n].first-x0) / (x1-x0);
      int i = static_cast<int>(std::floor(t));
      double tt = t-i;
      M(n, i) = this->BasisFunction3(tt);
      M(n, i+1) = this->BasisFunction2(tt);
      M(n, i+2) = this->BasisFunction1(tt);
      M(n, i+3) = this->BasisFunction0(tt);
   }

   // solve
   Eigen::VectorXf aa(N+5);
   aa = M.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(y);

   // copy eigen vector back into std::vector
   a = std::vector<double>(aa.data(), aa.data() + aa.size());
}

double
CubicSpline::operator[](double x) const
{
   double t = N*(x-x0)/(x1-x0);
   double i = static_cast<int>(std::floor(t));
   double tt = t-i;
   return a[i] * this->BasisFunction3(tt)
      + a[i+1] * this->BasisFunction2(tt)
      + a[i+2] * this->BasisFunction1(tt)
      + a[i+3] * this->BasisFunction0(tt);
}

double
CubicSpline::deriv(double x) const
{
   double t = N*(x-x0)/(x1-x0);
   double i = static_cast<int>(std::floor(t));
   double tt = t-i;
   double Scale = N/(x1-x0);  // dt / dx
   return Scale *
      (a[i] * this->BasisFunctionDeriv3(tt)
       + a[i+1] * this->BasisFunctionDeriv2(tt)
       + a[i+2] * this->BasisFunctionDeriv1(tt)
       + a[i+3] * this->BasisFunctionDeriv0(tt));
}

double
CubicSpline::deriv2(double x) const
{
   // dy/dx = dy/dt dt/dx
   // d2y/dx2 = d2y/{dtdx} dx/dt + dy/dt d2t/dx2
   // but d2t/dx2 = 0
   // so we have d2y/dx2 = d2y/{dtdx} dx/dt = d2y/dt2 (dt/dx)^2
   double t = N*(x-x0)/(x1-x0);
   double i = static_cast<int>(std::floor(t));
   double tt = t-i;
   double Scale = N/(x1-x0);  // dt / dx
   return std::pow(Scale,2) *
      (a[i] * this->BasisFunctionSecondDeriv3(tt)
       + a[i+1] * this->BasisFunctionSecondDeriv2(tt)
       + a[i+2] * this->BasisFunctionSecondDeriv1(tt)
       + a[i+3] * this->BasisFunctionSecondDeriv0(tt));
}

