#include "Arnoldi.hxx"
#include <iostream>

using namespace kry;
using namespace std;

int main()
{
  constexpr size_t N{5}, M{5}, max_row{4}, n{5};
  KSMatrix A{N, M, max_row};
  double x0[N] = {0,0,0,0,0};
  double b[N] = {0.47, 0.32, 0.34, 0.41, 0.28};

  Arnoldi arnoldi(N, n, A, x0, b);

  
  size_t r[N] = {2, 3, 3, 4, 2};
  size_t c[N*max_row] = {0, 1, 0, 0, 
                         0, 3, 2, 0,
                         3, 2, 1, 0,
                         2, 1, 4, 3, 
                         3, 4, 0, 0};

  double v[N*max_row] = {0.40, 0.24, 0.00, 0.00, 
                         0.74, 0.40, 0.30, 0.00, 
                         0.50, 0.90, 0.70, 0.00, 
                         0.50, 0.83, 0.70, 0.65, 
                         0.70, 0.70, 0.00, 0.00};

  A.r[0:N] = r[0:N]; 
  A.c[0:N*max_row] = c[0:N*max_row];
  A.v[0:N*max_row] = v[0:N*max_row];

  cout << "b" << endl;
  cout << show_vec(A.N, b) << endl;

  arnoldi.H.data[0:n*n] = 0;

  arnoldi.compute();

  cout << endl;

  LOG(("%s\n", "Q"));
  for(size_t i=0; i<n; ++i) 
  {
    for(size_t j=0; j<N; ++j) { cout << arnoldi.Q.data[i*N+j] << " "; }
    cout << endl;
  }
  cout << endl;

  LOG(("%s\n", "H"));
  for(size_t i=0; i<n; ++i)
  {
    for(size_t j=0; j<n; ++j){ cout << arnoldi.H.data[i*n+j] << " "; }
    cout << endl;
  }


}
