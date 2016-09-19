cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(CMAKE_CXX_FLAGS_RELEASE "")


#################################################################################
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast -funroll-loops -ffast-math -fno-signed-zeros -fno-trapping-math -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -funroll-loops -ffast-math -fno-signed-zeros -fno-trapping-math -fomit-frame-pointer")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -O3")


set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finline-limit=8192")



#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")


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

set(LEVMAR_DIR "levmar-2.6")
include_directories(${LEVMAR_DIR})


add_subdirectory(${LEVMAR_DIR})
add_subdirectory(lapack_part)

add_subdirectory(sp/conv)
add_subdirectory(sp/cls)

