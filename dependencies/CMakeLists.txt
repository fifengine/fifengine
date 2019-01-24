#------------------------------------------------------------------------------
#                           Fifengine Dependencies                                               
#------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

PROJECT(fifengine-dependencies)

get_filename_component(
    CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/includes" 
    ABSOLUTE
)
set(CMAKE_MODULE_PATH       ${CMAKE_SOURCE_DIR}/../cmake)
set(CMAKE_VERBOSE_MAKEFILE  ON)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "default install path" FORCE )
endif()

message(STATUS "\n== Build System for ${PROJECT_NAME}\n")

# create a "Release" build by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Select build type. Options are: 'Debug', 'Release', 'RelWithDebInfo' and 'MinSizeRel'."
    FORCE
  )  
  message(STATUS "Set CMAKE_BUILD_TYPE to default '${CMAKE_BUILD_TYPE}'.")
endif(NOT CMAKE_BUILD_TYPE)

# set a fallback download folder for dependencies
if(NOT DEPENDENCY_DOWNLOAD_DIR)
    get_filename_component(
        DEPENDENCY_DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/downloads" 
        ABSOLUTE
    )
    message("   DEPENDENCY_DOWNLOAD_DIR not set. Using default.")
endif(NOT DEPENDENCY_DOWNLOAD_DIR)

# set a fallback installation folder for dependencies
if(NOT DEPENDENCY_INSTALL_DIR)
    get_filename_component(
        DEPENDENCY_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/includes" 
        ABSOLUTE
    )
    message("   DEPENDENCY_INSTALL_DIR not set. Using default.")    
endif(NOT DEPENDENCY_INSTALL_DIR)

# set a fallback folder for the extracted dependencies
if(NOT DEPENDENCY_EXTRACT_DIR)
    get_filename_component(
        DEPENDENCY_EXTRACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_NAME}/extracted"
        ABSOLUTE
    )
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

OPTION(FIFE_DEPENDENCY_GLEW          "Download and build dependency: GLEW"       ON)
# TODO boost
OPTION(FIFE_DEPENDENCY_ZLIB          "Download and build dependency: ZLIB"       ON)
OPTION(FIFE_DEPENDENCY_LIBPNG        "Download and build dependency: LIBPNG"     ON)
OPTION(FIFE_DEPENDENCY_OPENALSOFT    "Download and build dependency: OPENALSOFT" ON)
OPTION(FIFE_DEPENDENCY_SWIG          "Download and build dependency: SWIG"       ON)
OPTION(FIFE_DEPENDENCY_TINYXML       "Download and build dependency: TINYXML"    ON)
OPTION(FIFE_DEPENDENCY_LIBOGG        "Download and build dependency: LIBOGG"     ON)
OPTION(FIFE_DEPENDENCY_LIBVORBIS     "Download and build dependency: LIBVORBIS"  ON)
OPTION(FIFE_DEPENDENCY_FIFECHAN      "Download and build dependency: FIFECHAN"   ON)
OPTION(FIFE_DEPENDENCY_SDL2          "Download and build dependency: SDL2"       ON)
OPTION(FIFE_DEPENDENCY_SDL2_TTF      "Download and build dependency: SDL2_TTF"   ON)
OPTION(FIFE_DEPENDENCY_SDL2_IMAGE    "Download and build dependency: SDL2_IMAGE" ON)
OPTION(FIFE_DEPENDENCY_CEGUI         "Download and build dependency: CEGUI"      OFF)

#------------------------------------------------------------------------------
#                   Check Options + Include external build scripts                                             
#------------------------------------------------------------------------------

IF(FIFE_DEPENDENCY_GLEW)
    INCLUDE(get-glew)
ENDIF(FIFE_DEPENDENCY_GLEW)

# TODO: boost

IF(FIFE_DEPENDENCY_ZLIB)
    INCLUDE(get-zlib)
ENDIF(FIFE_DEPENDENCY_ZLIB)

IF(FIFE_DEPENDENCY_LIBPNG)
    INCLUDE(get-libpng)
ENDIF(FIFE_DEPENDENCY_LIBPNG)

IF(FIFE_DEPENDENCY_OPENALSOFT)
    INCLUDE(get-openalsoft)
ENDIF(FIFE_DEPENDENCY_OPENALSOFT)

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

IF(FIFE_DEPENDENCY_SDL2)
    INCLUDE(get-sdl2)
ENDIF(FIFE_DEPENDENCY_SDL2)

IF(FIFE_DEPENDENCY_SDL2_TTF)
    INCLUDE(get-sdl2_ttf)
ENDIF(FIFE_DEPENDENCY_SDL2_TTF)

IF(FIFE_DEPENDENCY_SDL2_IMAGE)
    INCLUDE(get-sdl2_image)
ENDIF(FIFE_DEPENDENCY_SDL2_IMAGE)

IF(FIFE_DEPENDENCY_CEGUI)
    INCLUDE(get-cegui)
ENDIF(FIFE_DEPENDENCY_CEGUI)
