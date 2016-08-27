#------------------------------------------------------------------------------
#                           Fifengine Dependencies                                               
#------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

PROJECT(fifengine-dependencies)

set(CMAKE_INSTALL_PREFIX    ${CMAKE_SOURCE_DIR}/../../${PROJECT_NAME}/includes)
set(CMAKE_MODULE_PATH       ${CMAKE_SOURCE_DIR}/../cmake)
set(CMAKE_VERBOSE_MAKEFILE  ON)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "default install path" FORCE )
endif()

message(STATUS "Welcome!\n== Build System for ${PROJECT_NAME}")

# create a "Release" build by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Select build type. Options are: 'Debug', 'Release', 'RelWithDebInfo' and 'MinSizeRel'."
    FORCE
  )
  message(STATUS "Set CMAKE_BUILD_TYPE to default '${CMAKE_BUILD_TYPE}'.")
endif(NOT CMAKE_BUILD_TYPE)

# set a fallback download folder for dependencies ("dependencies/../../${PROJECT_NAME}/downloads")
if(NOT DEPENDENCY_DOWNLOAD_DIR)
    set(DEPENDENCY_DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/downloads)
    message("   DEPENDENCY_DOWNLOAD_DIR not set. Using default.")
endif(NOT DEPENDENCY_DOWNLOAD_DIR)

# set a fallback installation folder for dependencies ("dependencies/../../${PROJECT_NAME}/includes")
if(NOT DEPENDENCY_INSTALL_DIR)
    set(DEPENDENCY_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/includes)
    message("   DEPENDENCY_INSTALL_DIR not set. Using default.")    
endif(NOT DEPENDENCY_INSTALL_DIR)

# set a fallback folder for the extracted dependencies ("dependencies/../../${PROJECT_NAME}/extracted")
if(NOT DEPENDENCY_EXTRACT_DIR)
    set(DEPENDENCY_EXTRACT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/extracted)
    message("   DEPENDENCY_EXTRACT_DIR not set. Using default.")    
endif(NOT DEPENDENCY_EXTRACT_DIR)

#------------------------------------------------------------------------------
#                                 Display Status                                         
#------------------------------------------------------------------------------

message("")
message("== Config Overview")
message("   CMAKE_BINARY_DIR         |  ${CMAKE_BINARY_DIR}")
message("   CMAKE_INSTALL_PREFIX     |  ${CMAKE_INSTALL_PREFIX}")
message("   DEPENDENCY_INSTALL_DIR   |  ${DEPENDENCY_INSTALL_DIR}") 
message("   DEPENDENCY_DOWNLOAD_DIR  |  ${DEPENDENCY_DOWNLOAD_DIR}") 
message("   DEPENDENCY_EXTRACT_DIR   |  ${DEPENDENCY_EXTRACT_DIR}") 
message("")

#------------------------------------------------------------------------------
#                                Options                                                   
#------------------------------------------------------------------------------

OPTION(FIFE_DEPENDENCY_SDL2          "Download and build dependency: SDL2"       ON)
OPTION(FIFE_DEPENDENCY_SDL2_image    "Download and build dependency: SDL2_image" OFF)
OPTION(FIFE_DEPENDENCY_SDL2_ttf      "Download and build dependency: SDL2_ttf"   OFF)
OPTION(FIFE_DEPENDENCY_LIBPNG        "Download and build dependency: LIBPNG"     OFF)
OPTION(FIFE_DEPENDENCY_OPENAL        "Download and build dependency: OPENAL"     OFF)
OPTION(FIFE_DEPENDENCY_OGGVORBIS     "Download and build dependency: OGGVORBIS"  OFF)

#------------------------------------------------------------------------------
#                   Check Options + Include external build scripts                                             
#------------------------------------------------------------------------------

IF(FIFE_DEPENDENCY_SDL2)
    INCLUDE(get-SDL2)
ENDIF(FIFE_DEPENDENCY_SDL2)

IF(FIFE_DEPENDENCY_SDL2_IMAGE)
    INCLUDE(get-SDL2_IMAGE)
ENDIF(FIFE_DEPENDENCY_SDL2_IMAGE)

IF(FIFE_DEPENDENCY_SDL2_TTF)
    INCLUDE(get-SDL2_TTF)
ENDIF(FIFE_DEPENDENCY_SDL2_TTF)

IF(FIFE_DEPENDENCY_OGGVORBIS)
    INCLUDE(get-OGGVORBIS)
ENDIF(FIFE_DEPENDENCY_OGGVORBIS)

IF(FIFE_DEPENDENCY_LIBPNG)
    INCLUDE(get-LIBPNG)
ENDIF(FIFE_DEPENDENCY_LIBPNG)

IF(FIFE_DEPENDENCY_OPENAL)
    INCLUDE(get-OPENAL)
ENDIF(FIFE_DEPENDENCY_OPENAL)
