#################################################################################
project(sp-cls)

set(SRC

    ../math.hpp
    ../complex.hpp
    ../serialization.hpp


    ###########################################################
    main.cpp

    patternExtractor.hpp
    patternExtractor.cpp


    shape.hpp
    shape.cpp


    nanoflann.hpp
    kdTree.hpp
)


############################################################################
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options filesystem serialization)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "boost not found")
endif()

include_directories(${Boost_INCLUDE_DIRS})



############################################################################
find_package(Qt5Gui REQUIRED)

############################################################################
add_executable(${PROJECT_NAME} ${SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} sp-utils)
#target_link_libraries(${PROJECT_NAME} levmar lapack_part)
#target_link_libraries(${PROJECT_NAME} "-Wl,-Bstatic -lquadmath -Wl,-Bdynamic")
target_link_libraries(${PROJECT_NAME} Qt5::Gui)
