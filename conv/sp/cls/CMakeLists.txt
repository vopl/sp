#################################################################################
project(sp-cls)

set(SRC

    ../math.hpp
    ../complex.hpp


    ###########################################################
    main.cpp

    spectrFetcher.hpp
    spectrFetcher.cpp

    som.hpp
    som.cpp

    nanoflann.hpp
    KDTreeVectorOfVectorsAdaptor.h
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
#target_link_libraries(${PROJECT_NAME} levmar lapack_part)
#target_link_libraries(${PROJECT_NAME} "-Wl,-Bstatic -lquadmath -Wl,-Bdynamic")
