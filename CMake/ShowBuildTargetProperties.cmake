#
# ShowBuildTargetProperties.cmake
# Copyright 2024 Jens A. Koch.
# SPDX-License-Identifier: MIT
# This file is part of fifeengine/fifechan.
#

#
# Show Build Target Properties
#
# Usage:
# add_executable(my_exe WIN32 ${APP_SOURCES})
# show_build_target_properties(my_exe)
#

function(show_build_target_property target property)
  if(NOT TARGET ${target})
    message("There is no target named '${target}'.")
    return()
  endif()

  get_target_property(values ${target} ${property})
  if(values)
    # split INCLUDE_DIRECTORIES into multiple lines for better readability
    if(property STREQUAL "INCLUDE_DIRECTORIES")
        message(STATUS "[${target}] ${property}:")
        foreach(dir IN LISTS values)
            message(STATUS "[${target}]  · ${dir}")
        endforeach()
        # split LINK_LIBRARIES into multiple lines for better readability
    elseif(property STREQUAL "LINK_LIBRARIES")
        message(STATUS "[${target}] ${property}:")
        foreach(lib IN LISTS values)
            message(STATUS "[${target}]  · ${lib}")
        endforeach()
    else()
        # all other properties are printed as a single line
        message(STATUS "[${target}] ${property} -> '${values}'")
    endif()
  endif()
endfunction()

function(show_build_target_properties target)
  message(STATUS "")
  message(STATUS "[INFO] Properties of Build Target '${target}':")
  set(properties
    SOURCE_DIR
    BINARY_DIR
    INSTALL_DIR
    INCLUDE_DIRECTORIES
    LINK_LIBRARIES
    LINK_FLAGS
    COMPILE_OPTIONS
    COMPILE_DEFINITIONS
    CMAKE_EXE_LINKER_FLAGS
  )
  foreach (prop ${properties})
    show_build_target_property(${target} ${prop})
  endforeach()
endfunction()
