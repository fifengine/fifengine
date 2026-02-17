#------------------------------------------------------------------------------
# External Project:    SWIG (Simplified Wrapper and Interface Generator)
#
# Website:             http://www.swig.org/
# Downloads:           http://www.swig.org/download.html
#
# Windows:             https://sourceforge.net/projects/swig/files/swigwin/
# Linux/Mac:           https://sourceforge.net/projects/swig/files/swig/
#------------------------------------------------------------------------------

include(ExternalProject)

set(SWIG_VERSION "4.4.1")

set(_SWIG_INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}/../build-tools/swig)

# For Windows it's faster to download the pre-compiled development binaries.
if(WIN32)
  ExternalProject_Add(
    swig
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL https://downloads.sourceforge.net/project/swig/swigwin/swigwin-${SWIG_VERSION}/swigwin-${SWIG_VERSION}.zip
    URL_MD5 9402fd1f2a961cf2fe4afb7ff80dfa6f
    INSTALL_DIR ${_SWIG_INSTALL_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND
    COMMAND
      ${CMAKE_COMMAND} -E rm -rf ${_SWIG_INSTALL_DIR}
    COMMAND
      ${CMAKE_COMMAND} -E make_directory ${_SWIG_INSTALL_DIR}
    COMMAND
     ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR> ${_SWIG_INSTALL_DIR}
    LOG_DOWNLOAD 1
    LOG_INSTALL 1
  )

  # swig.exe inside top-level of the installation directory
  set(SWIG_BUILT_EXECUTABLE "${_SWIG_INSTALL_DIR}/swig.exe"
      CACHE FILEPATH "SWIG executable built by get-swig")

else()
  # Use SWIG 4.x which satisfies the minimum required by the project
  # https://sourceforge.net/projects/swig/files/swig/
  ExternalProject_Add(
    swig
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL http://prdownloads.sourceforge.net/swig/swig-${SWIG_VERSION}.tar.gz
    CONFIGURE_COMMAND
    ${CMAKE_COMMAND} -E chdir ${DEPENDENCY_EXTRACT_DIR}/src/swig
    ${CMAKE_COMMAND} -E env
        # Ensure no auto-detection interferes
        PCRE_CFLAGS= PCRE_LIBS=
    ./configure
        --prefix=${_SWIG_INSTALL_DIR}
        --without-pcre
        --without-javascript
        --without-java
        --without-csharp
        --without-go
        --without-lua
        --without-perl5
        --without-php
        --without-r
        --without-ruby
        --without-tcl
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${DEPENDENCY_EXTRACT_DIR}/src/swig make
    INSTALL_COMMAND ${CMAKE_COMMAND} -E chdir ${DEPENDENCY_EXTRACT_DIR}/src/swig make install
    INSTALL_DIR ${_SWIG_INSTALL_DIR}
    LOG_DOWNLOAD 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
  )

  # swig binary inside /bin folder of the installation directory
  set(SWIG_BUILT_EXECUTABLE "${_SWIG_INSTALL_DIR}/bin/swig"
      CACHE FILEPATH "SWIG executable built by get-swig")
endif()
