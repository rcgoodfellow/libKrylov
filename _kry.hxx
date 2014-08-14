
typedef void(*Kry_F)(void *args);

struct Kry_Q;

typedef enum Kry_TState { CREATED, WAITING, EXECUTING, FINISHED } Kry_TState;
typedef struct Kry_Task
{
  Kry_F f;
  void *args;
  Kry_TState state;

  struct Kry_Task *downstream;
  size_t downstream_count;
  pthread_mutex_t downstream_mtx;

  size_t waiting_count;
  pthread_mutex_t waiting_mtx;
  pthread_cond_t waiting_cond;

  struct Kry_Q *Q;
} 
Kry_Task;

typedef struct Kry_Q
{
  Kry_Task *head, *tail;
  pthread_mutex_t mtx;
  pthread_cond_t cnd;
}
Kry_Q;

typedef struct Kry_Thread
{
  pthread_t t;
  Kry_Task task;
  Kry_Q *Q;
}
Kry_Thread;

typedef struct Kry_State 
{
  Kry_Thread *pool;
}
Kry_State;

void Kry_Init();

Kry_State *__krystate__;

typedef enum Kry_Error
{
  Kry_OK,
  Kry_Fail
}
Kry_Error;

Kry_Q* Kry_CreateQ();
Kry_Task* Kry_CreateTask(Kry_F, void*);
void Kry_TStart(size_t num_threads, Kry_Q *Q);
void* __krythread__(void*);
Kry_Error Kry_AddDependency(Kry_Task *dependent, Kry_Task *independent);

#endif
