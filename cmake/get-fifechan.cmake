#------------------------------------------------------------------------------
# External Project:     FifeChan / FifeGUI
# Downloads:            https://github.com/fifengine/fifechan/releases
#------------------------------------------------------------------------------

include(ExternalProject)

option(FIFECHAN_BUILD_FROM_SOURCE "Build FifeChan from source instead of using pre-built binaries (Windows)" OFF)

#set(FIFECHAN_VERSION "0.1.5" CACHE STRING "The FifeChan version (can be: tag, branch, commit hash) to fetch")
set(FIFECHAN_VERSION "origin/main" CACHE STRING "The FifeChan version (can be: tag, branch, commit hash) to fetch")
set(FIFECHAN_GIT_REPO "https://github.com/fifengine/fifechan.git" CACHE STRING "FifeChan git repository URL")

# Required dependency paths (provided by dependencies/CMakeLists.txt)
if(NOT DEFINED DEPENDENCY_INSTALL_DIR)
  message(FATAL_ERROR "DEPENDENCY_INSTALL_DIR must be defined before including get-fifechan.cmake")
endif()
if(NOT DEFINED DEPENDENCY_EXTRACT_DIR)
  message(FATAL_ERROR "DEPENDENCY_EXTRACT_DIR must be defined before including get-fifechan.cmake")
endif()
if(NOT DEFINED DEPENDENCY_DOWNLOAD_DIR)
  message(FATAL_ERROR "DEPENDENCY_DOWNLOAD_DIR must be defined before including get-fifechan.cmake")
endif()

if(WIN32 AND MSVC AND NOT FIFECHAN_BUILD_FROM_SOURCE)

    # === On Windows download the pre-compiled development binaries ===

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

else()

  # === Build from source ===

  # Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    if(POLICY CMP0135)
      cmake_policy(SET CMP0135 NEW)
      set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
    endif()
  endif()

  set(_FIFECHAN_PREFIX_ARGS)

  # Prepare CMAKE_PREFIX_PATH arguments for ExternalProject; pass each entry
  # as its own -DCMAKE_PREFIX_PATH argument.
  if(DEFINED CMAKE_PREFIX_PATH)
  foreach(_p ${CMAKE_PREFIX_PATH})
    # Quote the entire -D argument to preserve paths with spaces
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

  # Build the main CMAKE_ARGS list
  set(FIFECHAN_CMAKE_ARGS)
  list(APPEND FIFECHAN_CMAKE_ARGS
    # Quote each entry: ensures values with spaces stay as single arguments
    "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
    "-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    "-DVCPKG_MANIFEST_MODE=ON"
    # disabled: we only need the lib
    "-DFIFEGUI_TESTS=OFF"
    # disabled: we only need the lib and this requires sdl_mixer
    "-DFIFEGUI_EXAMPLES=OFF"
    # append lists
    ${_FIFECHAN_VCPKG_ARGS}
    ${_FIFECHAN_PREFIX_ARGS}
  )

  if(NOT TARGET fifechan)
    ExternalProject_Add(
      fifechan
      PREFIX ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
      GIT_REPOSITORY ${FIFECHAN_GIT_REPO}
      GIT_TAG ${FIFECHAN_VERSION}  # can be: tag, branch, commit hash
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
      INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
      CMAKE_ARGS ${FIFECHAN_CMAKE_ARGS}
      UPDATE_DISCONNECTED TRUE  # avoid fetching on every configure
      UPDATE_COMMAND ""        # disable automatic git update/checkout during configure
    )
  endif()

  # Stable wrapper target for callers expecting an install-like target.
  if(NOT TARGET fifechan-install)
    add_custom_target(fifechan-install DEPENDS fifechan)
  endif()

endif()

# Export expected install locations and create imported targets
set(FIFECHAN_BUILT_INCLUDE_DIR "${DEPENDENCY_INSTALL_DIR}/include" CACHE PATH "FifeChan include dir from get-fifechan")
set(FIFECHAN_BUILT_LIBRARY_DIR "${DEPENDENCY_INSTALL_DIR}/lib" CACHE PATH "FifeChan lib dir from get-fifechan")

if(NOT TARGET Fifechan::fifechan)
  add_library(Fifechan::fifechan UNKNOWN IMPORTED)
  set_target_properties(Fifechan::fifechan PROPERTIES
    INTERFACE_LINK_LIBRARIES "-L${FIFECHAN_BUILT_LIBRARY_DIR} -lfifechan"
    INTERFACE_INCLUDE_DIRECTORIES "${FIFECHAN_BUILT_INCLUDE_DIR}"
  )
endif()

if(NOT TARGET Fifechan::sdl)
  add_library(Fifechan::sdl UNKNOWN IMPORTED)
  set_target_properties(Fifechan::sdl PROPERTIES
    INTERFACE_LINK_LIBRARIES "-L${FIFECHAN_BUILT_LIBRARY_DIR} -lfifechan_sdl"
    INTERFACE_INCLUDE_DIRECTORIES "${FIFECHAN_BUILT_INCLUDE_DIR}"
  )
endif()

if(NOT TARGET Fifechan::opengl)
  add_library(Fifechan::opengl UNKNOWN IMPORTED)
  set_target_properties(Fifechan::opengl PROPERTIES
    INTERFACE_LINK_LIBRARIES "-L${FIFECHAN_BUILT_LIBRARY_DIR} -lfifechan_opengl"
    INTERFACE_INCLUDE_DIRECTORIES "${FIFECHAN_BUILT_INCLUDE_DIR}"
  )
endif()
