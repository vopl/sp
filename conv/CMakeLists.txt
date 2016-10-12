cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELEASE "")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")



#################################################################################
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")


set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=core2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=core2")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}      -Ofast -funroll-loops -fomit-frame-pointer -floop-nest-optimize -ftree-loop-im -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math -frounding-math")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -Ofast -funroll-loops -fomit-frame-pointer -floop-nest-optimize -ftree-loop-im -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math -frounding-math")

#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}      -fgraphite-identity -floop-interchange -floop-strip-mine -floop-block -floop-parallelize-all -ftree-loop-distribution ")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -fgraphite-identity -floop-interchange -floop-strip-mine -floop-block -fno-loop-parallelize-all -ftree-loop-distribution ")


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finline-limit=8192")

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,16777216")



#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")






#################################################################################
if( CMAKE_BUILD_TYPE MATCHES Release )
    find_program(CMAKE_GCC_AR NAMES ${_CMAKE_TOOLCHAIN_PREFIX}gcc-ar${_CMAKE_TOOLCHAIN_SUFFIX} HINTS ${_CMAKE_TOOLCHAIN_LOCATION})
    find_program(CMAKE_GCC_NM NAMES ${_CMAKE_TOOLCHAIN_PREFIX}gcc-nm HINTS ${_CMAKE_TOOLCHAIN_LOCATION})
    find_program(CMAKE_GCC_RANLIB NAMES ${_CMAKE_TOOLCHAIN_PREFIX}gcc-ranlib HINTS ${_CMAKE_TOOLCHAIN_LOCATION})

    if( CMAKE_GCC_AR AND CMAKE_GCC_NM AND CMAKE_GCC_RANLIB )
        set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -fno-fat-lto-objects" )
        set( CMAKE_AR "${CMAKE_GCC_AR}" )
        set( CMAKE_NM "${CMAKE_GCC_NM}" )
        set( CMAKE_RANLIB "${CMAKE_GCC_RANLIB}" )
    else()
        message( WARNING "GCC indicates LTO support, but binutils wrappers could not be found. Disabling LTO." )
    endif()
endif()





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




include_directories(.)

include_directories(eigen3)


set(LEVMAR_DIR "levmar-2.6")
include_directories(${LEVMAR_DIR})


add_subdirectory(${LEVMAR_DIR})
add_subdirectory(lapack_part)

add_subdirectory(sp/utils)
add_subdirectory(sp/conv)
add_subdirectory(sp/cls)

