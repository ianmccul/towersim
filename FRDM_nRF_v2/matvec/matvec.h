
#if !defined(MATVEC_H)
#define MATVEC_H

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>

template <typename T>
class vector3
{
   public:
      vector3() {}

      template <typename U>
      explicit vector3(vector3<U> const& x) { data[0] = x[0]; data[1] = x[1]; data[2] = x[2]; }

      vector3(T init) { data[0] = init; data[1] = init; data[2] = init; }

      vector3(T x_, T y_, T z_) { data[0] = x_; data[1] = y_; data[2] = z_; }

      void fill(T x) { data[0] = x; data[1] = x; data[2] = x; }

      T operator[](int n) const { return data[n]; }
      T& operator[](int n) { return data[n]; }

      T x() const { return data[0]; }
      T& x() { return data[0]; }

      T y() const { return data[1]; }
      T& y() { return data[1]; }

      T z() const { return data[2]; }
      T& z() { return data[2]; }

   private:
      T data[3];
};

template <typename T>
inline
vector3<T> operator+(vector3<T> const& a, vector3<T> const& b)
{
   return vector3<T>(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

template <typename T>
inline
vector3<T> operator-(vector3<T> const& a, vector3<T> const& b)
{
   return vector3<T>(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

template <typename T, typename U>
inline
vector3<T> operator+=(vector3<T>& a, vector3<U> const& b)
{
   a[0] += b[0];
   a[1] += b[1];
   a[2] += b[2];
   return a;
}

template <typename T, typename U>
inline
vector3<T> operator-=(vector3<T>& a, vector3<U> const& b)
{
   a[0] -= b[0];
   a[1] -= b[1];
   a[2] -= b[2];
   return a;
}

template <typename T, typename U>
vector3<decltype(std::declval<T>()/std::declval<U>())>
operator*(vector3<T> const& x, U const& y);

template <typename T, typename U>
inline
vector3<decltype(std::declval<T>()/std::declval<U>())>
operator/(vector3<T> const& a, U x)
{
   return {a[0]/x, a[1]/x, a[2]/x};
}

template <typename T>
inline
vector3<T>
min(vector3<T> const& a, vector3<T> const& b)
{
   using std::min;
   return vector3<T>(min(a[0], b[0]), min(a[1], b[1]), min(a[2], b[2]));
}

template <typename T>
inline
vector3<T>
max(vector3<T> const& a, vector3<T> const& b)
{
   using std::min;
   return vector3<T>(max(a[0], b[0]), max(a[1], b[1]), max(a[2], b[2]));
}

template <typename T>
inline
T norm_inf(vector3<T> const& x)
{
   using std::max;
   using std::abs;
   return max(max(abs(x[0]), abs(x[1])), abs(x[2]));
}

template <typename T>
class matrix3
{
   public:
      matrix3() {}

      matrix3(T init);

      T operator()(int i, int j) const { return 0; }

      // construct a 3x3 identity matrix
      static matrix3<T> identity();

   private:

      T data[9];
};

template <typename T>
int eigenvectors_symmetric(matrix3<T> const& A, matrix3<T>& Q, vector3<T>& w);

#include "matvec.cc"
#include "eigen.cc"

#endif
