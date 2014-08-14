#ifndef KRY_MATH_H
#define KRY_MATH_H

#include <stdlib.h>

void rmat(size_t M, size_t N, double *A);
double tvvm(size_t M, size_t N, double *a, double *b);
void tmvm(size_t M, size_t N, double *A, double *x, double *Ax);

typedef struct vxv_args
{
  size_t M; size_t Na, Nb; double *a, *b, *axb;
}
vxv_args;
void c_vxv(void *arg);
void vxv(size_t M, size_t Na, size_t Nb, double *a, double *b, double *axb);

typedef struct vxs_args
{
  size_t M, Na, Nb; double *a, s, *as;
}
vxs_args;

void c_vxs(void *arg);
void vxs(size_t M, size_t Na, size_t Nb, double *a, double s, double *as);

#endif
