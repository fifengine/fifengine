#------------------------------------------------------------------------------
# External Project:       SWIG (Simplified Wrapper and Interface Generator)
#
# Website:                http://www.swig.org/
# Downloads:              http://www.swig.org/download.html                                       
#------------------------------------------------------------------------------

INCLUDE(ExternalProject)

set(SWIG_VERSION "3.0.12")

# For Windows it's faster to download the pre-compiled development binaries.
if(WIN32)

    ExternalProject_Add(
        swig
        PREFIX              ${DEPENDENCY_EXTRACT_DIR}
        DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
        URL                 http://downloads.sourceforge.net/project/swig/swigwin/swigwin-${SWIG_VERSION}/swigwin-${SWIG_VERSION}.zip
        URL_MD5             a49524dad2c91ae1920974e7062bfc93
        #INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}/../build-tools/swig
        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ""
        INSTALL_COMMAND 
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/swig     ${DEPENDENCY_INSTALL_DIR}/../build-tools/swig
    )

else() 
    
    # TODO
    ExternalProject_Add(
        swig
        PREFIX              ${DEPENDENCY_EXTRACT_DIR}
        DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
        URL                 http://prdownloads.sourceforge.net/swig/swig-${SWIG_VERSION}.tar.gz
        URL_MD5             1     
        INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}/../build-tools/swig        
    )

endif()
