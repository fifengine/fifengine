#------------------------------------------------------------------------------
# External Project:     libpng
# Website:              http://www.libpng.org/pub/png/libpng.html
# Downloads:            ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/lpng1624.zip           
#------------------------------------------------------------------------------

include(ExternalProject)

set(LIBPNG_VERSION "1.6.24")

set(LIBPNG_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
    -DPNG_SHARED=on
    -DPNG_STATIC=off
    -DPNG_TESTS=off
    -DPNG_DEBUG=off
    -DPNGARG=off
    -DBUILD_SHARED_LIBS=FALSE
    -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_COMPILER_IS_GNUCXX=${CMAKE_COMPILER_IS_GNUCXX}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS=${GL_BINDING_C_FLAGS}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
)

ExternalProject_Add(
  libpng
  DEPENDS      zlib
  PREFIX       ${DEPENDENCY_EXTRACT_DIR}
  INSTALL_DIR  ${DEPENDENCY_INSTALL_DIR}
  URL          ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/lpng1624.zip
  #URL_MD5
  CMAKE_ARGS   ${LIBPNG_CMAKE_ARGS}     
)