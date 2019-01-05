#------------------------------------------------------------------------------
# External Project:     SDL2_image
# Downloads:            https://libsdl.org/projects/SDL_image/                                            
#------------------------------------------------------------------------------

include(ExternalProject)

set(SDL2_IMAGE_VERSION "2.0.4")

# For win32 it's faster to download the pre-compiled development binaries.
if (WIN32)

  if(MINGW) # download pre-compiled SDL2 dev package for MinGW 32/64-bit

    ExternalProject_Add(
      sdl2-image
      DEPENDS sdl2
      PREFIX ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
      URL https://libsdl.org/projects/SDL_image/release/SDL2_image-devel-${SDL2_IMAGE_VERSION}-mingw.tar.gz  
      URL_MD5 53fd232ce4d70a3af18909a8a7248094
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2-image/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2-image/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
    )

  endif(MINGW)

  if(MSVC) # download pre-compiled SDL2 dev package for Visual C++ 32/64-bit

    ExternalProject_Add(
      sdl2-image
      DEPENDS sdl2
      PREFIX ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
      URL https://libsdl.org/projects/SDL_image/release/SDL2_image-devel-${SDL2_IMAGE_VERSION}-VC.zip
      URL_MD5 9435e3ecc2f03eed6a24c7ed2dd96d99
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2-image/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2-image/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
    )

  endif(MSVC)

else() # build from source
 
  ExternalProject_Add(
    sdl2-image
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL https://libsdl.org/projects/SDL_image/release/SDL2_image-${SDL2_IMAGE_VERSION}.tar.gz
    URL_MD5 88b9766d5b64a67f82ba511c57e7ce10
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
  )

endif()