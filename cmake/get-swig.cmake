#------------------------------------------------------------------------------
# External Project:       SWIG (Simplified Wrapper and Interface Generator)
#
# Website:                http://www.swig.org/
# Downloads:              http://www.swig.org/download.html
#------------------------------------------------------------------------------

include(ExternalProject)

set(SWIG_VERSION "4.4.1")

# For Windows it's faster to download the pre-compiled development binaries.
if(WIN32)
  ExternalProject_Add(
    swig
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL http://downloads.sourceforge.net/project/swig/swigwin/swigwin-${SWIG_VERSION}/swigwin-${SWIG_VERSION}.zip
    URL_MD5 9402fd1f2a961cf2fe4afb7ff80dfa6f
    #INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}/../build-tools/swig
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND
    COMMAND
      ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/swig
      ${DEPENDENCY_INSTALL_DIR}/../build-tools/swig
  )
else()
  # Use SWIG 4.x which satisfies the minimum required by the project
  ExternalProject_Add(
    swig
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL http://prdownloads.sourceforge.net/swig/swig-${SWIG_VERSION}.tar.gz
    CONFIGURE_COMMAND
      ${CMAKE_COMMAND} -E echo "Configuring SWIG..." && ${CMAKE_COMMAND} -E chdir ${DEPENDENCY_EXTRACT_DIR}/src/swig sh
      -c "(./autogen.sh || true) && ./configure --prefix=${DEPENDENCY_INSTALL_DIR}/../build-tools/swig --without-pcre"
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${DEPENDENCY_EXTRACT_DIR}/src/swig sh -c "make"
    INSTALL_COMMAND ${CMAKE_COMMAND} -E chdir ${DEPENDENCY_EXTRACT_DIR}/src/swig sh -c "make install"
    INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}/../build-tools/swig
  )
endif()

set(SWIG_BUILT_EXECUTABLE "${DEPENDENCY_INSTALL_DIR}/../build-tools/swig/bin/swig" CACHE FILEPATH "SWIG executable built by get-swig")

