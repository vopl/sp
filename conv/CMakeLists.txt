set(CMAKE_CXX_FLAGS_RELEASE "")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -march=native -Ofast -funroll-loops -ffast-math -fno-signed-zeros -fno-trapping-math -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -march=native -O3")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-align-double")





project(conv)
cmake_minimum_required(VERSION 2.8)

set(SRC_LIST
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

file(GLOB LAPACK_PART_SRC lapack_part/*.c)
include_directories(lapack_part)


set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options filesystem)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "boost not found")
endif()

include_directories(${Boost_INCLUDE_DIRS})




add_executable(${PROJECT_NAME} ${SRC_LIST} ${DSP_SRC} ${DSP_INC} ${LAPACK_PART_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} levmar quadmath mpfr ${Boost_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} blas lapack)



add_subdirectory(levmar-2.6)
include_directories(levmar-2.6)
