#------------------------------------------------------------------------------
# External Project:     FifeChan / FifeGUI
# Downloads:            https://github.com/fifengine/fifechan/releases
#------------------------------------------------------------------------------

include(ExternalProject)

set(FIFECHAN_VERSION "0.1.5")

# For Windows it's faster to download the pre-compiled development binaries.
if(WIN32)

  if(MSVC) # download pre-compiled dev package for Visual C++ 32/64-bit
    ExternalProject_Add(
      fifechan
      PREFIX ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
      URL https://github.com/fifengine/fifechan/releases/download/0.1.5/fifechan-shared-dev-f848a38-VC15-x86.zip
      URL_MD5 4459fb7da09f3b26fa1a4116710ffa1d
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND
      COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/bin ${DEPENDENCY_INSTALL_DIR}/bin
      COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/lib ${DEPENDENCY_INSTALL_DIR}/lib
      COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/include
        ${DEPENDENCY_INSTALL_DIR}/include/fifechan
    )
  endif(MSVC)
else() # build from source
  # Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
  endif()

  # Prepare CMAKE_PREFIX_PATH arguments for ExternalProject; pass each entry
  # as its own -DCMAKE_PREFIX_PATH so ExternalProject does not split values.
  set(_FIFECHAN_PREFIX_ARGS)
  if(DEFINED DEPENDENCY_CMAKE_PREFIX_PATH)
    foreach(_p ${DEPENDENCY_CMAKE_PREFIX_PATH})
      list(APPEND _FIFECHAN_PREFIX_ARGS "-DCMAKE_PREFIX_PATH:PATH=${_p}")
    endforeach()
    message(STATUS "DEPENDENCY_CMAKE_PREFIX_PATH = '${DEPENDENCY_CMAKE_PREFIX_PATH}'")
  else()
    list(APPEND _FIFECHAN_PREFIX_ARGS "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux")
  endif()

  set(
    FIFECHAN_CMAKE_ARGS
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    ${_FIFECHAN_PREFIX_ARGS}
    -DSDL2_DIR:PATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/share/sdl2
    -DSDL2_IMAGE_DIR:PATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/share/sdl2-image
    -DSDL2_TTF_DIR:PATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/share/SDL2_ttf
  )

  # If vcpkg-installed SDL2 is present in the workspace, pass explicit include/library
  # locations so the fifechan configure step finds SDL2 reliably.
  if(EXISTS "${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/include/SDL2")
    list(
      APPEND FIFECHAN_CMAKE_ARGS
      -DSDL2_INCLUDE_DIR:PATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/include/SDL2
      -DSDL2_LIBRARY:FILEPATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/lib/libSDL2.a
      -DSDL2MAIN_LIBRARY:FILEPATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/lib/libSDL2main.a
    )
  endif()
  if(EXISTS "${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/include")
    list(
      APPEND FIFECHAN_CMAKE_ARGS
      -DSDL2_IMAGE_INCLUDE_DIR:PATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/include
      -DSDL2_IMAGE_LIBRARY:FILEPATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/lib/libSDL2_image.a
      -DSDL2_TTF_INCLUDE_DIR:PATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/include
      -DSDL2_TTF_LIBRARY:FILEPATH=${CMAKE_SOURCE_DIR}/../vcpkg_installed/x64-linux/lib/libSDL2_ttf.a
    )
  endif()

  set(DEPENDENCY_INSTALL_DIR ${CMAKE_BINARY_DIR}/dependencies/fifechan/install)
  set(DEPENDENCY_EXTRACT_DIR ${CMAKE_BINARY_DIR}/dependencies/fifechan/extract)
  set(DEPENDENCY_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/dependencies/fifechan/download)

  ExternalProject_Add(
    fifechan
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL https://github.com/fifengine/fifechan/archive/master.zip
    #CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
    INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
    CMAKE_ARGS ${FIFECHAN_CMAKE_ARGS}
  )
endif()
