#------------------------------------------------------------------------------
# External Project:     FifeChan / FifeGUI
# Downloads:            https://github.com/fifengine/fifechan/releases
#------------------------------------------------------------------------------

include(ExternalProject)

set(FIFECHAN_VERSION "0.1.5")

# Default dependency paths
if(NOT DEFINED DEPENDENCY_INSTALL_DIR)
  set(DEPENDENCY_INSTALL_DIR ${CMAKE_BINARY_DIR}/fifechan/install)
endif()
if(NOT DEFINED DEPENDENCY_EXTRACT_DIR)
  set(DEPENDENCY_EXTRACT_DIR ${CMAKE_BINARY_DIR}/fifechan/extract)
endif()
if(NOT DEFINED DEPENDENCY_DOWNLOAD_DIR)
  set(DEPENDENCY_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/fifechan/download)
endif()

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
  # as its own -DCMAKE_PREFIX_PATH argument.
  set(_FIFECHAN_PREFIX_ARGS)
  if(DEFINED DEPENDENCY_CMAKE_PREFIX_PATH)
    foreach(_p ${DEPENDENCY_CMAKE_PREFIX_PATH})
      list(APPEND _FIFECHAN_PREFIX_ARGS "-DCMAKE_PREFIX_PATH:PATH=${_p}")
    endforeach()
    message(STATUS "DEPENDENCY_CMAKE_PREFIX_PATH = '${DEPENDENCY_CMAKE_PREFIX_PATH}'")
  elseif(DEFINED CMAKE_PREFIX_PATH)
    foreach(_p ${CMAKE_PREFIX_PATH})
      list(APPEND _FIFECHAN_PREFIX_ARGS "-DCMAKE_PREFIX_PATH:PATH=${_p}")
    endforeach()
  elseif(DEFINED VCPKG_INSTALLED_DIR)
    list(APPEND _FIFECHAN_PREFIX_ARGS "-DCMAKE_PREFIX_PATH:PATH=${VCPKG_INSTALLED_DIR}")
  endif()

  set(_FIFECHAN_VCPKG_ARGS)
  if(DEFINED VCPKG_TARGET_TRIPLET)
    list(APPEND _FIFECHAN_VCPKG_ARGS "-DVCPKG_TARGET_TRIPLET:STRING=${VCPKG_TARGET_TRIPLET}")
  endif()
  if(DEFINED VCPKG_INSTALLED_DIR)
    list(APPEND _FIFECHAN_VCPKG_ARGS "-DVCPKG_INSTALLED_DIR:PATH=${VCPKG_INSTALLED_DIR}")
  endif()

  set(
    FIFECHAN_CMAKE_ARGS
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DVCPKG_MANIFEST_MODE=OFF
    ${_FIFECHAN_VCPKG_ARGS}
    ${_FIFECHAN_PREFIX_ARGS}
  )

  if(NOT TARGET fifechan)
    ExternalProject_Add(
      fifechan
      PREFIX ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
      URL https://github.com/fifengine/fifechan/archive/refs/tags/0.1.5.zip
      #CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
      INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
      CMAKE_ARGS ${FIFECHAN_CMAKE_ARGS}
    )
  endif()

  # Create a stable wrapper target that other targets can depend on.
  set(_fifechan_stamp "${DEPENDENCY_INSTALL_DIR}/.fifechan_installed")
  ExternalProject_Add_Step(
    fifechan
    touchstamp
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPENDENCY_INSTALL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${_fifechan_stamp}"
    DEPENDEES install
  )

  # Ensure built library files and headers are copied
  ExternalProject_Add_Step(
    fifechan
    install_files
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPENDENCY_INSTALL_DIR}/lib"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPENDENCY_INSTALL_DIR}/include"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "<BINARY_DIR>/lib" "${DEPENDENCY_INSTALL_DIR}/lib"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "<SOURCE_DIR>/include" "${DEPENDENCY_INSTALL_DIR}/include"
    DEPENDEES build
  )

  if(NOT TARGET fifechan-install)
    add_custom_target(fifechan-install DEPENDS "${_fifechan_stamp}")
  endif()

  set(FIFECHAN_INSTALL_STAMP "${_fifechan_stamp}" CACHE FILEPATH "Fifechan install stamp file")
  endif()

# Export expected install locations and create imported targets
set(FIFECHAN_BUILT_INCLUDE_DIR "${DEPENDENCY_INSTALL_DIR}/include" CACHE PATH "FifeChan include dir from get-fifechan")
set(FIFECHAN_BUILT_LIBRARY_DIR "${DEPENDENCY_INSTALL_DIR}/lib" CACHE PATH "FifeChan lib dir from get-fifechan")

if(NOT TARGET Fifechan::fifechan)
  add_library(Fifechan::fifechan UNKNOWN IMPORTED)
  set_target_properties(Fifechan::fifechan PROPERTIES
    INTERFACE_LINK_LIBRARIES "-L${FIFECHAN_BUILT_LIBRARY_DIR} -lfifechan"
  )
endif()

# Optionally create component targets commonly used by the project
if(NOT TARGET Fifechan::sdl)
  add_library(Fifechan::sdl UNKNOWN IMPORTED)
  set_target_properties(Fifechan::sdl PROPERTIES
    INTERFACE_LINK_LIBRARIES "-L${FIFECHAN_BUILT_LIBRARY_DIR} -lfifechan_sdl"
  )
endif()

if(NOT TARGET Fifechan::opengl)
  add_library(Fifechan::opengl UNKNOWN IMPORTED)
  set_target_properties(Fifechan::opengl PROPERTIES
    INTERFACE_LINK_LIBRARIES "-L${FIFECHAN_BUILT_LIBRARY_DIR} -lfifechan_opengl"
  )
endif()

