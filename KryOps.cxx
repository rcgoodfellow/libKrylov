#include "KryOps.hxx"

using namespace kry;

KScalar kry::operator * (KVec a, KVec b)
{
  KScalar ab(0);
  KTask::F f = [a,b,ab]() -> void { dot_vv(a, b, ab); };
  KryEngine::Enqueue(KTask{f}); 
  return ab;
}

KVec kry::operator * (KVec a, KScalar b)
{
  KVec ab(a.M);
  KTask::F f = [a,b,ab]() -> void { mul_vs(a, b, ab); };
  KryEngine::Enqueue(KTask{f});
  return ab;
}

KVec kry::operator * (KScalar a, KVec b)
{
  //just flip the ops to reuse the commutative equivelant
  return b * a;
}
