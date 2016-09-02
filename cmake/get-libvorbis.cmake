#------------------------------------------------------------------------------
# External Project:     libvorbis
# Downloads:            http://downloads.xiph.org/releases/vorbis/                                
#------------------------------------------------------------------------------

include(ExternalProject)

set(LIBOGG_VERSION "1.3.5")

set(LIBOGG_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}      
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

ExternalProject_Add(
    libogg
    PREFIX          ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
    DOWNLOAD_NAME   libvorbis-v${LIBOGG_VERSION}.zip
    URL             http://downloads.xiph.org/releases/vorbis/libvorbis-${LIBOGG_VERSION}.zip    
    URL_MD5         1
    PATCH_COMMAND   ${CMAKE_COMMAND} -E copy ${CMAKE_MODULE_PATH}/patches/libvorbis/CMakeLists.txt  ${DEPENDENCY_EXTRACT_DIR}/src/libvorbis
    CMAKE_ARGS      ${LIBOGG_CMAKE_ARGS}
)