#include "kry_math.h"

void rmat(size_t M, size_t N, double *A)
{
  for(size_t i=0; i<M*N; ++i) { A[i] = (rand() % 100) / 10.0; }
}

double tvvm(size_t M, size_t N, double *a, double *b)
{
  double s = 0;
  for(size_t i=0; i<M; ++i) 
  {
    s += a[i*N] * b[i]; 
  }
  return s;
}


void c_vxv(void *a)
{
  vxv_args *_a = (vxv_args*)a;
  vxv(_a->M, _a->Na, _a->Nb, _a->a, _a->b, _a->axb);
}

void vxv(size_t M, size_t Na, size_t Nb, double *a, double *b, double *axb)
{
  double d = 0;
  for(size_t i=0; i<M; ++i)
  {
    d += a[i*Na] * b[i*Nb];
  }
  *axb = d;
}

void vxs(size_t M, size_t Na, size_t Nas, double *a, double s, double *as)
{
  for(size_t i=0; i<M; ++i)
  {
    as[i*Nas] = a[i*Na] * s;
  }
}

void c_vxs(void *a)
{
  vxs_args *_a = (vxs_args*)a; 
  vxs(_a->M, _a->Na, _a->Nb, _a->a, _a->s, _a->as);
}

void tmvm(size_t M, size_t N, double *A, double *x, double *Ax)
{
  for(size_t i=0; i<N; ++i)
  {
    Ax[i] = tvvm(M, N, &A[i], x);
  }
}
