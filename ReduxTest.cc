#include <iostream>
#include <cilk/cilk.h>
#include <mm_malloc.h>
#include <chrono>
#include <thread>

using namespace std::chrono;

inline void 
serialRedux(double *data, size_t sz, double &result)
{
  for(size_t i=0; i<sz; ++i){ result += data[i]; }
}

inline void
parallelRedux(double *data, size_t sz, size_t tc, double &result)
{
  size_t stride = sz/tc;
  double *results = (double*)_mm_malloc(sizeof(double)*tc, 64);
  std::thread *thds = new std::thread[tc];
  for(size_t i=0; i<tc; ++i)
  {    
    thds[i] = std::thread(serialRedux, data + i*stride, stride, std::ref(results[i]));
  }
  for(size_t i=0; i<tc; ++i){ thds[i].join(); }
  serialRedux(results, tc, result);
}

int main()
{
  size_t sz{100000};

  double sr{0}, pr{0};
  double *vec = (double*)_mm_malloc(sizeof(double) * sz, 64);
  vec[0:sz] = 0.003;

  auto s = high_resolution_clock::now();
  serialRedux(vec, sz, sr);
  auto f = high_resolution_clock::now();
  auto d = f - s;
  std::cout << "[serial] " << duration_cast<microseconds>(d).count() << "us" << std::endl; 
  std::cout << "result: " << sr << std::endl;
  
  s = high_resolution_clock::now();
  parallelRedux(vec, sz, 4, pr);
  f = high_resolution_clock::now();
  d = f - s;
  std::cout << "[parallel] " << duration_cast<microseconds>(d).count() << "us" << std::endl; 
  std::cout << "result: " << pr << std::endl;

}
