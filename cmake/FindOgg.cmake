# - Try to find OGG
# and define
#
#  OGG_FOUND        - System has OGG
#  OGG_INCLUDE_DIRS - The include directories
#  OGG_LIBRARY      - The libraries

# try to find package and provide a hint
find_package(PkgConfig QUIET)
pkg_check_modules(PC_OGG QUIET ogg)

find_path(OGG_INCLUDE_DIR 
    NAMES ogg/ogg.h 
    HINTS ${PC_OGG_INCLUDE_DIRS} 
    PATH_SUFFIXES ogg
)

find_library(OGG_LIBRARY 
    NAMES ogg libogg
    HINTS ${PC_OGG_LIBRARY_DIRS}
    PATH ${OGG_INCLUDE_DIR}/../lib lib
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OGG DEFAULT_MSG OGG_LIBRARY OGG_INCLUDE_DIR)

mark_as_advanced(OGG_LIBRARY OGG_INCLUDE_DIR)
