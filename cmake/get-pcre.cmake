#------------------------------------------------------------------------------
# External Project:     pcre
# Homepage:             http://pcre.org/
# Downloads:            https://ftp.pcre.org/pub/pcre/
#------------------------------------------------------------------------------

include(ExternalProject)

set(PCRE_VERSION "8.40")

set(PCRE_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -G ${CMAKE_GENERATOR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

ExternalProject_Add(
  pcre
  PREFIX          ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME   pcre-${PCRE_VERSION}.tar.bz2
  URL             https://ftp.pcre.org/pub/pcre/pcre-${PCRE_VERSION}.tar.bz2
  URL_MD5         41a842bf7dcecd6634219336e2167d1d
  CMAKE_ARGS      ${PCRE_CMAKE_ARGS}
)
