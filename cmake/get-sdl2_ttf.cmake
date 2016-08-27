#------------------------------------------------------------------------------
# External Project:     SDL2_ttf
# Downloads:            https://libsdl.org/projects/SDL_ttf/                                            
#------------------------------------------------------------------------------

include(ExternalProject)

set(SDL2_TTF_VERSION "2.0.14")

# For win32 it's faster to download the pre-compiled development binaries.
if (WIN32)

  if(MINGW) # download pre-compiled SDL2 dev package for MinGW 32/64-bit

    ExternalProject_Add(
      sdl2-ttf
      DEPENDS sdl2
      PREFIX ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
      URL https://libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-${SDL2_TTF_VERSION}-mingw.tar.gz
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
    URL http://download.savannah.gnu.org/releases/freetype/freetype-2.4.12.tar.gz
    DOWNLOAD_DIR ${DOWNLOAD_DIR}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
  )
  
  ExternalProject_Add(
    sdl2-ttf
    DEPENDS freetype2
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-${SDL2_TTF_VERSION}.tar.gz 
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR} --with-freetype-prefix=${DEPENDENCY_INSTALL_DIR}
  )

endif()