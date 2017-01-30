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

set(OPENAL_VERSION "1.17.2")

# For win32 it's faster to download the pre-compiled development binaries.
if (WIN32)

    ExternalProject_Add(
        openal
        PREFIX ${DEPENDENCY_EXTRACT_DIR}
        DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
        URL http://kcat.strangesoft.net/openal-binaries/openal-soft-${OPENAL_VERSION}-bin.zip
        URL_MD5 5f5a0484f8fcc808fead0cd23fd557b4     
        INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND 
            COMMAND ${CMAKE_COMMAND} -E copy ${DEPENDENCY_EXTRACT_DIR}/src/openal/bin/Win32/soft_oal.dll  ${DEPENDENCY_INSTALL_DIR}/bin/OpenAL32.dll     
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/openal/libs/Win32    ${DEPENDENCY_INSTALL_DIR}/lib
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/openal/include/AL    ${DEPENDENCY_INSTALL_DIR}/include/OpenAL
    )

else() # build from source

    SET(OPENAL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/openal)

    SET(OPENAL_CMAKE_ARGS ${COMMON_CMAKE_ARGS}
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
        openal
        PREFIX ${DEPENDENCY_EXTRACT_DIR}
        DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
        URL http://kcat.strangesoft.net/openal-releases/openal-soft-${OPENAL_VERSION}.tar.bz2
        URL_MD5 1764e0d8fec499589b47ebc724e0913d     
        INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
        CMAKE_ARGS ${OPENAL_CMAKE_ARGS}
    )

endif()
