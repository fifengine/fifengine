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
      -DZLIB_LIBRARY=${DEPENDENCY_INSTALL_DIR}/lib
      -DZLIB_INCLUDE_DIR=${DEPENDENCY_INSTALL_DIR}/include
      -DPNG_SHARED=off
      -DPNG_STATIC=on
      -DPNG_TESTS=off
      -DPNG_DEBUG=off
      -DPNGARG=off
      -DBUILD_SHARED_LIBS=FALSE
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_COMPILER_IS_GNUCXX=${CMAKE_COMPILER_IS_GNUCXX}
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS=${GL_BINDING_C_FLAGS}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
)

# For Windows it's faster to download the pre-compiled development binaries.
if (WIN32)

  if(MSVC)

    ExternalProject_Add(
      libpng
      DEPENDS         zlib
      PREFIX          ${DEPENDENCY_EXTRACT_DIR}  
      DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
      URL             ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/lpng1624.zip
      URL_MD5         9572a426a87407f6cfa41f61cbe2e3b2  
      #BUILD_COMMAND   "msbuild ${DEPENDENCY_EXTRACT_DIR}/src/libpng/projects/vstudio/vstudio.sln /t:libpng /p:Configuration=Release /p:Platform=Win32 /v:minimal"
      INSTALL_COMMAND
        COMMAND ${CMAKE_COMMAND} -E copy "${DEPENDENCY_EXTRACT_DIR}/src/libpng/projects/vstudio/Release Library/libpng16.lib" "${DEPENDENCY_INSTALL_DIR}/lib/libpng16.lib" 
      INSTALL_DIR     ${DEPENDENCY_INSTALL_DIR}
      CMAKE_ARGS      ${LIBPNG_CMAKE_ARGS}     
    )

  endif(MSVC)

else() # build from source
 
  ExternalProject_Add(
    libpng
    DEPENDS      zlib
    PREFIX       ${DEPENDENCY_EXTRACT_DIR}  
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL          ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/lpng1624.zip
    URL_MD5      9572a426a87407f6cfa41f61cbe2e3b2
    INSTALL_DIR  ${DEPENDENCY_INSTALL_DIR}
    CMAKE_ARGS   ${LIBPNG_CMAKE_ARGS}     
  )

endif()