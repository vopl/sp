project(conv)
cmake_minimum_required(VERSION 2.8)

set(SRC_LIST
    main.cpp

    sp/math.hpp

    sp/config.hpp

    sp/window_rect.hpp
    sp/window_nutall.hpp
    sp/window_hann.hpp
    sp/window_hann2.hpp

    sp/convolve.hpp
)

include_directories(.)

add_executable(${PROJECT_NAME} ${SRC_LIST})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
