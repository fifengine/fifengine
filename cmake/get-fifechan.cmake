#------------------------------------------------------------------------------
# External Project:     FifeChan / FifeGUI
# Downloads:            https://github.com/fifengine/fifechan/releases                                            
#------------------------------------------------------------------------------

include(ExternalProject)

set(FIFECHAN_VERSION "0.1.4")

# For Windows it's faster to download the pre-compiled development binaries.
if (WIN32)

  #if(MINGW) # download pre-compiled dev package for MinGW 32/64-bit
    
    #ExternalProject_Add(
      #fifechan
      #PREFIX              ${DEPENDENCY_EXTRACT_DIR}
      #DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
      #URL                 https://github.com/fifengine/fifechan/releases/tag/${FIFECHAN_VERSION}/fifechan-v${FIFECHAN_VERSION}-MinGW.tar.gz
      #URL                 https://ci.appveyor.com/api/projects/jakoch/fifechan/artifacts/fifechan-shared-MinGW.tar.gz
      #URL_MD5             1
      #CONFIGURE_COMMAND   ""
      #BUILD_COMMAND       ""
      #INSTALL_COMMAND 
        #COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        #COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/include ${DEPENDENCY_INSTALL_DIR}/include/fifechan
    #)

  #endif(MINGW)

  if(MSVC) # download pre-compiled dev package for Visual C++ 32/64-bit

    ExternalProject_Add(
      fifechan
      PREFIX              ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
      #URL                 https://github.com/fifengine/fifechan/releases/tag/${FIFECHAN_VERSION}/fifechan-v${FIFECHAN_VERSION}-VC14.zip
      URL                 https://ci.appveyor.com/api/buildjobs/ut5f5r7r7vdo9jvs/artifacts/fifechan-shared-VC14-x86.zip
      URL_MD5             4922f113510f61b81288c9dba1538a44
      CONFIGURE_COMMAND   ""
      BUILD_COMMAND       ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/bin     ${DEPENDENCY_INSTALL_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/include ${DEPENDENCY_INSTALL_DIR}/include/fifechan
    )

  endif(MSVC)

else() # build from source

  set(FIFECHAN_CMAKE_ARGS
      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
  )
   
  ExternalProject_Add(
    fifechan
    PREFIX              ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
    URL                 https://github.com/fifengine/fifechan/archive/master.zip
    URL_MD5             1
    #CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
    INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}
    CMAKE_ARGS          ${FIFECHAN_CMAKE_ARGS}
  )

endif()
