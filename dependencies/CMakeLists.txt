#------------------------------------------------------------------------------
#                           Fifengine Dependencies
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.5)

project(fifengine-dependencies)

# Legacy fallback for standalone invocation. In the current explicit 4-step
# flow, callers usually pass explicit DEPENDENCY_* paths.
get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/includes" ABSOLUTE)
# Keep module includes local to this repository.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../cmake)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "default install path" FORCE)
endif()

message(STATUS "\n== Build System for ${PROJECT_NAME}\n")

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
  get_filename_component(DEPENDENCY_DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/downloads" ABSOLUTE)
  message(STATUS "DEPENDENCY_DOWNLOAD_DIR not set. Using default.")
endif(NOT DEPENDENCY_DOWNLOAD_DIR)

# ExternalProject installs (SWIG/Fifechan) are driven by DEPENDENCY_INSTALL_DIR.
if(NOT DEPENDENCY_INSTALL_DIR)
  get_filename_component(DEPENDENCY_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/includes" ABSOLUTE)
  message(STATUS "DEPENDENCY_INSTALL_DIR not set. Using default.")
endif(NOT DEPENDENCY_INSTALL_DIR)

# Fallback extraction folder for dependency source archives.
if(NOT DEPENDENCY_EXTRACT_DIR)
  get_filename_component(DEPENDENCY_EXTRACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/extracted" ABSOLUTE)
  message(STATUS "DEPENDENCY_EXTRACT_DIR not set. Using default.")
endif(NOT DEPENDENCY_EXTRACT_DIR)

#------------------------------------------------------------------------------
#                                 Display Status
#------------------------------------------------------------------------------

message("")
message("== Config Overview")
message("")
message("   CMAKE_BUILD_TYPE           |  ${CMAKE_BUILD_TYPE}")
message("   CMAKE_BINARY_DIR           |  ${CMAKE_BINARY_DIR}")
message("   CMAKE_INSTALL_PREFIX       |  ${CMAKE_INSTALL_PREFIX}")
message("   DEPENDENCY_INSTALL_DIR     |  ${DEPENDENCY_INSTALL_DIR}")
message("   DEPENDENCY_DOWNLOAD_DIR    |  ${DEPENDENCY_DOWNLOAD_DIR}")
message("   DEPENDENCY_EXTRACT_DIR     |  ${DEPENDENCY_EXTRACT_DIR}")
message("")

# Forward prefix paths to dependency external projects (e.g. fifechan).
# In the explicit 4-step flow this is provided by caller args
# (build.sh / CI workflow); CMAKE_PREFIX_PATH is only a fallback.
if(NOT DEFINED DEPENDENCY_CMAKE_PREFIX_PATH AND DEFINED CMAKE_PREFIX_PATH)
  set(DEPENDENCY_CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}")
endif()

if(DEFINED DEPENDENCY_CMAKE_PREFIX_PATH AND NOT "${DEPENDENCY_CMAKE_PREFIX_PATH}" STREQUAL "")
  message(STATUS "DEPENDENCY_CMAKE_PREFIX_PATH -> ${DEPENDENCY_CMAKE_PREFIX_PATH}")
else()
  message(STATUS "DEPENDENCY_CMAKE_PREFIX_PATH not set; relying on toolchain defaults")
endif()

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
