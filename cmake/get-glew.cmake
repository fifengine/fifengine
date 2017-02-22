#------------------------------------------------------------------------------
# External Project:     GLEW - OpenGL Extension Wrangler Library
# Downloads:            http://glew.sourceforge.net/  
# Github:               https://github.com/nigels-com/glew                                          
#------------------------------------------------------------------------------

include(ExternalProject)

set(GLEW_VERSION "2.0.0")

# For Windows it's faster to download the pre-compiled development binaries.
if (WIN32)

  if(MSVC) # download pre-compiled dev package for Visual C++ 32/64-bit

    ExternalProject_Add(
      glew
      PREFIX              ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
      URL                 https://github.com/nigels-com/glew/releases/download/glew-${GLEW_VERSION}/glew-${GLEW_VERSION}-win32.zip   
      URL_MD5             7a11b325d21b610c8ded673719740619
      CONFIGURE_COMMAND   ""
      BUILD_COMMAND       ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/glew/bin/Release/Win32/  ${DEPENDENCY_INSTALL_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/glew/lib/Release/Win32/  ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/glew/include/GL          ${DEPENDENCY_INSTALL_DIR}/include/GL
    )

  endif(MSVC)

  if(MINGW) # download pre-compiled dev package for MinGW 32/64-bit
    
    # TODO

  endif(MINGW)

else() # build from source

  # TODO

endif()
