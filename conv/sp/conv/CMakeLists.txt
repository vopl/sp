#################################################################################
project(sp-conv)

set(SRC

    ../math.hpp
    ../complex.hpp


    ###########################################################
    main.cpp
    test.cpp
    prony.cpp
    inv.cpp

    loadWav.hpp
    loadWav.cpp

    spectrStore.hpp
    spectrStore.cpp

    periodGrid.hpp
    periodGrid.cpp

    signalConvolver.hpp
    signalConvolver.cpp

    signalConvolverLevel.hpp
    signalConvolverLevel.cpp

    kernelTabled.hpp
    kernelTabled.cpp
)


############################################################################
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options filesystem)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "boost not found")
endif()

include_directories(${Boost_INCLUDE_DIRS})



############################################################################
add_executable(${PROJECT_NAME} ${SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} levmar lapack_part sp-utils)
target_link_libraries(${PROJECT_NAME} "-Wl,-Bstatic -lquadmath -Wl,-Bdynamic")
