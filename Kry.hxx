#ifndef KRY_HXX
#define KRY_HXX

#ifdef DEBUG
# define LOG(x) printf x
# define LOG_FUNC() printf("%s\n", __func__)
#else
# define LOG(x) ;
# define LOG_FUNC() ;
#endif

#include <cstddef>
#include <vector>
#include <random>
#include <queue>
#include <functional>
#include <initializer_list>
#include <memory>
#include <future>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>
#include <iomanip>

#include <tbb/concurrent_queue.h>
#include <mkl.h>
#include <mm_malloc.h>

namespace kry {

static constexpr size_t __malign__{64};

struct KTask
{
  using F = std::function<void()>;
  F f{[]()->void{}};

  void operator()();

  KTask(F);
  KTask() = default;
};

struct KQueue
{
  tbb::concurrent_bounded_queue<KTask> Q{};
};
 

struct KObject
{
  enum class State { Materializing, SolidState, Vapor };

  std::mutex mtx;
  std::condition_variable cnd;
  State state;
  size_t size;
  double *data;

  KObject(size_t size);
  ~KObject();
  
  void signalReady();
};

struct KSubspace
{
  size_t M, N;
  double *data;

  KSubspace(size_t N, size_t M);
  double* operator()(size_t);

};

struct KMatrix
{
  size_t M, N;
  double *data;

  KMatrix(size_t N, size_t M);
  static KMatrix Random(size_t M, size_t N, double min, double max);
  static KMatrix Identity(size_t M, size_t N);
};

struct KSMatrix
{
  size_t M, N, n;
  size_t *r, *c;
  double *v;

  KSMatrix(size_t M, size_t N, size_t n);
};

struct KVector
{
  size_t N;
  double *data;

  KVector(size_t N);
  static KVector Random(size_t N, double min, double max);
};

struct KVec
{
  size_t M;
  KObject *obj;

  explicit KVec(size_t m);
  KVec() = delete;
  KVec(std::initializer_list<double>);
  static KVec Random(size_t m);
  static KVec Fill(size_t m, double value);

  double & operator()(size_t);
};

struct KScalar
{
  KObject *obj;

  explicit KScalar(double);
  KScalar() = delete;

  double & operator()();
};

struct Rotator
{
  double c, s;
  size_t i, j;

  Rotator(double xi, double xj, size_t i, size_t j);
  double* applyT(double *x);
  double* apply(double *x);

  double* applyL(double *A, size_t m, size_t n);
  double* applyR(double *A, size_t m, size_t n);

};

struct rotator 
{ 
  size_t i, j; 
  double c, s; 

  void apply_left(KVector &v);
};

rotator rotate_left(KMatrix &A, size_t i, size_t j);
void q_update(KMatrix Q, rotator r);
void transpose(KMatrix Q);
KVector back_substitute(KMatrix &H, KVector &e);

KMatrix operator*(KMatrix &a, KMatrix &b);

class KryEngine
{
  KQueue Q;
  static void __kthread_go__();

  public:
    static void TStart(size_t num_threads);
    static void Enqueue(KTask t);

    static KryEngine& get();
};

std::string show_vec(size_t N, double *x);
std::string show_mat(size_t m, size_t n, double *A);

double random_s(double min, double max);
void random_v(size_t N, double *x, double min, double max);
void random_m(size_t m, size_t n, double *A, double min, double max);

KVector mul_ss_v(KSubspace &S, KVector &x);

}

#endif
