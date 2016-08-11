project(conv)
cmake_minimum_required(VERSION 2.8)

set(SRC_LIST
    main.cpp

    sp/types.hpp
    sp/complex.hpp
    sp/math.hpp

    sp/config.hpp

    sp/window_rect.hpp
    sp/window_nutall.hpp
    sp/window_hann.hpp
    sp/window_hann2.hpp

    sp/convolve.hpp


    sp/responseModel.hpp
    sp/responseModel.cpp
)

include_directories(.)

add_executable(${PROJECT_NAME} ${SRC_LIST})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} levmar lapack blas)



add_subdirectory(levmar-2.6)
