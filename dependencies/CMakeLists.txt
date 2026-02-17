#------------------------------------------------------------------------------
#                           Fifengine Dependencies
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(fife-dependencies)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

# Canonical output root for dependency artifacts.
set(
  FIFE_DEPENDENCIES_ROOT
  "${CMAKE_CURRENT_SOURCE_DIR}/../out/fife-dependencies"
  CACHE PATH "Root directory for dependency artifacts (install/downloads/extracted/build-tools)"
)
get_filename_component(FIFE_DEPENDENCIES_ROOT "${FIFE_DEPENDENCIES_ROOT}" ABSOLUTE)
# Keep module includes local to this repository.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../cmake)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${FIFE_DEPENDENCIES_ROOT}/install" CACHE PATH "default install path" FORCE)
endif()

# create a "Release" build by default
if(NOT CMAKE_BUILD_TYPE)
  set(
    CMAKE_BUILD_TYPE
    "Release"
    CACHE STRING
    "Select build type. Options are: 'Debug', 'Release', 'RelWithDebInfo' and 'MinSizeRel'."
    FORCE
  )
  message(STATUS "Set CMAKE_BUILD_TYPE to default '${CMAKE_BUILD_TYPE}'.")
endif(NOT CMAKE_BUILD_TYPE)

# Fallback locations used when the caller does not provide DEPENDENCY_*.
# In the explicit workflow/build.sh flow these are usually passed in.
if(NOT DEPENDENCY_DOWNLOAD_DIR)
  set(DEPENDENCY_DOWNLOAD_DIR "${FIFE_DEPENDENCIES_ROOT}/downloads")
  message(STATUS "DEPENDENCY_DOWNLOAD_DIR not set. Using default.")
endif(NOT DEPENDENCY_DOWNLOAD_DIR)

# ExternalProject installs (SWIG/Fifechan) are driven by DEPENDENCY_INSTALL_DIR.
if(NOT DEPENDENCY_INSTALL_DIR)
  set(DEPENDENCY_INSTALL_DIR "${FIFE_DEPENDENCIES_ROOT}/install")
  message(STATUS "DEPENDENCY_INSTALL_DIR not set. Using default.")
endif(NOT DEPENDENCY_INSTALL_DIR)

# Fallback extraction folder for dependency source archives.
if(NOT DEPENDENCY_EXTRACT_DIR)
  set(DEPENDENCY_EXTRACT_DIR "${FIFE_DEPENDENCIES_ROOT}/extracted")
  message(STATUS "DEPENDENCY_EXTRACT_DIR not set. Using default.")
endif(NOT DEPENDENCY_EXTRACT_DIR)

#------------------------------------------------------------------------------
#                                 Display Status
#------------------------------------------------------------------------------

message(STATUS "== Build System for ${PROJECT_NAME}")
message(STATUS "")
message(STATUS "== Config Overview")
message(STATUS "")
message(STATUS "   CMAKE_BUILD_TYPE           |  ${CMAKE_BUILD_TYPE}")
message(STATUS "   CMAKE_BINARY_DIR           |  ${CMAKE_BINARY_DIR}")
message(STATUS "   CMAKE_INSTALL_PREFIX       |  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "   DEPENDENCY_INSTALL_DIR     |  ${DEPENDENCY_INSTALL_DIR}")
message(STATUS "   DEPENDENCY_DOWNLOAD_DIR    |  ${DEPENDENCY_DOWNLOAD_DIR}")
message(STATUS "   DEPENDENCY_EXTRACT_DIR     |  ${DEPENDENCY_EXTRACT_DIR}")
message(STATUS "")

#------------------------------------------------------------------------------
#                                Options
#------------------------------------------------------------------------------

option(FIFE_DEPENDENCY_SWIG "Download and build dependency: SWIG" ON)
option(FIFE_DEPENDENCY_FIFECHAN "Download and build dependency: FIFECHAN" ON)

#------------------------------------------------------------------------------
#                   Check Options + Include external dependency scripts
#------------------------------------------------------------------------------

if(FIFE_DEPENDENCY_SWIG)
  include(get-swig)
endif(FIFE_DEPENDENCY_SWIG)

if(FIFE_DEPENDENCY_FIFECHAN)
  include(get-fifechan)
endif(FIFE_DEPENDENCY_FIFECHAN)
