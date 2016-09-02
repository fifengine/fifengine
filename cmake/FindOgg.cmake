cmake_minimum_required(VERSION 2.8)

find_path(OGG_INCLUDE_DIR ogg/ogg.h)

set(OGG_NAMES ${OGG_NAMES} ogg libogg)

find_library(OGG_LIBRARY NAMES ${OGG_NAMES} PATH)

if(OGG_INCLUDE_DIR AND OGG_LIBRARY)
    set(OGG_FOUND TRUE)
endif()

if(OGG_FOUND)
    if(NOT OGG_FIND_QUIETLY)
        message(STATUS "Found Ogg: ${OGG_LIBRARY}")
    endif()
else(OGG_FOUND)
    if(OGG_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find ogg")
    endif()
endif()