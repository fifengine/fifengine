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

# get GLEW version from glew.h
if(GLEW_INCLUDE_DIR AND EXISTS "${GLEW_INCLUDE_DIR}/GL/glew.h")
  file(STRINGS "${GLEW_INCLUDE_DIR}/GL/glew.h" GLEW_VERSION_LINE REGEX "^VERSION [0-9].[0-9].[0-9]+$")        
  string(REPLACE "VERSION " "" GLEW_VERSION "${GLEW_VERSION_LINE}")
  unset(GLEW_VERSION_LINE)
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GLEW 
  REQUIRED_VARS GLEW_LIBRARY GLEW_INCLUDE_DIR
  VERSION_VAR GLEW_VERSION
)

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)