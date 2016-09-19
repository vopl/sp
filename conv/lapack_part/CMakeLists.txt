project(lapack_part)

############################################################################
file(GLOB LAPACK_PART_SRC *.c *.h)

add_library(lapack_part STATIC ${LAPACK_PART_SRC})

