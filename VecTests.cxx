#include "Vec.hxx"
#include <iostream>
#include <chrono>

using namespace std::chrono;

int main()
{
  std::cout << "Vector Tests" << std::endl;
  

  kry::Vec<1000000> v{}; 
  v.data[0:1000000] = 1.4747;

  auto s = high_resolution_clock::now();
  double vn = p_norm(v);
  auto f = high_resolution_clock::now();
  auto d = f - s;
  std::cout 
    << "[SIMD]" 
    << duration_cast<microseconds>(d).count() << "us"
    << std::endl;
  std::cout 
    << "VNorm SIMD: " << vn 
    << std::endl;
  
  s = high_resolution_clock::now();
  vn = s_norm(v);
  f = high_resolution_clock::now();
  d = f - s;
  std::cout 
    << "[PSEQ]" 
    << duration_cast<microseconds>(d).count() << "us"
    << std::endl;
  std::cout 
    << "VNorm PSEQ: " << vn 
    << std::endl;


  return 0;
}
