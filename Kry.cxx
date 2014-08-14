#include "Kry.hxx"

using namespace kry;
using std::vector;
using std::random_device;
using std::uniform_real_distribution;
using std::default_random_engine;
using std::initializer_list;
using std::shared_ptr;
using std::thread;
using std::string;
using std::stringstream;
using std::setprecision;
using std::fixed;

//~=~ KTask ~=~ ---------------------------------------------------------------
KTask::KTask(F f) : f{f} {}
void KTask::operator()(){ f(); }

//~=~ KryEngine ~=~ -----------------------------------------------------------
KryEngine& KryEngine::get()
{
  static KryEngine instance;
  return instance;
}

void KryEngine::__kthread_go__()
{
  while(47 == 47)
  {
    KTask kt; 
    KryEngine::get().Q.Q.pop(kt);
    kt();
  }
}

void KryEngine::TStart(size_t num_threads)
{
  //Do something to start threads
  //KryEngine::get()
  for(size_t i=0; i<num_threads; ++i)
  {
    thread t{__kthread_go__};
    t.detach();
  }
}

void KryEngine::Enqueue(KTask t)
{
  KryEngine::get().Q.Q.push(t);
}

//~=~ KObject ~=~ -------------------------------------------------------------
KObject::KObject(size_t size)
  : 
    //state{State::Materializing}, 
    size{size},
    data{(double*)_mm_malloc(sizeof(double)*size, __malign__)}
{ 
  mtx.lock();
}

KObject::~KObject()
{
  if(data) {
    _mm_free(data);
    data = nullptr;
    state = State::Vapor;
  }
}

void KObject::signalReady()
{
  state = State::SolidState;
  cnd.notify_all();
  mtx.unlock();
}

//~=~ KSubspace ~=~ -----------------------------------------------------------
KSubspace::KSubspace(size_t N, size_t M)
  : N{N}, M{M},
    data{(double*)_mm_malloc(sizeof(double)*(M+1)*N, 64)} //M+1 because we always overshoot on subspace construction by one
{

}

double* KSubspace::operator()(size_t i)
{
  return &data[N*i];
}

//~=~ KMatrix ~=~ -------------------------------------------------------------
KMatrix::KMatrix(size_t N, size_t M)
  : N{N}, M{M},
    data{(double*)_mm_malloc(sizeof(double)*M*N, 64)}
{

}

KMatrix KMatrix::Random(size_t M, size_t N, double min, double max)
{
  KMatrix A{N, M};
  random_m(M, N, A.data, min, max);
  return A;
}

KMatrix KMatrix::Identity(size_t M, size_t N)
{
  KMatrix I{N, M};
  I.data[0:N*M] = 0;

  for(size_t i=0; i<M; ++i) { I.data[i*N + i] = 1; }

  return I;
}

KVector::KVector(size_t N)
  : N{N},
    data{(double*)_mm_malloc(sizeof(double)*N, 64)}
{ }

KVector KVector::Random(size_t N, double min, double max)
{
  KVector v{N};
  random_v(N, v.data, min, max);
  return v;
}

//~=~ KSMatrix ~=~ ------------------------------------------------------------
KSMatrix::KSMatrix(size_t M, size_t N, size_t n)
  : M{M}, N{N}, n{n},
    r{(size_t*)_mm_malloc(sizeof(size_t)*M, 64)},
    c{(size_t*)_mm_malloc(sizeof(size_t)*M*N, 64)},
    v{(double*)_mm_malloc(sizeof(size_t)*M*N, 64)}
{

}

//~=~ KVec ~=~ ----------------------------------------------------------------
KVec KVec::Random(size_t m)
{
  random_device rd;
  uniform_real_distribution<double> dst{0.5,3};
  default_random_engine re{rd()};

  KVec v(m);
  v.obj->data[0:m] = dst(re);
  v.obj->signalReady();

  return v;
}

KVec KVec::Fill(size_t m, double value)
{
  KVec v(m);

  KTask::F f = [=]()
  { 
    v.obj->data[0:m] = value;
    v.obj->signalReady();
  };
  KryEngine::Enqueue(KTask{f});

  return v;
}

KVec::KVec(size_t m) 
  : M{m},
    obj{new KObject{m}}
{ }

KVec::KVec(initializer_list<double> x)
  : KVec(x.size())
{
  size_t i=0;
  for(double d : x) { obj->data[i++] = d; }
}

double & KVec::operator()(size_t i)
{
  return obj->data[i];
}

//~=~ KScalar ~=~ -------------------------------------------------------------
KScalar::KScalar(double value) 
  : obj{new KObject(1)} 
{
  *obj->data = value;
}

double & KScalar::operator()()
{
  return *obj->data;
}

//~=~ Rotator ~=~ -------------------------------------------------------------
Rotator::Rotator(double xi, double xj, size_t i, size_t j)
  : i{i}, j{j}
{
  long double B = fmax(abs(xi), abs(xj));
  if(B == 0)
  {
    c = 1, s = 0;
  }
  else
  {
    long double _xi = xi/B, _xj = xj/B;
    long double v = sqrt(_xi*_xi + _xj*_xj);
    c = _xi/v, s = _xj/v;
  }

}

void rotator::apply_left(KVector &v)
{
  double *x = v.data;
  double xi = x[i], xj = x[j];
  x[i] =  xi*c + xj*s;
  x[j] = -xi*s + xj*c;
}

double* Rotator::applyT(double *x)
{
  double xi = x[i], xj = x[j];
  x[i] =  xi*c + xj*s;
  x[j] = -xi*s + xj*c;

  return x;
}

double* Rotator::apply(double *x)
{
  double xi = x[i], xj = x[j];
  x[i] = xi*c - xj*s;
  x[j] = xi*s + xj*c;

  return x;
}

double* Rotator::applyL(double *A, size_t m, size_t n)
{
  double *_i_ = (double*)_mm_malloc(sizeof(double)*n, 64),
         *_j_ = (double*)_mm_malloc(sizeof(double)*n, 64);

  for(size_t k=0; k<n; ++k)
  {
    _i_[k] = A[i*n + k]*(c)  + A[j*n + k]*(s);
    _j_[k] = A[i*n + k]*(-s) + A[j*n + k]*(c);
  }

  for(size_t k=0; k<n; ++k)
  {
    A[i*n + k] = _i_[k];
    A[j*n + k] = _j_[k];
  } 

  return A;
}

rotator kry::rotate_left(KMatrix &A, size_t i, size_t j)
{
  double xi = A.data[A.N*i + i],
         xj = A.data[A.N*j + i];

  long double c, s;
  long double B = fmax(fabs(xi), fabs(xj));
  if(B == 0)
  {
    c = 1, s = 0;
  }
  else
  {
    long double _xi = xi/B, _xj = xj/B;
    long double v = sqrt(_xi*_xi + _xj*_xj);
    c = _xi/v, s = _xj/v;
  }

  size_t n = A.N;
  double *_i_ = (double*)_mm_malloc(sizeof(double)*n, 64),
         *_j_ = (double*)_mm_malloc(sizeof(double)*n, 64);

  for(size_t k=i; k<n; ++k)
  {
    _i_[k] = A.data[i*n + k]*(c)  + A.data[j*n + k]*(s);
    _j_[k] = A.data[i*n + k]*(-s) + A.data[j*n + k]*(c);
  }

  for(size_t k=i; k<n; ++k)
  {
    A.data[i*n + k] = _i_[k];
    A.data[j*n + k] = _j_[k];
  } 

  rotator r;
  r.i = i;
  r.j = j;
  r.c = c;
  r.s = s;
  
  _mm_free(_i_);
  _mm_free(_j_);

  return r;

}

void kry::q_update(KMatrix Q, rotator r)
{
  
  size_t n = Q.N;
  size_t i = r.i, j = r.j;
  double s = r.s, c = r.c;
  double *_i_ = (double*)_mm_malloc(sizeof(double)*n, 64),
         *_j_ = (double*)_mm_malloc(sizeof(double)*n, 64);

  for(size_t k=0; k<n; ++k)
  {
    _i_[k] = Q.data[i*n + k]*(c)  + Q.data[j*n + k]*(s);
    _j_[k] = Q.data[i*n + k]*(-s) + Q.data[j*n + k]*(c);
  }

  for(size_t k=0; k<n; ++k)
  {
    Q.data[i*n + k] = _i_[k];
    Q.data[j*n + k] = _j_[k];
  } 

  _mm_free(_i_);
  _mm_free(_j_);
  
}

void kry::transpose(KMatrix Q)
{
  for(size_t i=0; i<Q.M; ++i)
  {
    for(size_t j=0; j<i; ++j)
    {
      double *ij = &Q.data[Q.N*i + j],
             *ji = &Q.data[Q.N*j + i];

      double tmp = *ij;
      *ij = *ji;
      *ji = tmp;
    }
  }
}

KVector kry::back_substitute(KMatrix &H, KVector &e)
{
  KVector t{e.N};
  t.data[0:e.N] = 1;

  double s;
  for(int i=e.N-1; i>=0; --i)
  {
    s=0;
    for(int j=e.N-1; j>i; --j)
    {
      s += H.data[i*e.N + j]*t.data[j];
    }
    t.data[i] = (e.data[i] - s)/H.data[i*e.N + i];
  }

  //...

  return t;
}

double* Rotator::applyR(double *A, size_t m, size_t n)
{

}

string kry::show_vec(size_t N, double *x)
{
  stringstream ss;
  ss << setprecision(16) << fixed;
  ss << "[";
  for(size_t i=0; i<N-1; ++i)
  {
    ss << x[i] << " ";
  }
  ss << x[N-1] << "]";
  return ss.str();
}

std::string kry::show_mat(size_t m, size_t n, double *A)
{
  stringstream ss;
  ss << setprecision(16) << fixed;
  for(size_t i=0; i<n; ++i)
  {
    for(size_t j=0; j<m; ++j)
    {
      ss << A[i*n + j] << " ";    
    }
    ss << std::endl;
  }
  return ss.str();
}

__attribute__((constructor)) void rnd_init() { srand48((long)time(NULL)); }

double kry::random_s(double min, double max)
{
  return drand48() * (max - min) + min;
}

void kry::random_v(size_t N, double *x, double min, double max)
{
  for(size_t i=0; i<N; ++i){ x[i] = random_s(min, max); }
}

void kry::random_m(size_t m, size_t n, double *A, double min, double max)
{
  for(size_t i=0; i<m*n; ++i){ A[i] = random_s(min, max); }
}

KMatrix kry::operator*(KMatrix &a, KMatrix &b)
{
  KMatrix c{a.N, a.N};
  c.data[0:a.N*a.N] = 0;

  for(size_t x=0; x<a.N; ++x)
  {
    for(size_t y=0; y<a.N; ++y)
    {
      for(size_t z=0; z<a.N; ++z)
      {
        c.data[x*a.N + y] += a.data[x*a.N + z] * b.data[z*a.N + y];
      }
    }
  }

  return c;
}


KVector kry::mul_ss_v(KSubspace &S, KVector &x)
{
  KVector Sx{S.N};

  for(size_t i=0; i<S.N; ++i)
  {
    Sx.data[i] = 0;
    for(size_t j=0; j<S.M; ++j)
    {
      Sx.data[i] += S.data[S.N*j + i] * x.data[j];
    }
  }

  return Sx;
}
