#include "Vec.hxx"
#include <iostream>
#include <chrono>

using namespace std::chrono;

int main()
{
  std::cout << "Vector Tests" << std::endl;
  

  kry::Vec<100000> v{}; 
  v.data[0:100000] = 1.4747;

  auto s = high_resolution_clock::now();
  double vn = p_norm(v);
  v /= vn;
  auto f = high_resolution_clock::now();
  auto d = f - s;
  std::cout 
    << "[SIMD]" 
    << duration_cast<microseconds>(d).count() << "us"
    << std::endl;
  std::cout 
    << "VNorm SIMD: " << vn 
    << " [ " << v[0] << ", " << v[1] << ", " << v[2] << "..."
    << std::endl;
  
  v.data[0:100000] = 1.4747;

  s = high_resolution_clock::now();
  vn = s_norm(v);
  v /= vn;
  f = high_resolution_clock::now();
  d = f - s;
  std::cout 
    << "[PSEQ]" 
    << duration_cast<microseconds>(d).count() << "us"
    << std::endl;
  std::cout 
    << "VNorm PSEQ: " << vn  
    << " [ " << v[0] << ", " << v[1] << ", " << v[2] << "..."
    << std::endl;


  return 0;
}
