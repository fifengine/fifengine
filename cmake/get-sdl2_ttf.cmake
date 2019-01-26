#------------------------------------------------------------------------------
# External Project:     SDL2_ttf
# Downloads:            https://libsdl.org/projects/SDL_ttf/                                            
#------------------------------------------------------------------------------

include(ExternalProject)

set(SDL2_TTF_VERSION "2.0.15")

# For win32 it's faster to download the pre-compiled development binaries.
if (WIN32)

  if(MINGW) # download pre-compiled SDL2 dev package for MinGW 32/64-bit

    ExternalProject_Add(
      sdl2-ttf
      DEPENDS sdl2
      PREFIX ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
      URL https://libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-${SDL2_TTF_VERSION}-mingw.tar.gz
      URL_MD5 9d537c47736ba8112ee63cc94871ff06
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2-ttf/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2-ttf/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
    )

  endif(MINGW)

  if(MSVC) # download pre-compiled SDL2 dev package for Visual C++ 32/64-bit

    ExternalProject_Add(
      sdl2-ttf
      DEPENDS sdl2
      PREFIX ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
      URL https://libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-${SDL2_TTF_VERSION}-VC.zip
      URL_MD5 26f762c8d9b011505529f1f73fc45f88
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2-ttf/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2-ttf/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
    )

  endif(MSVC)

else() # build from source

  ExternalProject_Add(
    freetype2
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    URL http://download.savannah.gnu.org/releases/freetype/freetype-2.9.1.tar.gz
    URL_MD5 3adb0e35d3c100c456357345ccfa8056
    DOWNLOAD_DIR ${DOWNLOAD_DIR}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
  )
  
  ExternalProject_Add(
    sdl2-ttf
    DEPENDS freetype2
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-${SDL2_TTF_VERSION}.tar.gz 
    URL_MD5 04fe06ff7623d7bdcb704e82f5f88391
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR} --with-freetype-prefix=${DEPENDENCY_INSTALL_DIR}
  )

endif()