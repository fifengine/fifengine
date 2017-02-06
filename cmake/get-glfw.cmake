#------------------------------------------------------------------------------
# External Project:     glfw
# Downloads:            http://www.glfw.org/
# Github:               https://github.com/glfw/glfw
#------------------------------------------------------------------------------

include(ExternalProject)

set(GLFW_VERSION "3.2.1")

set(GLFW_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -G ${CMAKE_GENERATOR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

ExternalProject_Add(
  glfw
  PREFIX          ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME   glfw-${GLFW_VERSION}.zip
  URL             https://github.com/glfw/glfw/releases/download/${GLFW_VERSION}/glfw-${GLFW_VERSION}.zip
  URL_MD5         824c99eea073bdd6d2fec76b538f79af
  CMAKE_ARGS      ${GLFW_CMAKE_ARGS}
)
