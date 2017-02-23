# - Try to find GLEW
# and define
#
#  GLEW_FOUND          - System has GLEW
#  GLEW_INCLUDE_DIR    - The GLEW include directories
#  GLEW_LIBRARIES      - The GLEW libraries

find_path(GLEW_INCLUDE_DIR NAMES GL/glew.h)

# glew32d glew32sd 
find_library(GLEW_LIBRARY 
    NAMES GLEW glew32 glew32s 
    PATHS ${GLEW_INCLUDE_DIR}/../lib lib
)

if(GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
    set(GLEW_FOUND TRUE)
endif()

if(GLEW_FOUND)
    if(NOT GLEW_FIND_QUIETLY)
        message(STATUS "  GLEW:  ${GLEW_LIBRARY}")
        message(STATUS "  GLEW INCLUDE:  ${GLEW_INCLUDE_DIR}")
    endif()
else(GLEW_FOUND)
    if(GLEW_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find GLEW.")
    endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GLEW REQUIRED_VARS GLEW_INCLUDE_DIR GLEW_LIBRARY)

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)