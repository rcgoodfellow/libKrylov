#ifndef KRY_MATH_HXX
#define KRY_MATH_HXX

#include "Kry.hxx"
#include <stdexcept>
#include <ctime>

namespace kry {

//Vector-Vector dot product
void dot_vv(KVec a, KVec b, KScalar ab);
void mul_vs(KVec a, KScalar b, KVec ab);


double dot_sv_v(KSMatrix &A, size_t row, double *x);
double dot_v_v(size_t N, double *a, double *b);
double dot_v_v(size_t N, double *a, size_t sa, double *b, size_t sb);
double norm_v(size_t N, double *a);


}

#endif
