project(levmar)


############################################################################
set(HAVE_LAPACK 1)
set(LINSOLVERS_RETAIN_MEMORY 1)
set(LM_EXT_PREC 1)
set(LM_DBL_PREC 1)
set(LM_SNGL_PREC 0)

set(LEVMAR_DIR .)

configure_file(${LEVMAR_DIR}/levmar.h.in ${LEVMAR_DIR}/levmar.h)


set(LEVMAR_SRC
  ${LEVMAR_DIR}/lm.cpp
  ${LEVMAR_DIR}/Axb.cpp
  ${LEVMAR_DIR}/misc.cpp

  ${LEVMAR_DIR}/levmar.h
  ${LEVMAR_DIR}/misc.h
  ${LEVMAR_DIR}/compiler.h
)

set(LEVMAR_SRC_INC
  ${LEVMAR_DIR}/lm_core.cpp
  ${LEVMAR_DIR}/Axb_core.cpp
  ${LEVMAR_DIR}/misc_core.cpp
)

set_source_files_properties(${LEVMAR_SRC_INC} PROPERTIES HEADER_FILE_ONLY TRUE)







add_library(levmar STATIC ${LEVMAR_SRC} ${LEVMAR_INC})

set_property(TARGET levmar PROPERTY CXX_STANDARD 14)
set_property(TARGET levmar PROPERTY CXX_STANDARD_REQUIRED ON)
