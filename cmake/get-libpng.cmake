#------------------------------------------------------------------------------
# External Project:     libpng
# Website:              http://www.libpng.org/pub/png/libpng.html
# Downloads:            https://github.com/glennrp/libpng/archive/v1.6.37.zip
# Github:               https://github.com/glennrp/libpng
#------------------------------------------------------------------------------

include(ExternalProject)

set(LIBPNG_VERSION "1.6.37")

set(LIBPNG_CMAKE_ARGS ${COMMON_CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
    -DBUILD_SHARED_LIBS=TRUE
    -DPNG_SHARED=on
    -DPNG_STATIC=off
    -DPNG_TESTS=off
    -DPNG_DEBUG=off
    -DZLIB_LIBRARY:FILEPATH=${DEPENDENCY_INSTALL_DIR}/lib/zlib.lib
    -DZLIB_INCLUDE_DIR:PATH=${DEPENDENCY_INSTALL_DIR}/include
)

ExternalProject_Add(
  libpng
  DEPENDS             zlib
  PREFIX              ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME       libpng-v${LIBPNG_VERSION}.zip
  URL                 https://github.com/glennrp/libpng/archive/v${LIBPNG_VERSION}.zip
  URL_MD5             1bbb6b2bcd580d50b82145e827eb3fb4
  INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}
  CMAKE_ARGS          ${LIBPNG_CMAKE_ARGS}
)
