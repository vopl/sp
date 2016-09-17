set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


set(CMAKE_CXX_FLAGS_RELEASE "")


#################################################################################
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -Ofast -funroll-loops -ffast-math -fno-signed-zeros -fno-trapping-math -fomit-frame-pointer")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -march=native -O3")


set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=core2")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finline-limit=8192")



set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")


#################################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)



#################################################################################
execute_process(
    COMMAND git log -n1 --pretty=format:%H
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE "GIT_COMMIT"
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Git commit: ${GIT_COMMIT}")

add_definitions(-DGIT_COMMIT=${GIT_COMMIT})


#################################################################################
project(conv)
cmake_minimum_required(VERSION 2.8)

set(SRC
    main.cpp
    test.cpp
    prony.cpp
    inv.cpp

    sp/loadWav.hpp
    sp/loadWav.cpp

    sp/spectrStore.hpp
    sp/spectrStore.cpp

    sp/types.hpp
    sp/complex.hpp
    sp/math.hpp

    sp/config.hpp

    sp/periodGrid.hpp
    sp/periodGrid.cpp

#    sp/convolver.hpp
#    sp/convolver.cpp

    sp/signalConvolver.hpp
    sp/signalConvolver.cpp

    sp/signalConvolverLevel.hpp
    sp/signalConvolverLevel.cpp

#    sp/kernel.hpp
#    sp/kernel.cpp

    sp/kernelTabled.hpp
    sp/kernelTabled.cpp


#    test/responseModel.h
#    test/responseModel.cpp

#    test/signalProcessor.h
#    test/signalProcessor.cpp

#    test/scaledData.h
#    test/scaledData.cpp

#    test/settings.h
)

include_directories(.)


############################################################################
file(GLOB LAPACK_PART_SRC lapack_part/*.c)
include_directories(lapack_part)


############################################################################
SET(HAVE_LAPACK 1)
SET(LINSOLVERS_RETAIN_MEMORY 1)
SET(LM_EXT_PREC 1)
SET(LM_DBL_PREC 1)
SET(LM_SNGL_PREC 0)

SET(LEVMAR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/levmar-2.6)

CONFIGURE_FILE(${LEVMAR_DIR}/levmar.h.in ${LEVMAR_DIR}/levmar.h)
INCLUDE_DIRECTORIES(${LEVMAR_DIR})

set(LEVMAR_SRC
  ${LEVMAR_DIR}/lm.cpp
  ${LEVMAR_DIR}/Axb.cpp
  ${LEVMAR_DIR}/misc.cpp

  ${LEVMAR_DIR}/levmar.h
  ${LEVMAR_DIR}/misc.h
  ${LEVMAR_DIR}/compiler.h
)

set(LEVMAR_SRC_INC
  ${LEVMAR_DIR}/lm_core.cpp
  ${LEVMAR_DIR}/Axb_core.cpp
  ${LEVMAR_DIR}/misc_core.cpp
)

set_source_files_properties(${LEVMAR_SRC_INC} PROPERTIES HEADER_FILE_ONLY TRUE)


############################################################################
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options filesystem)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "boost not found")
endif()

include_directories(${Boost_INCLUDE_DIRS})




############################################################################
add_executable(${PROJECT_NAME} ${SRC} ${LAPACK_PART_SRC} ${LEVMAR_SRC} ${LEVMAR_SRC_INC})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} blas lapack)

target_link_libraries(${PROJECT_NAME} "-Wl,-Bstatic -lquadmath -Wl,-Bdynamic")




#add_subdirectory(levmar-2.6)
#include_directories(levmar-2.6)
