// -*- C++ -*-
//
// Implementation of Runge-Kutta ODE solvers
//

// time-independent case

template <typename F>
void
RungeKutta(double& x, F f, double step)
{
   double k1 = step*f(x);
   double k2 = step*f(x+0.5*k1);
   double k3 = step*f(x+0.5*k2);
   double k4 = step*f(x+k3);
   x += (k1 + 2*k2 + 2*k3 + k4)/6.0;
}

template <typename F1, typename F2>
void
RungeKutta(double& x1, double& x2, F1 f1, F2 f2, double step)
{
   double k11 = step*f1(x1,x2);
   double k12 = step*f2(x1,x2);

   double x11 = x1+0.5*k11;
   double x12 = x2+0.5*k12;
   double k21 = step*f1(x11, x12);
   double k22 = step*f2(x11, x12);

   double x21 = x1+0.5*k21;
   double x22 = x2+0.5*k22;
   double k31 = step*f1(x21, x22);
   double k32 = step*f2(x21, x22);

   double x31 = x1+k31;
   double x32 = x2+k32;
   double k41 = step*f1(x31, x32);
   double k42 = step*f2(x31, x32);

   x1 += (k11 + 2*k21 + 2*k31 + k41)/6.0;
   x2 += (k12 + 2*k22 + 2*k32 + k42)/6.0;
}

// time-dependent case

template <typename F>
void
RungeKutta(double& x, double& t, F f, double step)
{
   double k1 = step*f(x,t);
   double k2 = step*f(x+0.5*k1, t + 0.5*step);
   double k3 = step*f(x+0.5*k2, t + 0.5*step);
   double k4 = step*f(x+k3, t + step);
   x += (k1 + 2*k2 + 2*k3 + k4)/6.0;
   t += step;
}

template <typename F1, typename F2>
void
RungeKutta(double& x1, double& x2, double& t, F1 f1, F2 f2, double step)
{
   double k11 = step*f1(x1,x2,t);
   double k12 = step*f2(x1,x2,t);

   double x11 = x1+0.5*k11;
   double x12 = x2+0.5*k12;
   double k21 = step*f1(x11, x12, t + 0.5*step);
   double k22 = step*f2(x11, x12, t + 0.5*step);

   double x21 = x1+0.5*k21;
   double x22 = x2+0.5*k22;
   double k31 = step*f1(x21, x22, t + 0.5*step);
   double k32 = step*f2(x21, x22, t + 0.5*step);

   double x31 = x1+k31;
   double x32 = x2+k32;
   double k41 = step*f1(x31, x32, t + step);
   double k42 = step*f2(x31, x32, t + step);

   x1 += (k11 + 2*k21 + 2*k31 + k41)/6.0;
   x2 += (k12 + 2*k22 + 2*k32 + k42)/6.0;
   t += step;
}

template <typename F1, typename F2, typename F3>
void
RungeKutta(double& x1, double& x2, double& x3, double& t, F1 f1, F2 f2, F3 f3, double step)
{
   double k11 = step*f1(x1,x2,x3,t);
   double k12 = step*f2(x1,x2,x3,t);
   double k13 = step*f3(x1,x2,x3,t);

   double x11 = x1+0.5*k11;
   double x12 = x2+0.5*k12;
   double x13 = x3+0.5*k13;
   double k21 = step*f1(x11, x12, x13, t + 0.5*step);
   double k22 = step*f2(x11, x12, x13, t + 0.5*step);
   double k23 = step*f3(x11, x12, x13, t + 0.5*step);

   double x21 = x1+0.5*k21;
   double x22 = x2+0.5*k22;
   double x23 = x3+0.5*k23;
   double k31 = step*f1(x21, x22, x23, t + 0.5*step);
   double k32 = step*f2(x21, x22, x23, t + 0.5*step);
   double k33 = step*f3(x21, x22, x23, t + 0.5*step);

   double x31 = x1+k31;
   double x32 = x2+k32;
   double x33 = x3+k33;
   double k41 = step*f1(x31, x32, x33, t + step);
   double k42 = step*f2(x31, x32, x33, t + step);
   double k43 = step*f3(x31, x32, x33, t + step);

   x1 += (k11 + 2*k21 + 2*k31 + k41)/6.0;
   x2 += (k12 + 2*k22 + 2*k32 + k42)/6.0;
   x3 += (k13 + 2*k23 + 2*k33 + k43)/6.0;
   t += step;
}

template <typename F1, typename F2, typename F3, typename F4>
void
RungeKutta(double& x1, double& x2, double& x3, double& x4, double& t, F1 f1, F2 f2, F3 f3, F4 f4, double step)
{
   double k11 = step*f1(x1,x2,x3,x4,t);
   double k12 = step*f2(x1,x2,x3,x4,t);
   double k13 = step*f3(x1,x2,x3,x4,t);
   double k14 = step*f4(x1,x2,x3,x4,t);

   double x11 = x1+0.5*k11;
   double x12 = x2+0.5*k12;
   double x13 = x3+0.5*k13;
   double x14 = x4+0.5*k14;
   double k21 = step*f1(x11, x12, x13, x14, t + 0.5*step);
   double k22 = step*f2(x11, x12, x13, x14, t + 0.5*step);
   double k23 = step*f3(x11, x12, x13, x14, t + 0.5*step);
   double k24 = step*f4(x11, x12, x13, x14, t + 0.5*step);

   double x21 = x1+0.5*k21;
   double x22 = x2+0.5*k22;
   double x23 = x3+0.5*k23;
   double x24 = x4+0.5*k24;
   double k31 = step*f1(x21, x22, x23, x24, t + 0.5*step);
   double k32 = step*f2(x21, x22, x23, x24, t + 0.5*step);
   double k33 = step*f3(x21, x22, x23, x24, t + 0.5*step);
   double k34 = step*f4(x21, x22, x23, x24, t + 0.5*step);

   double x31 = x1+k31;
   double x32 = x2+k32;
   double x33 = x3+k33;
   double x34 = x4+k34;
   double k41 = step*f1(x31, x32, x33, x34, t + step);
   double k42 = step*f2(x31, x32, x33, x34, t + step);
   double k43 = step*f3(x31, x32, x33, x34, t + step);
   double k44 = step*f4(x31, x32, x33, x34, t + step);

   x1 += (k11 + 2*k21 + 2*k31 + k41)/6.0;
   x2 += (k12 + 2*k22 + 2*k32 + k42)/6.0;
   x3 += (k13 + 2*k23 + 2*k33 + k43)/6.0;
   x4 += (k14 + 2*k24 + 2*k34 + k44)/6.0;
   t += step;
}

template <typename F>
void
RungeKutta(double& x1, double& x2, double& x3, double& x4, double& t, F f, double step)
{
   double k11 = step*f.f1(x1,x2,x3,x4,t);
   double k12 = step*f.f2(x1,x2,x3,x4,t);
   double k13 = step*f.f3(x1,x2,x3,x4,t);
   double k14 = step*f.f4(x1,x2,x3,x4,t);

   double x11 = x1+0.5*k11;
   double x12 = x2+0.5*k12;
   double x13 = x3+0.5*k13;
   double x14 = x4+0.5*k14;
   double k21 = step*f.f1(x11, x12, x13, x14, t + 0.5*step);
   double k22 = step*f.f2(x11, x12, x13, x14, t + 0.5*step);
   double k23 = step*f.f3(x11, x12, x13, x14, t + 0.5*step);
   double k24 = step*f.f4(x11, x12, x13, x14, t + 0.5*step);

   double x21 = x1+0.5*k21;
   double x22 = x2+0.5*k22;
   double x23 = x3+0.5*k23;
   double x24 = x4+0.5*k24;
   double k31 = step*f.f1(x21, x22, x23, x24, t + 0.5*step);
   double k32 = step*f.f2(x21, x22, x23, x24, t + 0.5*step);
   double k33 = step*f.f3(x21, x22, x23, x24, t + 0.5*step);
   double k34 = step*f.f4(x21, x22, x23, x24, t + 0.5*step);

   double x31 = x1+k31;
   double x32 = x2+k32;
   double x33 = x3+k33;
   double x34 = x4+k34;
   double k41 = step*f.f1(x31, x32, x33, x34, t + step);
   double k42 = step*f.f2(x31, x32, x33, x34, t + step);
   double k43 = step*f.f3(x31, x32, x33, x34, t + step);
   double k44 = step*f.f4(x31, x32, x33, x34, t + step);

   x1 += (k11 + 2*k21 + 2*k31 + k41)/6.0;
   x2 += (k12 + 2*k22 + 2*k32 + k42)/6.0;
   x3 += (k13 + 2*k23 + 2*k33 + k43)/6.0;
   x4 += (k14 + 2*k24 + 2*k34 + k44)/6.0;
   t += step;
}

