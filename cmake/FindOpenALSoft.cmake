# - Try to find OpenAL-Soft
# and define
#
#  OPENALSOFT_FOUND          - System has OpenAL-Soft
#  OPENALSOFT_INCLUDE_DIR    - The OpenAL-Soft include directories
#  OPENALSOFT_LIBRARY        - The OpenAL-Soft libraries

find_path(OPENALSOFT_INCLUDE_DIR
  NAMES al.h
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
    # Travis CI - MAC OSX
    /usr/local/opt/openal-soft/
    # Linux Include PATH
    /usr/include/AL/
)

find_library(OPENALSOFT_LIBRARY
  NAMES libopenal openal libOpenAL32 OpenAL32 OpenAL32.dll soft_oal.dll
  PATH_SUFFIXES lib64 lib libs64 libs
  PATHS
    ${OPENALSOFT_INCLUDE_DIR}/../lib
    lib
    /usr/lib
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OPENALSOFT DEFAULT_MSG OPENALSOFT_LIBRARY OPENALSOFT_INCLUDE_DIR)

mark_as_advanced(OPENALSOFT_INCLUDE_DIR OPENALSOFT_LIBRARY)
