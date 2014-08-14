#ifndef KRY_ARNOLDI_HXX
#define KRY_ARNOLDI_HXX

#include "Kry.hxx"
#include "KryMath.hxx"
#include <atomic>
#include <vector>
#include <tbb/concurrent_queue.h>

namespace kry {

using JobQ = tbb::concurrent_bounded_queue<int>;

struct Orchestrator;
struct Worker;
struct Arnoldi;

struct Orchestrator
{
  Arnoldi *A;
  size_t m; //current subspace size
  size_t N, n;

  double *sysdot_in, *sysdot_out;
  double *subdot_in, *subdot_out;
  double *ortho_out;

  size_t restart_count{0};
  std::vector<Worker> workers;
  std::atomic<unsigned int> s[5];
  JobQ sysdotQ, subdotQ, orthoQ;
  std::mutex start_work_mtx, work_finished_mtx, finished_mtx;
  std::condition_variable start_work_cnd, work_finished_cnd, finished_cnd;
  bool finished{false};

  Orchestrator(Arnoldi *A); 
  ~Orchestrator();

  void begin();
  void compute_initial_residual();
  void flush(JobQ *q);
  void launchSysdotStage();
  void launchSubdotStage();
  void launchOrthoStage();
  void decideFinished();
  //hessenberg triangular annihilate
  void hessengulate();
  void restart();
};

struct Worker
{
  enum class Task{Sysdot, Subdot, Ortho};
  Task task{Task::Sysdot};
  int id;
  Orchestrator *orch;
  std::thread *thd;
  Worker(Orchestrator *orch);
  bool self_destruct{false};

  void go();

  private: 
    void do_go();
    void sysdot(), subdot(), ortho();
    bool stopcheck(size_t which);
};

struct Arnoldi
{
  size_t N, n;
  KSubspace Q;
  KMatrix H, QH;
  KSMatrix A;

  double *x0, *b;
  double r0_norm;
  
  Orchestrator orch;

  std::mutex finished_mtx;
  std::condition_variable finished_cnd;

  Arnoldi(size_t N, size_t n, KSMatrix A, double *x0, double *b);
  void compute();
};

}

#endif
