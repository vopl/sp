#################################################################################
project(sp-utils)

set(SRC

    wavStore.hpp
    wavStore.cpp

    spectrStore.hpp
    spectrStore.cpp

)


############################################################################
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options filesystem)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "boost not found")
endif()

include_directories(${Boost_INCLUDE_DIRS})



############################################################################
add_library(${PROJECT_NAME} ${SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
