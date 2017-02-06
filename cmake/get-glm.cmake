#------------------------------------------------------------------------------
# External Project:     glm
# Downloads:            http://glm.g-truc.net/0.9.8/index.html
# Github:               https://github.com/g-truc/glm/tags
#------------------------------------------------------------------------------

include(ExternalProject)

set(GLM_VERSION "0.9.8.4")

set(GLM_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -G ${CMAKE_GENERATOR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

ExternalProject_Add(
  glm
  PREFIX          ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME   glm-${GLM_VERSION}.tar.gz
  URL             https://github.com/g-truc/glm/archive/${GLM_VERSION}.tar.gz
  URL_MD5         26ca8792038da90f67301fa0a3d5b693
  CMAKE_ARGS      ${GLM_CMAKE_ARGS}
)
