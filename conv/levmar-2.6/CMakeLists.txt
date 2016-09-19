# levmar CMake file; see http://www.cmake.org and
#                        http://www.insightsoftwareconsortium.org/wiki/index.php/CMake_Tutorial

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(LEVMAR)

SET(HAVE_LAPACK 1)
# the directory where the lapack/blas/f2c libraries reside

SET(LINSOLVERS_RETAIN_MEMORY 1 CACHE BOOL "Should linear solvers retain working memory between calls? (non-reentrant!)")


SET(LM_EXT_PREC 1)
SET(LM_DBL_PREC 0)
SET(LM_SNGL_PREC 0)


# compiler flags
#ADD_DEFINITIONS(-DLINSOLVERS_RETAIN_MEMORY) # do not free memory between linear solvers calls
#REMOVE_DEFINITIONS(-DLINSOLVERS_RETAIN_MEMORY) # free memory between calls

########################## NO CHANGES BEYOND THIS POINT ##########################

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/levmar.h.in ${CMAKE_CURRENT_SOURCE_DIR}/levmar.h)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})


# levmar library source files
ADD_LIBRARY(levmar STATIC
  lm.cpp Axb.cpp misc.cpp
  #lmlec.cpp lmbc.cpp lmblec.cpp lmbleic.cpp
  levmar.h misc.h compiler.h
)

set_property(TARGET levmar PROPERTY CXX_STANDARD 14)
set_property(TARGET levmar PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-align-double")

set(CMAKE_CXX_FLAGS_RELEASE "")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -march=native -Ofast -funroll-loops -ffast-math -fno-signed-zeros -fno-trapping-math -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -O3")
