#------------------------------------------------------------------------------
# External Project:     libvorbis
# Downloads:            http://downloads.xiph.org/releases/vorbis/                                
#------------------------------------------------------------------------------

include(ExternalProject)

set(LIBVORBIS_VERSION "1.3.5")

set(LIBVORBIS_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -G ${CMAKE_GENERATOR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}      
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      # find libogg
      -DCMAKE_PREFIX_PATH=${DEPENDENCY_INSTALL_DIR} ${CMAKE_PREFIX_PATH}
      -DOGG_LIBRARY=${DEPENDENCY_INSTALL_DIR}/lib/ogg.lib 
      -DOGG_INCLUDE_DIR=${DEPENDENCY_INSTALL_DIR}/include    
)

if(WIN32)
  add_definitions(-D_USE_MATH_DEFINES)
endif()

ExternalProject_Add(
    libvorbis
    DEPENDS         libogg
    PREFIX          ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
    DOWNLOAD_NAME   libvorbis-v${LIBOGG_VERSION}.zip
    URL             http://downloads.xiph.org/releases/vorbis/libvorbis-${LIBOGG_VERSION}.zip    
    URL_MD5         c39454ff13e79dc8c89bc3a9295b3298
    PATCH_COMMAND   ${CMAKE_COMMAND} -E copy ${CMAKE_MODULE_PATH}/patches/libvorbis/CMakeLists.txt  ${DEPENDENCY_EXTRACT_DIR}/src/libvorbis
    CMAKE_ARGS      ${LIBVORBIS_CMAKE_ARGS}
)
