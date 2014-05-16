#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# libKrylov Build File
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

SET(CMAKE_CXX_COMPILER icpc)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
SET(CMAKE_CXX_COMPILER icl++)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

SET(CMAKE_C_COMPILER icc)
SET(CMAKE_CXX_FLAGS "-g -std=c++11 -Wall -Wextra -fcolor-diagnostics")

ADD_EXECUTABLE(VecTests VecTests.cxx)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ADD_EXECUTABLE(VecTestsMic VecTests.cxx)
SET_TARGET_PROPERTIES(VecTestsMic PROPERTIES COMPILE_FLAGS "-mmic ${CMAKE_CXX_FLAGS}")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
