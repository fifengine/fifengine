#------------------------------------------------------------------------------
# External Project:     GLEW - OpenGL Extension Wrangler Library
# Downloads:            http://glew.sourceforge.net/
# Github:               https://github.com/nigels-com/glew
#------------------------------------------------------------------------------

include(ExternalProject)

set(GLEW_VERSION "2.2.0")

# For Windows it's faster to download the pre-compiled development binaries.
if (WIN32)

    ExternalProject_Add(
      glew
      PREFIX              ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
      URL                 https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
      URL_MD5             1feddfe8696c192fa46a0df8eac7d4bf
      CONFIGURE_COMMAND   ""
      BUILD_COMMAND       ""
      INSTALL_COMMAND
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/glew/bin/Release/Win32/  ${DEPENDENCY_INSTALL_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/glew/lib/Release/Win32/  ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/glew/include/GL          ${DEPENDENCY_INSTALL_DIR}/include/GL
    )

else() # build from source

  # TODO

endif()
