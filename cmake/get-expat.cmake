#------------------------------------------------------------------------------
# External Project:     expat
# Downloads:            http://www.libexpat.org/
# Github:               https://sourceforge.net/projects/expat/files/expat/
#------------------------------------------------------------------------------

include(ExternalProject)

set(EXPAT_VERSION "2.2.0")

set(EXPAT_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -G ${CMAKE_GENERATOR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

ExternalProject_Add(
  expat
  PREFIX          ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME   expat-${EXPAT_VERSION}.tar.bz2
  URL             https://sourceforge.net/projects/expat/files/expat/${EXPAT_VERSION}/expat-${EXPAT_VERSION}.tar.bz2/download    
  URL_MD5         2f47841c829facb346eb6e3fab5212e2
  CMAKE_ARGS      ${EXPAT_CMAKE_ARGS}
)
