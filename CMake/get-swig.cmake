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

set(SWIG_VERSION "4.5.1")
set(SWIG_FILE_HASH "5392a623386399e1eb5e51fec9edbabb")

if(NOT DEFINED FIFE_DEPENDENCIES_ROOT)
  message(FATAL_ERROR "FIFE_DEPENDENCIES_ROOT must be defined before including get-swig.cmake")
endif()
if(NOT DEFINED DEPENDENCY_EXTRACT_DIR)
  message(FATAL_ERROR "DEPENDENCY_EXTRACT_DIR must be defined before including get-swig.cmake")
endif()
if(NOT DEFINED DEPENDENCY_DOWNLOAD_DIR)
  message(FATAL_ERROR "DEPENDENCY_DOWNLOAD_DIR must be defined before including get-swig.cmake")
endif()

set(_SWIG_INSTALL_DIR ${FIFE_DEPENDENCIES_ROOT}/build-tools/swig)

# For Windows it's faster to download the pre-compiled development binaries.
if(WIN32)
  ExternalProject_Add(
    swig
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL https://downloads.sourceforge.net/project/swig/swigwin/swigwin-${SWIG_VERSION}/swigwin-${SWIG_VERSION}.zip
    URL_HASH MD5=${SWIG_FILE_HASH}
    # Preserve the archive timestamps. The build relies on the shipped
    # generated files (e.g. CParse/parser.c) being newer than their sources
    # (CParse/parser.y); letting CMake touch every file during extraction can
    # invert that order and force a bison regeneration.
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
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
    # Keep the release tarball's timestamps. SWIG ships pre-generated parser
    # files; if extraction stamps parser.y newer than parser.c, make invokes
    # bison, which is not part of the CI image.
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    CONFIGURE_COMMAND
    ${CMAKE_COMMAND} -E chdir ${DEPENDENCY_EXTRACT_DIR}/src/swig
    ${CMAKE_COMMAND} -E env
    # Ensure no auto-detection interferes
    PCRE_CFLAGS= PCRE_LIBS=
    ./configure
        --prefix=${_SWIG_INSTALL_DIR}
        --without-c
        --without-csharp
        --without-d
        --without-go
        --without-guile
        --without-java
        --without-javascript
        --without-lua
        --without-ocaml
        --without-octave
        --without-pcre
        --without-perl5
        --without-php
        --without-r
        --without-ruby
        --without-scilab
        --without-tcl
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${DEPENDENCY_EXTRACT_DIR}/src/swig make
    INSTALL_COMMAND ${CMAKE_COMMAND} -E chdir ${DEPENDENCY_EXTRACT_DIR}/src/swig make install
    INSTALL_DIR ${_SWIG_INSTALL_DIR}
  )

  # swig binary inside /bin folder of the installation directory
  set(SWIG_BUILT_EXECUTABLE "${_SWIG_INSTALL_DIR}/bin/swig"
      CACHE FILEPATH "SWIG executable built by get-swig")
endif()
