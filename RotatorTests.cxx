#include "KryMath.hxx"
#include <iostream>
#include <iomanip>

using namespace kry;
using namespace std;

void rotate_vector()
{
  size_t N = 8;

  double *x = (double*)_mm_malloc(sizeof(double)*N, 64);
  random_v(N, x, 0, 100);

  cout << show_vec(N, x) << endl;

  size_t i=0, j=1;
  Rotator R(x[i], x[j], i, j);
  R.applyT(x);
  
  cout << show_vec(N, x) << endl;
}

void rotate_matrix()
{
  KMatrix A = KMatrix::Random(8, 8, 0, 100),
          Q = KMatrix::Identity(8,8),
          R = KMatrix{8,8};

  R.data[0:64] = A.data[0:64];
  KVector b = KVector::Random(8, 0, 100);

  rotator r;
  for(size_t i=0; i<A.N; ++i)
  {
    for(size_t j=i+1; j<A.M; ++j)
    {
      r = rotate_left(R, i, j);
      /*
      Q.data[8*i +i] = Q.data[8*j +j] = r.c;
      Q.data[8*i +j] = -r.s;
      Q.data[8*j +i] = r.s;
      */
      q_update(Q, r);
    }
  }

 
  cout << "A" << endl;
  cout << show_mat(A.M, A.N, A.data) << endl;

  cout << "Q_t" << endl;
  cout << show_mat(A.M, A.N, Q.data) << endl;
  
  transpose(Q);
  
  cout << "Q" << endl;
  cout << show_mat(A.M, A.N, Q.data) << endl;

  cout << "R" << endl;
  cout << show_mat(A.M, A.N, R.data) << endl;

  KMatrix AA = Q * R;

  cout << show_mat(AA.M, AA.N, AA.data) << endl;
}

int main()
{
  LOG(("Rotator Tests\n"));

  //rotate_vector();
  rotate_matrix();

}

