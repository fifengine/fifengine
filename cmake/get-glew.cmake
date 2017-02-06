#------------------------------------------------------------------------------
# External Project:     glew
# Downloads:            http://glew.sourceforge.net/
# Github:               https://sourceforge.net/projects/glew/files/glew/
#------------------------------------------------------------------------------

include(ExternalProject)

set(GLEW_VERSION "2.0.0")

set(GLEW_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -G ${CMAKE_GENERATOR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

ExternalProject_Add(
  glew
  PREFIX          ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME   glew-${GLEW_VERSION}.tgz
  URL             https://sourceforge.net/projects/glew/files/glew/${GLEW_VERSION}/glew-${GLEW_VERSION}.tgz/download
  URL_MD5         2a2cd7c98f13854d2fcddae0d2b20411
  CMAKE_ARGS      ${GLEW_CMAKE_ARGS}
)
