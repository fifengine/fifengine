#------------------------------------------------------------------------------
# FindFifeChan.cmake
#------------------------------------------------------------------------------
#
# Find the native fifechan includes and libraries. Supports components.
#
# Usage: find_package(FifeChan COMPONENTS opengl sdl REQUIRED)
#
# FIFECHAN_INCLUDE_DIR        - where to find fifechan/fifechan.hüü, etc.
# FIFECHAN_LIBRARIES          - List of libraries when using fifechan.
# FIFECHAN_FOUND              - True, if fifechan found.
# 
# FIFECHAN_${component}_FOUND - True, if fifechan_component found.
#
#------------------------------------------------------------------------------
# Author: Jens A. Koch <jakoch@web.de>
#------------------------------------------------------------------------------

if(FIFECHAN_INCLUDE_DIR)
    # Already in cache, be silent
    set(FIFECHAN_FIND_QUIETLY TRUE)
endif(FIFECHAN_INCLUDE_DIR)

#------------------------------------------------------------------------------
#                       Find FIFECHAN_INCLUDE_DIR                                
#------------------------------------------------------------------------------

find_path(FIFECHAN_INCLUDE_DIR NAMES fifechan.hpp PATH_SUFFIXES fifechan)

#------------------------------------------------------------------------------
#                       Find FIFECHAN_LIBRARY                              
#------------------------------------------------------------------------------

# support alternative library names: suffix "_static" and prefix "lib"
find_library(FIFECHAN_LIBRARY NAMES fifechan fifechan_static libfifechan libfifechan_static)

#------------------------------------------------------------------------------
#                           Find libraries (components)
#
#   Example: find_package(FifeChan COMPONENTS opengl sdl REQUIRED)                                    
#------------------------------------------------------------------------------

IF(FifeChan_FIND_COMPONENTS)
  
  message(STATUS "Found the following FifeChan libraries:")

  FOREACH(component ${FifeChan_FIND_COMPONENTS})

    find_library(FIFECHAN_${component}_LIBRARY NAMES fifechan_${component})

    if(FIFECHAN_${component}_LIBRARY)        
        SET(FIFECHAN_${component}_FOUND 1)
        list(APPEND FIFECHAN_COMPONENTS "${FIFECHAN_${component}_LIBRARY}")
        message(STATUS "  fifechan_${component}")
    else()
        SET(FIFECHAN_${component}_FOUND 0)
        message(STATUS "  fifechan_${component} not found.")
    endif()

  ENDFOREACH()
ENDIF()

# - handle the QUIETLY and REQUIRED arguments
# - set FIFECHAN_FOUND to TRUE if all REQUIRED_VARS are TRUE
# - additionally handle components, which are found: FIFECHAN_${component}_FOUND
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FIFECHAN 
    REQUIRED_VARS FIFECHAN_LIBRARY FIFECHAN_INCLUDE_DIR 
    HANDLE_COMPONENTS
)

if(FIFECHAN_FOUND)
    set(FIFECHAN_LIBRARIES ${FIFECHAN_LIBRARY} ${FIFECHAN_COMPONENTS})
endif(FIFECHAN_FOUND)

# for debugging
#message(STATUS "FIFECHAN_INCLUDE_DIR  = '${FIFECHAN_INCLUDE_DIR}'")
#message(STATUS "FIFECHAN_LIBRARIES    = '${FIFECHAN_LIBRARIES}'")

mark_as_advanced(FIFECHAN_INCLUDE_DIR FIFECHAN_LIBRARY)
