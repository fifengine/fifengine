# - Try to find VORBIS
# and define
#
#  VORBIS_FOUND        - System has VORBIS
#  VORBIS_INCLUDE_DIRS - The include directories
#  VORBIS_LIBRARY      - The libraries
#  VORBISFILE_LIBRARY  - The vorbisfile

# try to find package and provide a hint
find_package(PkgConfig QUIET)
pkg_check_modules(PC_VORBISFILE QUIET vorbisfile)
find_path(VORBIS_INCLUDE_DIRS NAMES vorbis/vorbisfile.h HINTS ${PC_VORBIS_INCLUDE_DIRS} PATH_SUFFIXES vorbis)
find_library(VORBIS_LIBRARY NAMES vorbis HINTS ${PC_VORBIS_LIBRARY_DIRS})

# Find vorbisfile dependency
find_package(PkgConfig QUIET)
pkg_check_modules(PC_VORBISFILE QUIET vorbisfile)
find_library(VORBISFILE_LIBRARY NAMES vorbisfile HINTS ${PC_VORBIS_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(vorbis DEFAULT_MSG VORBIS_LIBRARY VORBIS_INCLUDE_DIRS)
find_package_handle_standard_args(vorbisfile DEFAULT_MSG VORBISFILE_LIBRARY VORBIS_INCLUDE_DIRS)