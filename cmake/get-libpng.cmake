#------------------------------------------------------------------------------
# External Project:     libpng
# Website:              http://www.libpng.org/pub/png/libpng.html
# Downloads:            ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/lpng1624.zip           
# Github:               https://github.com/glennrp/libpng
#------------------------------------------------------------------------------

include(ExternalProject)

set(LIBPNG_VERSION "1.6.24")

set(LIBPNG_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
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
      -DZLIB_LIBRARY:FILEPATH=${DEPENDENCY_INSTALL_DIR}/zlib/lib/zlib.lib
      -DZLIB_INCLUDE_DIR:PATH=${DEPENDENCY_INSTALL_DIR}/zlib/include    
)
    
#BUILD_COMMAND   "msbuild ${DEPENDENCY_EXTRACT_DIR}/src/libpng/projects/vstudio/vstudio.sln /t:libpng /p:Configuration=Release /p:Platform=Win32 /v:minimal"
#INSTALL_COMMAND
#  COMMAND ${CMAKE_COMMAND} -E copy "${DEPENDENCY_EXTRACT_DIR}/src/libpng/projects/vstudio/Release Library/libpng16.lib" "${DEPENDENCY_INSTALL_DIR}/lib/libpng16.lib" 

ExternalProject_Add(
  libpng
  DEPENDS             zlib
  PREFIX              ${DEPENDENCY_EXTRACT_DIR}  
  DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
  URL                 ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/lpng1624.zip
  URL_MD5             9572a426a87407f6cfa41f61cbe2e3b2
  INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}
  CONFIGURE_COMMAND   ${CMAKE_COMMAND} ${DEPENDENCY_EXTRACT_DIR}/src/libpng 
         -G ${CMAKE_GENERATOR} 
         -DZLIB_INCLUDE_DIR:PATH=${DEPENDENCY_INSTALL_DIR}/zlib/include
         -DZLIB_LIBRARY:FILEPATH=${DEPENDENCY_INSTALL_DIR}/zlib/lib/zlib.lib
         -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
  BUILD_COMMAND       devenv libpng.sln /build Release /project png16
  INSTALL_COMMAND     devenv libpng.sln /build Release /project INSTALL
  #CMAKE_ARGS   ${LIBPNG_CMAKE_ARGS}     
)