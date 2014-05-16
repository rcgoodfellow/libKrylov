#ifndef _KRYLOV_VEC_
#define _KRYLOV_VEC_

#include <cstddef>
#include <type_traits>
#include <initializer_list>
#include <cassert>
#include <iterator>
#include <cmath>
#include <mm_malloc.h>

namespace kry
{

template<size_t N>
class Vec
{
  public:
    Vec(std::initializer_list<double> d)
    {
      assert(d.size() == N);
      for(auto it = d.begin(); it != d.end(); ++it) 
        data[std::distance(it, d.begin())] = *it;
    }
    Vec() { }
    //double *data = (double*)_mm_malloc(sizeof(double)*N, 64);
    double data[N] __attribute__((aligned(64)));
    static constexpr size_t size{N};
    double & operator[](size_t i){ return data[i]; }
    double const & operator[](size_t i) const { return data[i]; }

};

template<size_t N>
double p_norm(const Vec<N> &v)
{
  double squares[N] __attribute__((aligned(64)));
  squares[0:v.size] = v.data[0:v.size] * v.data[0:v.size];

  double sum{0};
  for(size_t i=0; i<v.size; ++i) sum += squares[i];
  return sqrt(sum);
}

template<size_t N>
double s_norm(const Vec<N> &v)
{
  double sum{0};
  for(size_t i=0; i<v.size; ++i) sum += pow(v[i], 2);
  return sqrt(sum);
}

}

#endif
