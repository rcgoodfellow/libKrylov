#include "Kry.hxx"
#include "KryOps.hxx"
#include <cstdlib>
#include <iostream>

using namespace kry;
using namespace std;

int main() 
{
  KryEngine::TStart(2);

  cout << "Creating Random Vectors" << endl;
  size_t M = 10240;
  KVec a = KVec::Fill(M, 1),
       b = KVec::Fill(M, 2),
       c = KVec::Fill(M, 3),
       d = KVec::Fill(M, 4);


  //These operations are being _enqueued_ to the Krylov engine through operator
  //overloading. The next two lines execute instantly as a, b, and c are already
  //_materialized_ and ready to go. These two operations will also likely
  //execute in parallel. The thrid line has to wait for the scalars e and f to
  //become materialized to execute.

  KScalar e = a * b; 
  KScalar f = b * c;
  KScalar g = (e*c)*(d*f);

  //causes block until g is avail
  cout << "g:" << g() << endl;

  return EXIT_SUCCESS;
}

