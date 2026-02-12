#------------------------------------------------------------------------------
#                           Fifengine Dependencies
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.5)

project(fifengine-dependencies)

get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/includes" ABSOLUTE)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../cmake)
set(CMAKE_VERBOSE_MAKEFILE ON)

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

# set a fallback download folder for dependencies
if(NOT DEPENDENCY_DOWNLOAD_DIR)
  get_filename_component(DEPENDENCY_DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/downloads" ABSOLUTE)
  message("   DEPENDENCY_DOWNLOAD_DIR not set. Using default.")
endif(NOT DEPENDENCY_DOWNLOAD_DIR)

# set a fallback installation folder for dependencies
if(NOT DEPENDENCY_INSTALL_DIR)
  get_filename_component(DEPENDENCY_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/includes" ABSOLUTE)
  message("   DEPENDENCY_INSTALL_DIR not set. Using default.")
endif(NOT DEPENDENCY_INSTALL_DIR)

# set a fallback folder for the extracted dependencies
if(NOT DEPENDENCY_EXTRACT_DIR)
  get_filename_component(DEPENDENCY_EXTRACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/extracted" ABSOLUTE)
  message("   DEPENDENCY_EXTRACT_DIR not set. Using default.")
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

#------------------------------------------------------------------------------
#                                Options
#------------------------------------------------------------------------------

option(FIFE_DEPENDENCY_SWIG "Download and build dependency: SWIG" ON)
option(FIFE_DEPENDENCY_FIFECHAN "Download and build dependency: FIFECHAN" ON)

#------------------------------------------------------------------------------
#                   Check Options + Include external build scripts
#------------------------------------------------------------------------------

if(FIFE_DEPENDENCY_SWIG)
  include(get-swig)
endif(FIFE_DEPENDENCY_SWIG)

if(FIFE_DEPENDENCY_FIFECHAN)
  include(get-fifechan)
endif(FIFE_DEPENDENCY_FIFECHAN)
