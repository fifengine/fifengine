#------------------------------------------------------------------------------
# External Project:     FifeChan / FifeGUI
# Downloads:            https://github.com/fifengine/fifechan/releases
#------------------------------------------------------------------------------

include(ExternalProject)

# For development use "orgin/main" to always get the latest code.
# For release use the specific tag, e.g. "v0.2.0".
set(FIFECHAN_VERSION "v0.2.0" CACHE STRING "The FifeChan version (can be: tag, branch, commit hash) to fetch")

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

if(WIN32 AND MSVC AND NOT FIFE_DEPENDENCY_BUILD_FROM_SOURCE)

    # === On Windows download the pre-compiled development binaries ===

    # ExternalProject_Add(
    #   fifechan
    #   PREFIX ${DEPENDENCY_EXTRACT_DIR}
    #   DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    #   URL https://github.com/fifengine/fifechan/releases/download/0.1.5/fifechan-shared-dev-f848a38-VC15-x86.zip
    #   URL_MD5 4459fb7da09f3b26fa1a4116710ffa1d
    #   CONFIGURE_COMMAND ""
    #   BUILD_COMMAND ""
    #   INSTALL_COMMAND
    #   COMMAND
    #     ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/bin ${DEPENDENCY_INSTALL_DIR}/bin
    #   COMMAND
    #     ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/lib ${DEPENDENCY_INSTALL_DIR}/lib
    #   COMMAND
    #     ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/include
    #     ${DEPENDENCY_INSTALL_DIR}/include/fifechan
    # )

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
  # add the Vcpkg include directory to CMAKE_PREFIX_PATH so <utf8cpp/utf8.h> can be found
  if(DEFINED VCPKG_INSTALLED_DIR)
    list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
  endif()
  # Prepare CMAKE_PREFIX_PATH arguments for ExternalProject
  if(DEFINED CMAKE_PREFIX_PATH)
    # Quote the entire -D argument to preserve paths with spaces
    foreach(_p ${CMAKE_PREFIX_PATH})
      list(APPEND _FIFECHAN_PREFIX_ARGS "-DCMAKE_PREFIX_PATH:PATH=${_p}")
    endforeach()
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
    # explicitly build as shared library
    "-DBUILD_SHARED_LIBS=ON"
    # disabled: we do this manually in the CI workflow
    "-DVCPKG_MANIFEST_MODE=OFF"
    # disabled: we only need the lib
    "-DFIFEGUI_TESTS=OFF"
    # disabled: we only need the lib and this requires sdl_mixer
    "-DFIFEGUI_EXAMPLES=OFF"
    # explicitly enable OpenGL (defaults to ON but be explicit)
    "-DENABLE_OPENGL=ON"
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
      GIT_PROGRESS FALSE
      INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
      CMAKE_ARGS ${FIFECHAN_CMAKE_ARGS}
    )
  endif()

  # stable alias for the install step
  if(NOT TARGET fifechan-install)
    add_custom_target(fifechan-install DEPENDS fifechan)
  endif()

endif()
