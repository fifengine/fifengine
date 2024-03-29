#------------------------------------------------------------------------------
# External Project:     libogg
# Downloads:            http://downloads.xiph.org/releases/ogg/
#------------------------------------------------------------------------------

include(ExternalProject)

set(LIBOGG_VERSION "1.3.5")

set(LIBOGG_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -G ${CMAKE_GENERATOR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

ExternalProject_Add(
    libogg
    PREFIX          ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
    DOWNLOAD_NAME   libogg-${LIBOGG_VERSION}.zip
    URL             http://downloads.xiph.org/releases/ogg/libogg-${LIBOGG_VERSION}.zip
    URL_MD5         62169085ce1cff6780b45de91000c776
    PATCH_COMMAND   ${CMAKE_COMMAND} -E copy ${CMAKE_MODULE_PATH}/patches/libogg/CMakeLists.txt  ${DEPENDENCY_EXTRACT_DIR}/src/libogg
    CMAKE_ARGS      ${LIBOGG_CMAKE_ARGS}
)
