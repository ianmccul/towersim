// -*- C++ -*-
//
// ODE solver for a system of first-order ODE's
//
// Given a differential equation
// dx/dt = F(x,t)
// we want to solve for x, given the initial condition x(t=0) = x0.
//
// Generalizing this to a system of first order equations is straightforward
// by treating x and F as vectors. But for small numbers of equations it is
// more convenient to have explicit forms.
// We could template this over the scalar type if we wanted to solve complex
// equations.

#if !defined(RUNGEKUTTA_H)
#define RUNGEKUTTA_H

// Time-independent versions.  These take functions of the form
// dx/dt = f(x)
template <typename F>
void
RungeKutta(double& x, F f, double step);

template <typename F1, typename F2>
void
RungeKutta(double& x1, double& x2, F1 f1, F2 f2, double step);

template <typename F1, typename F2, typename F3, typename F4>
void
RungeKutta(double& x1, double& x2, double& x3, double& x4, F1 f1, F2 f2, F3 f3, F4 f4, double step);


// Time-dependent forms dx/dt = f(x,t)
template <typename F>
void
RungeKutta(double& x, double& t, F f, double step);

// Runge-Kutta for a 2-variable ODE (dx1/dt,dx2/dt) = (f1(x1,x2,t), f2(x1,x2,t))
template <typename F1, typename F2>
void
RungeKutta(double& x1, double& x2, double& t, F1 f1, F2 f2, double step);

// Runge-Kutta for a 3-variable ODE (dx1/dt,dx2/dt,dx3/dt) = (f1(x1,x2,t), f2(x1,x2,t), f3(x1,x2,x3,t))
template <typename F1, typename F2, typename F3>
void
RungeKutta(double& x1, double& x2, double& x3, double& t, F1 f1, F2 f2, F3 f3, double step);

// Runge-Kutta for a 4-variable ODE
template <typename F1, typename F2, typename F3, typename F4>
void
RungeKutta(double& x1, double& x2, double& x3, double& x4, double& t, F1 f1, F2 f2, F3 f3, F4 f4, double step);

// version that takes a functor with 4 member functions,
// f1, f2, f3, f4
template <typename F>
void
RungeKutta(double& x1, double& x2, double& x3, double& x4, double& t, F f, double step);

#include "rungekutta.tcc"

#endif
