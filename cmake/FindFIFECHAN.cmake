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
# Usage
#
# find_package(FifeChan COMPONENTS opengl sdl REQUIRED)
# target_link_libraries(myapp Fifechan::fifechan Fifechan::sdl Fifechan::opengl)
#
#------------------------------------------------------------------------------
# Author: Jens A. Koch <jakoch@web.de>
#------------------------------------------------------------------------------

find_path(FIFECHAN_INCLUDE_DIR
  NAMES fifechan.hpp
  PATH_SUFFIXES fifechan
)

find_library(FIFECHAN_LIBRARY
  NAMES fifechan fifechan_static libfifechan libfifechan_static
)

# Handle components: sdl, opengl
if(FIFECHAN_FIND_COMPONENTS)
  foreach(component IN LISTS FIFECHAN_FIND_COMPONENTS)
    string(TOLOWER "${component}" component_lower)

    if(component_lower STREQUAL "sdl")
      find_library(FIFECHAN_sdl_LIBRARY
        NAMES fifechan_sdl fifechan libfifechan
      )
    elseif(component_lower STREQUAL "opengl")
      find_library(FIFECHAN_opengl_LIBRARY
        NAMES fifechan_opengl fifechan_opengl_static libfifechan_opengl
      )
    else()
      find_library(FIFECHAN_${component_lower}_LIBRARY
        NAMES fifechan_${component_lower}
      )
    endif()

    if(FIFECHAN_${component_lower}_LIBRARY)
      set(FIFECHAN_${component_lower}_FOUND TRUE)
      list(APPEND FIFECHAN_COMPONENTS "${FIFECHAN_${component_lower}_LIBRARY}")
    endif()
  endforeach()
endif()

# Standard package handling
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FIFECHAN
  REQUIRED_VARS FIFECHAN_INCLUDE_DIR FIFECHAN_LIBRARY
  HANDLE_COMPONENTS
)

# Create imported targets
if(FIFECHAN_FOUND AND NOT TARGET Fifechan::fifechan)
  add_library(Fifechan::fifechan UNKNOWN IMPORTED)
  if(EXISTS "${FIFECHAN_LIBRARY}")
    set_target_properties(Fifechan::fifechan PROPERTIES
      IMPORTED_LOCATION "${FIFECHAN_LIBRARY}"
    )
  else()
    if(DEFINED FIFECHAN_BUILT_LIBRARY_DIR)
      set_target_properties(Fifechan::fifechan PROPERTIES
        INTERFACE_LINK_LIBRARIES "-L${FIFECHAN_BUILT_LIBRARY_DIR} -lfifechan"
      )
    endif()
  endif()
  if(EXISTS "${FIFECHAN_INCLUDE_DIR}")
    set_target_properties(Fifechan::fifechan PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${FIFECHAN_INCLUDE_DIR}"
    )
  endif()

  set(FIFECHAN_LIBRARIES Fifechan::fifechan)

  foreach(component IN LISTS FIFECHAN_FIND_COMPONENTS)
    string(TOLOWER "${component}" component_lower)
    if(TARGET Fifechan::${component_lower})
      continue()
    endif()

    if(FIFECHAN_${component_lower}_FOUND)
      add_library(Fifechan::${component_lower} UNKNOWN IMPORTED)
      if(EXISTS "${FIFECHAN_${component_lower}_LIBRARY}")
        set_target_properties(Fifechan::${component_lower} PROPERTIES
          IMPORTED_LOCATION "${FIFECHAN_${component_lower}_LIBRARY}"
        )
      else()
        if(DEFINED FIFECHAN_BUILT_LIBRARY_DIR)
          set_target_properties(Fifechan::${component_lower} PROPERTIES
            INTERFACE_LINK_LIBRARIES "-L${FIFECHAN_BUILT_LIBRARY_DIR} -lfifechan_${component_lower}"
          )
        endif()
      endif()
      if(EXISTS "${FIFECHAN_INCLUDE_DIR}")
        set_target_properties(Fifechan::${component_lower} PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES "${FIFECHAN_INCLUDE_DIR}"
        )
      endif()
      list(APPEND FIFECHAN_LIBRARIES Fifechan::${component_lower})
    endif()
  endforeach()
endif()

#message(STATUS "FIFECHAN_INCLUDE_DIR  = '${FIFECHAN_INCLUDE_DIR}'")
#message(STATUS "FIFECHAN_LIBRARIES    = '${FIFECHAN_LIBRARIES}'")

mark_as_advanced(FIFECHAN_INCLUDE_DIR FIFECHAN_LIBRARY)
