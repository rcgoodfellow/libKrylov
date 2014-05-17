#include "Vec.hxx"
#include <iostream>
#include <chrono>
#include "Topo.hxx"

using namespace std::chrono;
using namespace kry;

int main()
{
  System s;
  s.node(0, 7);
  s.node(1, -6);
  s.node(2, -3);
  s.node(3, -2);
  s.node(4, 4);

  s.connect(0,1, 3);
  s.connect(1,2, 1);
  s.connect(1,3, 3);
  s.connect(2,3, 4);
  s.connect(3,4, 3);

  Arnoldi adi{&s, {0,0,0,0,0}};
  adi.go();
}
