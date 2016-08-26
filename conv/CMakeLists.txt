#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -march=native -Ofast -funroll-loops -fno-math-errno -funsafe-math-optimizations -fassociative-math -ffinite-math-only -g -DNDEBUG -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -march=native -O3 -funroll-loops -g -DNDEBUG -fomit-frame-pointer")


project(conv)
cmake_minimum_required(VERSION 2.8)

set(SRC_LIST
    main.cpp

    sp/types.hpp
    sp/complex.hpp
    sp/math.hpp

    sp/config.hpp

    sp/periodGrid.hpp
    sp/periodGrid.cpp

    sp/convolver.hpp
    sp/convolver.cpp

    sp/signalConvolver.hpp
    sp/signalConvolver.cpp

    sp/signalConvolverLevel.hpp
    sp/signalConvolverLevel.cpp

    sp/kernel.hpp
    sp/kernel.cpp

    sp/kernelTabled.hpp
    sp/kernelTabled.cpp


    test/responseModel.h
    test/responseModel.cpp

    test/signalProcessor.h
    test/signalProcessor.cpp

    test/scaledData.h
    test/scaledData.cpp

    test/settings.h
)

include_directories(.)







add_executable(${PROJECT_NAME} ${SRC_LIST} ${DSP_SRC} ${DSP_INC})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} levmar lapack blas quadmath)



add_subdirectory(levmar-2.6)
include_directories(levmar-2.6)
