#include "KryMath.hxx"

using namespace kry;
using std::runtime_error;
using std::lock_guard;
using std::mutex;

void kry::dot_vv(KVec a, KVec b, KScalar ab)
{
  if(a.M != b.M) { throw runtime_error("nonconformal dot_vv"); }
  size_t M = a.M;

  lock_guard<mutex> la{a.obj->mtx};
  lock_guard<mutex> lb{b.obj->mtx};

  double dp{0};
  double *_a = a.obj->data, *_b = b.obj->data;

  for(size_t i=0; i<M; ++i) { dp += _a[i] * _b[i]; }

  *ab.obj->data = dp;
  ab.obj->signalReady();
};

void kry::mul_vs(KVec a, KScalar b, KVec ab)
{
  lock_guard<mutex> la{a.obj->mtx}, lb{b.obj->mtx};

  double *_a = a.obj->data, *_b = b.obj->data, *_ab = ab.obj->data;

  for(size_t i=0; i<a.M; ++i) { _ab[i] = _a[i] * _b[i]; }

  ab.obj->signalReady();
}

double kry::dot_sv_v(KSMatrix &A, size_t row, double *x)
{
  double dot{0};
  double *r = &A.v[row*A.n];
  size_t *c = &A.c[row*A.n];
  for(size_t i=0; i<A.r[row]; ++i){ dot += r[i] * x[c[i]]; }
  return dot;
}

double kry::dot_v_v(size_t N, double *a, double *b)
{
  double dot{0};
  for(size_t i=0; i<N; ++i){ dot += a[i] * b[i]; }
  return dot;
}

double kry::dot_v_v(size_t N, double *a, size_t sa, double *b, size_t sb)
{
  double dot{0};
  for(size_t i=0; i<N; ++i) { dot += a[i*sa] * b[i*sb]; }
  return dot;
}

double kry::norm_v(size_t N, double *a)
{
  return sqrt(dot_v_v(N, a, a));
}

