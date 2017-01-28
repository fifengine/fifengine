#------------------------------------------------------------------------------
# External Project:     libpng
# Website:              http://www.libpng.org/pub/png/libpng.html
# Downloads:            https://github.com/glennrp/libpng/archive/v1.6.28.zip
# Github:               https://github.com/glennrp/libpng
#------------------------------------------------------------------------------

include(ExternalProject)

set(LIBPNG_VERSION "1.6.28")

set(LIBPNG_CMAKE_ARGS ${COMMON_CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_CONFIGURATION_TYPES=Release
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DBUILD_SHARED_LIBS=FALSE
    -DPNG_SHARED=off
    -DPNG_STATIC=on
    -DPNG_TESTS=off
    -DZLIB_LIBRARY:FILEPATH=${DEPENDENCY_INSTALL_DIR}/lib/zlib.lib
    -DZLIB_INCLUDE_DIR:PATH=${DEPENDENCY_INSTALL_DIR}/include    
)
    
ExternalProject_Add(
  libpng
  DEPENDS             zlib
  PREFIX              ${DEPENDENCY_EXTRACT_DIR}  
  DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME       libpng-v${LIBPNG_VERSION}.zip
  URL                 https://github.com/glennrp/libpng/archive/v${LIBPNG_VERSION}.zip
  URL_MD5             eeb04099fb0910df8d4691ecf8135cb8
  INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}
  CONFIGURE_COMMAND   ${CMAKE_COMMAND} ${DEPENDENCY_EXTRACT_DIR}/src/libpng 
         -G ${CMAKE_GENERATOR} 
         -DZLIB_INCLUDE_DIR:PATH=${DEPENDENCY_INSTALL_DIR}/include
         -DZLIB_LIBRARY:FILEPATH=${DEPENDENCY_INSTALL_DIR}/lib/zlib.lib
         -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
  BUILD_COMMAND       ${CMAKE_COMMAND} --build ${DEPENDENCY_EXTRACT_DIR}/src/libpng-build --config Release --target INSTALL
  #BUILD_COMMAND       msbuild libpng.sln /target:INSTALL /p:Platform=Win32 /p:Configuration=Release
  #INSTALL_COMMAND     msbuild libpng.sln /build Release /project INSTALL
  #CMAKE_ARGS   ${LIBPNG_CMAKE_ARGS}     
)
