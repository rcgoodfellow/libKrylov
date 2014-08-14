#include "Kry.hxx"
#include "KryOps.hxx"
#include <cstdlib>
#include <iostream>
#include <chrono>

using namespace kry;
using namespace std;
using namespace std::chrono;

int main()
{
  KryEngine::TStart(8);

  cout << "Go:" << endl;
  size_t M{20000};

  vector<KVec> vecs;
  vector<KScalar> dots;

  size_t N{90000};

  for(size_t i=0; i<N; ++i) 
  {
    vecs.push_back(KVec::Fill(M, 3));
  }
  
  auto start = high_resolution_clock::now();
  
  for(size_t i=0; i<N/2; ++i) 
  {
    dots.push_back(vecs[i*2] * vecs[i*2+1]); 
  }

  double sum{0};
  for(size_t i=0; i<N/2; ++i)
  {
    dots[i].obj->mtx.lock();
    sum += dots[i](); 
    dots[i].obj->mtx.unlock();
  }

  auto stop = high_resolution_clock::now();
  auto dt = duration_cast<microseconds>(stop-start);

  cout << "result: " << sum << " in: " << dt.count()/1000.0 << " ms" << endl;
}
