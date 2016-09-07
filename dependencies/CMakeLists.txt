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

message(STATUS "\nWelcome!\n== Build System for ${PROJECT_NAME}")

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

OPTION(FIFE_DEPENDENCY_SDL2          "Download and build dependency: SDL2"       ON)
OPTION(FIFE_DEPENDENCY_SDL2_IMAGE    "Download and build dependency: SDL2_IMAGE" ON)
OPTION(FIFE_DEPENDENCY_SDL2_TTF      "Download and build dependency: SDL2_TTF"   ON)
# TODO boost
OPTION(FIFE_DEPENDENCY_ZLIB          "Download and build dependency: ZLIB"       ON)
OPTION(FIFE_DEPENDENCY_LIBPNG        "Download and build dependency: LIBPNG"     ON)
OPTION(FIFE_DEPENDENCY_OPENAL        "Download and build dependency: OPENAL"     ON)
OPTION(FIFE_DEPENDENCY_SWIG          "Download and build dependency: SWIG"       ON)
OPTION(FIFE_DEPENDENCY_TINYXML       "Download and build dependency: TINYXML"    ON)
OPTION(FIFE_DEPENDENCY_LIBOGG        "Download and build dependency: LIBOGG"     ON)
OPTION(FIFE_DEPENDENCY_LIBVORBIS     "Download and build dependency: LIBVORBIS"  ON)
OPTION(FIFE_DEPENDENCY_FIFECHAN      "Download and build dependency: FIFECHAN"   ON)

#------------------------------------------------------------------------------
#                   Check Options + Include external build scripts                                             
#------------------------------------------------------------------------------

IF(FIFE_DEPENDENCY_SDL2)
    INCLUDE(get-sdl2)
ENDIF(FIFE_DEPENDENCY_SDL2)

IF(FIFE_DEPENDENCY_SDL2_IMAGE)
    INCLUDE(get-sdl2_image)
ENDIF(FIFE_DEPENDENCY_SDL2_IMAGE)

IF(FIFE_DEPENDENCY_SDL2_TTF)
    INCLUDE(get-sdl2_ttf)
ENDIF(FIFE_DEPENDENCY_SDL2_TTF)

# TODO: boost

IF(FIFE_DEPENDENCY_ZLIB)
    INCLUDE(get-zlib)
ENDIF(FIFE_DEPENDENCY_ZLIB)

IF(FIFE_DEPENDENCY_LIBPNG)
    INCLUDE(get-libpng)
ENDIF(FIFE_DEPENDENCY_LIBPNG)

IF(FIFE_DEPENDENCY_OPENAL)
    INCLUDE(get-openal)
ENDIF(FIFE_DEPENDENCY_OPENAL)

IF(FIFE_DEPENDENCY_SWIG)
    INCLUDE(get-swig)
ENDIF(FIFE_DEPENDENCY_SWIG)

IF(FIFE_DEPENDENCY_TINYXML)
    INCLUDE(get-tinyxml)
ENDIF(FIFE_DEPENDENCY_TINYXML)

IF(FIFE_DEPENDENCY_LIBOGG)
    INCLUDE(get-libogg)
ENDIF(FIFE_DEPENDENCY_LIBOGG)

IF(FIFE_DEPENDENCY_LIBVORBIS)
    INCLUDE(get-libvorbis)
ENDIF(FIFE_DEPENDENCY_LIBVORBIS)

IF(FIFE_DEPENDENCY_FIFECHAN)
    INCLUDE(get-fifechan)
ENDIF(FIFE_DEPENDENCY_FIFECHAN)
