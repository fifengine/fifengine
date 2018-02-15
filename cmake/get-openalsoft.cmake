#------------------------------------------------------------------------------
# External Project:       OpenAL-Soft
#
# Website:                http://kcat.strangesoft.net/openal.html
# Downloads:              http://kcat.strangesoft.net/openal.html#download
#
# Alternative Downloads:  http://kcat.strangesoft.net/openal-binaries/  
#                         http://kcat.strangesoft.net/openal-releases/                                          
#------------------------------------------------------------------------------

INCLUDE(ExternalProject)

set(OPENALSOFT_VERSION "1.18.1")

# For win32 it's faster to download the pre-compiled development binaries.
if (WIN32)

    ExternalProject_Add(
        openalsoft
        PREFIX ${DEPENDENCY_EXTRACT_DIR}
        DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
        URL http://kcat.strangesoft.net/openal-binaries/openal-soft-${OPENALSOFT_VERSION}-bin.zip
        URL_MD5 2bb29a4f20892cce233546839752a221
        INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND 
            COMMAND ${CMAKE_COMMAND} -E copy ${DEPENDENCY_EXTRACT_DIR}/src/openalsoft/bin/Win32/soft_oal.dll  ${DEPENDENCY_INSTALL_DIR}/bin/OpenAL32.dll     
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/openalsoft/libs/Win32    ${DEPENDENCY_INSTALL_DIR}/lib
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/openalsoft/include/AL    ${DEPENDENCY_INSTALL_DIR}/include/AL
    )

else() # build from source

    SET(OPENALSOFT_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/openal)

    SET(OPENALSOFT_CMAKE_ARGS ${COMMON_CMAKE_ARGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        -DCMAKE_DEBUG_POSTFIX=d
        -DALSOFT_CONFIG=OFF
        -DALSOFT_EXAMPLES=OFF
        -DALSOFT_NO_CONFIG_UTIL=ON
        -DALSOFT_UTILS=OFF
        #-DLIBTYPE=STATIC
    )

    ExternalProject_Add(
        openalsoft
        PREFIX ${DEPENDENCY_EXTRACT_DIR}
        DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
        URL http://kcat.strangesoft.net/openal-releases/openal-soft-${OPENALSOFT_VERSION}.tar.bz2
        URL_MD5 04796baf3dcf1d30df631e79d9f96dd7
        INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
        CMAKE_ARGS ${OPENALSOFT_CMAKE_ARGS}
    )

endif()
