// -*- C++ -*- $Id: posix_time_operators.h 966 2012-06-17 12:05:28Z ianmcc $

#if !defined(POSIX_TIME_OPERATORS_H_HUJH78970)
#define POSIX_TIME_OPERATORS_H_HUJH78970

namespace boost
{

namespace posix_time
{

inline
boost::posix_time::time_duration
operator*(boost::posix_time::time_duration const& t, int x)
{
   return t.operator*(x);
}

inline
boost::posix_time::time_duration
operator*(int x, boost::posix_time::time_duration const& t)
{
   return t.operator*(x);
}

inline
boost::posix_time::time_duration
operator*(boost::posix_time::time_duration const& t, double x)
{
   long micro = t.total_microseconds();
   return boost::posix_time::microseconds(long(micro*x));
}

inline
boost::posix_time::time_duration
operator*(double x, boost::posix_time::time_duration const& t)
{
   long micro = t.total_microseconds();
   return boost::posix_time::microseconds(long(micro*x));
}

inline
double
operator/(boost::posix_time::time_duration const& t1, boost::posix_time::time_duration const& t2)
{
   return double(t1.total_microseconds()) / double(t2.total_microseconds());
}

}

}

#endif
