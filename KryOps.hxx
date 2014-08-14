#ifndef KRY_OPERATORS_HXX
#define KRY_OPERATORS_HXX

#include "Kry.hxx"
#include "KryMath.hxx"

namespace kry {

KScalar operator * (KVec a, KVec b);
KVec operator * (KVec a, KScalar b);
KVec operator * (KScalar a, KVec b);

}

#endif
