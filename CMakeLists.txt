CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

SET(CMAKE_CXX_COMPILER /opt/intel/bin/icpc)
SET(CMAKE_C_COMPILER /opt/intel/bin/icc)

#SET(CMAKE_CXX_COMPILER clang++)
#SET(CMAKE_C_COMPILER clang)

#SET(CMAKE_CXX_COMPILER g++)
#SET(CMAKE_C_COMPILER gcc)

INCLUDE_DIRECTORIES(/opt/intel/composer_xe_2015.0.064/tbb/include
  /opt/intel/mkl/include
  )
LINK_DIRECTORIES(/opt/intel/composer_xe_2015.0.064/tbb/lib/intel64/gcc4.4)

SET(CMAKE_CXX_FLAGS 
  "-DDEBUG -std=c++11 -Wall -Wextra -fpic -O2 -debug inline-debug-info -shared-intel -g ")
SET(CMAKE_C_FLAGS 
  "-DDEBUG -std=c11 -Wall -Wextra -O2 -shared-intel -debug inline-debug-info -g")

SET(CMAKE_EXE_LINKER_FLAGS "-std=c++11")
SET(CMAKE_MODULE_LINKER_FLAGS "-std=c++11")
SET(CMAKE_SHARED_LINKER_FLAGS "-std=c++11")

ADD_LIBRARY(Kry SHARED
  Kry.hxx Kry.cxx
  KryOps.hxx KryOps.cxx
  KryMath.hxx KryMath.cxx
  Arnoldi.hxx Arnoldi.cxx
)

#ADD_EXECUTABLE(Kry1 Kry1.cxx)
#TARGET_LINK_LIBRARIES(Kry1 Kry tbb pthread m )

ADD_EXECUTABLE(Kry2 Kry2.cxx)
TARGET_LINK_LIBRARIES(Kry2 Kry tbb pthread m )

ADD_EXECUTABLE(ArnoldiTests ArnoldiTests.cxx)
TARGET_LINK_LIBRARIES(ArnoldiTests Kry tbb pthread m )

ADD_EXECUTABLE(ConcurrencyTest ConcurrencyTest.cxx)
TARGET_LINK_LIBRARIES(ConcurrencyTest pthread)

ADD_EXECUTABLE(RotatorTests RotatorTests.cxx)
TARGET_LINK_LIBRARIES(RotatorTests Kry tbb m)
