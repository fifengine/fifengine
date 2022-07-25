#------------------------------------------------------------------------------
# External Project:     zlib
# Downloads:            http://zlib.net/
# Github:               https://github.com/madler/zlib
#------------------------------------------------------------------------------

include(ExternalProject)

set(ZLIB_VERSION "1.2.12")

set(ZLIB_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -G ${CMAKE_GENERATOR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      -DSKIP_INSTALL_FILES=ON
)

ExternalProject_Add(
  zlib
  PREFIX          ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME   zlib-v${ZLIB_VERSION}.zip
  URL             https://github.com/madler/zlib/archive/v${ZLIB_VERSION}.zip
  URL_MD5         50f045c0f544726cf70d1d865bb4d6a4
  CMAKE_ARGS      ${ZLIB_CMAKE_ARGS}
)
