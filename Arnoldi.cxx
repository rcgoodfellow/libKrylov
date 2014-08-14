#include "Arnoldi.hxx"

using namespace kry;

using std::thread;
using std::unique_lock;
using std::mutex;
using std::defer_lock_t;

#include <iostream>
static mutex print_lk;
using std::cout;
using std::endl;

//~=~ Arnoldi ~=~ -------------------------------------------------------------
Arnoldi::Arnoldi(size_t N, size_t n, KSMatrix A, double *x0, double *b)
  : N{N}, n{n},
    Q{N,n},
    H{n,n}, QH{KMatrix::Identity(n,n)},
    A{A}, x0{x0}, b{b},
    orch{this}
{ 
}

void Arnoldi::compute()
{
  LOG_FUNC();
  orch.begin();
  finished_cnd.notify_all();
}

//~=~ Orchestrator ~=~ --------------------------------------------------------
Orchestrator::Orchestrator(Arnoldi *A)
  : A{A}, m{0}, N{A->N}, n{A->n}, 
    workers(std::thread::hardware_concurrency(), Worker(this))
{
  s[:] = A->N;
  ortho_out = (double*)_mm_malloc(sizeof(double)*A->N, 64);
  subdot_out = (double*)_mm_malloc(sizeof(double)*A->n, 64);
  subdot_out[0:A->n] = 0;
  for(Worker &w : workers){ w.go(); }
}

Orchestrator::~Orchestrator()
{
  for(Worker &w : workers){ w.self_destruct = true; }
  s[3] = workers.size();
  start_work_cnd.notify_all();
  LOG(("\n%s\t", "Waiting for workers to self destruct"));
  while(s[3]){ usleep(1); LOG(("%s",".")); }
  LOG(("\n"));
}

void Orchestrator::begin()
{
  LOG_FUNC();
  compute_initial_residual();
  m = 1;

  while(!finished)
  {
    LOG(("%s\n", "~~iter~~"));
    unique_lock<mutex> lk{work_finished_mtx};
    {
      //Create leading subspace vector
      sysdot_in = A->Q(m-1);
      sysdot_out = A->Q(m);
      launchSysdotStage();
      work_finished_cnd.wait(lk, [this](){return s[0] == 0;});
      flush(&sysdotQ);

      //Orthogonalization
      subdot_in = sysdot_out;
      launchSubdotStage();
      work_finished_cnd.wait(lk, [this](){return s[1] == 0;});
      flush(&subdotQ);
      
      for(size_t i=0; i<m; ++i)
      {
        A->H.data[i*n + (m-1)] = subdot_out[i];
      }

      launchOrthoStage();
      work_finished_cnd.wait(lk, [this](){return s[2] == 0;});
      flush(&orthoQ);

      subdot_in[0:N] -= ortho_out[0:N];
    
      //Re-Orthogonalization
      launchSubdotStage();
      work_finished_cnd.wait(lk, [this](){return s[1] == 0;});
      flush(&subdotQ);
      
      for(size_t i=0; i<m; ++i)
      {
        A->H.data[i*n + (m-1)] += subdot_out[i];
      }

      launchOrthoStage();
      work_finished_cnd.wait(lk, [this](){return s[2] == 0;});
      flush(&orthoQ);

      subdot_in[0:N] -= ortho_out[0:N];

      double *the_muffin = &A->H.data[n*m + (m-1)];
      *the_muffin = norm_v(N, subdot_in);

      LOG(("%s %f\n", "the muffin", *the_muffin));
      if(fabs(*the_muffin) < 1e-15) 
      { 
        LOG(("%s\n", "muffin's cooked"));
        finished = true;
        break;
      }

      subdot_in[0:N] /= *the_muffin;

      ++m;
    }
    lk.unlock();

    decideFinished();
  }
  hessengulate();

}

void Orchestrator::compute_initial_residual()
{
  LOG_FUNC();

  unique_lock<mutex> lk{work_finished_mtx};
  {
    LOG(("%s\n", "job lock acquired"));
    sysdot_in = A->x0;
    sysdot_out = A->Q(0); 

    launchSysdotStage();
    work_finished_cnd.wait(lk, [this](){return s[0] == 0;});
    flush(&sysdotQ);

    sysdot_out[0:N] = A->b[0:N] - sysdot_out[0:N];
    A->r0_norm = norm_v(N, sysdot_out);
    sysdot_out[0:N] /= A->r0_norm;
  }
  lk.unlock();
}

void Orchestrator::flush(JobQ *q)
{
  for(size_t i=0; i<workers.size(); ++i){ q->push(-1); }
}
  
void Orchestrator::launchSysdotStage()
{
  sysdotQ.clear();
  s[0] = N;
  for(int i=0; i<N; ++i){ sysdotQ.push(i); }
  for(Worker &w : workers){ w.task = Worker::Task::Sysdot; }
  start_work_cnd.notify_all();
}

void Orchestrator::launchSubdotStage()
{
  subdotQ.clear();
  s[1] = m;
  for(int i=0; i<m; ++i){ subdotQ.push(i); }
  for(Worker &w : workers){ w.task = Worker::Task::Subdot; }
  start_work_cnd.notify_all();
}

void Orchestrator::launchOrthoStage()
{
  orthoQ.clear();
  s[2] = N;
  for(int i=0; i<N; ++i){ orthoQ.push(i); }
  for(Worker &w : workers){ w.task = Worker::Task::Ortho; }
  start_work_cnd.notify_all();
}

void Orchestrator::decideFinished()
{
  if(m > n){ finished = true; }
}

void Orchestrator::hessengulate()
{
  rotator r;
  KVector Qe{n};
  Qe.data[0:8] = 0;
  Qe.data[0] = A->r0_norm;

  cout << "H" << endl;
  cout << show_mat(n, n, A->H.data) << endl;
  for(size_t i=0; i<n-1; ++i)
  {
    r = rotate_left(A->H, i, i+1); 
    q_update(A->QH, r);
    r.apply_left(Qe);
  }
  cout << "H'" << endl;
  cout << show_mat(n, n, A->H.data) << endl;

  cout << "QH" << endl;
  cout << show_mat(n, n, A->QH.data) << endl;

  cout << "Qe" << endl;
  cout << show_vec(n, Qe.data) << endl;

  KVector t = back_substitute(A->H, Qe);
  cout << "t" << endl;
  cout << show_vec(n, t.data) << endl;

  KVector xn = mul_ss_v(A->Q, t);

  cout << "xn" << endl;
  cout << show_vec(xn.N, xn.data);

}

void Orchestrator::restart()
{
  //code for IRA goes here
  ++restart_count;
}

//~=~ Worker ~=~ --------------------------------------------------------------
Worker::Worker(Orchestrator *orch)
  : orch{orch}
{ }

void Worker::do_go()
{
  unique_lock<mutex> lk{orch->start_work_mtx, defer_lock_t{}};
  while(!self_destruct)
  {
    lk.lock();
    orch->start_work_cnd.wait(lk);
    lk.unlock();
    switch(task)
    {
      case Task::Sysdot : sysdot(); break;
      case Task::Subdot : subdot(); break;
      case Task::Ortho  : ortho();  break;
    }
  }
  --orch->s[3];
}

void Worker::go()
{
  thd = new thread{&Worker::do_go, this};
  thd->detach();
}

bool Worker::stopcheck(size_t which)
{
  if(orch->s[which] == 0) { orch->work_finished_cnd.notify_one(); return true; }
  if(orch->sysdotQ.empty()) { return true; }
  return false;
}

void Worker::sysdot()
{
  while(stopcheck(0) == false)
  {
    orch->sysdotQ.pop(id);

    if(id == -1){ return;}

    orch->sysdot_out[id] = dot_sv_v(orch->A->A, id, orch->sysdot_in);

    --orch->s[0];
  }
}

void Worker::subdot()
{
  while(stopcheck(1) == false)
  {
    orch->subdotQ.pop(id);

    if(id == -1){ return; }

    orch->subdot_out[id] =
      dot_v_v(
          orch->N, 
          orch->A->Q(id), 
          orch->subdot_in
          );
    
    --orch->s[1];
  }
}

void Worker::ortho()
{
  while(stopcheck(2) == false)
  {
    orch->orthoQ.pop(id);

    if(id == -1){ return; }
    
    orch->ortho_out[id] = 
      dot_v_v(
        orch->m, 
        orch->subdot_out, 1,
        &orch->A->Q.data[id], orch->N);

    --orch->s[2];
  }
}

