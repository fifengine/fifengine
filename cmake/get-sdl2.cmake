#------------------------------------------------------------------------------
# External Project:     SDL2
# Downloads:            https://libsdl.org/release/                                             
#------------------------------------------------------------------------------

include(ExternalProject)

set(SDL2_VERSION "2.0.5")

set(SDL2_CMAKE_ARGS ${COMMON_CMAKE_ARGS}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
  )
   
  ExternalProject_Add(
    sdl2
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL http://libsdl.org/release/SDL2-${SDL2_VERSION}.tar.gz
    URL_MD5 1
    #CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
    INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
    CMAKE_ARGS ${SDL2_CMAKE_ARGS}
)
