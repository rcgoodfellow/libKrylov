#include "kry.h"

void Kry_TStart(size_t num_threads, Kry_Q *Q)
{

  for(size_t i=0; i<num_threads; ++i) 
  {
    Kry_Thread *kt = (Kry_Thread*)malloc(sizeof(Kry_Thread));
    kt->Q = Q;
    pthread_create(&kt->t, NULL, &__krythread__, kt);
  }
}

void* __krythread__(void *x)
{
  Kry_Thread *kt = (Kry_Thread*)x;

  //here we wait for the queue to have work

  return NULL;
}


Kry_Task* Kry_CreateTask(Kry_F f, void *args)
{
  Kry_Task *kt = (Kry_Task*)malloc(sizeof(Kry_Task));
  kt->f = f;
  kt->args = args;
  kt->state = CREATED;

  kt->downstream = NULL;
  kt->downstream_count = 0;
  pthread_mutex_init(&kt->downstream_mtx, NULL);

  kt->waiting_count = 0;
  pthread_mutex_init(&kt->waiting_mtx, NULL);
  pthread_cond_init(&kt->waiting_cond, NULL);

  kt->Q = NULL;
  return kt;
}

KryQ::KryQ()
{
  head = NULL;
  tail = NULL;
  
  pthread_mutex_init(&KQ->mtx, NULL);
  pthread_cond_init(&KQ->cnd, NULL);

  return KQ;
}

void Kry_Init()
{
  __krystate__ = (Kry_State*)malloc(sizeof(Kry_State)); 
}

Kry_Error Kry_AddDependency(Kry_Task *dependent, Kry_Task *independent)
{
   
  return Kry_OK;
}
