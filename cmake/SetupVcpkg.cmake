#
# SetupVcpkg
#
# This CMake script configures vcpkg using env variables instead of CLI options.
#

set(VCPKG_VERBOSE ON)

if(DEFINED ENV{VCPKG_VERBOSE} AND NOT DEFINED VCPKG_VERBOSE)
    set(VCPKG_VERBOSE "$ENV{VCPKG_VERBOSE}" CACHE BOOL "")
endif()

#
# Automatic installation of vcpkg dependencies.
# Still experimental?
#
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

# Copy dependencies into the output directory for executables.
# if(DEFINED ENV{VCPKG_APPLOCAL_DEPS} AND NOT DEFINED VCPKG_APPLOCAL_DEPS)
#     set(VCPKG_APPLOCAL_DEPS "$ENV{VCPKG_APPLOCAL_DEPS}" CACHE BOOL "")
# endif()

# # Copy dependencies into the install target directory for executables.
# if(DEFINED ENV{X_VCPKG_APPLOCAL_DEPS_INSTALL} AND NOT DEFINED VCPKG_APPLOCAL_DEPS)
#     # X_VCPKG_APPLOCAL_DEPS_INSTALL depends on CMake policy CMP0087
#     if(POLICY CMP0087)
#         cmake_policy(SET CMP0087 NEW)
#     endif()
#     set(X_VCPKG_APPLOCAL_DEPS_INSTALL "$ENV{X_VCPKG_APPLOCAL_DEPS_INSTALL}" CACHE BOOL "")
# endif()

#
# VCPKG_ROOT
#
# Please set VCPKG_ROOT on your env: export VCPKG_ROOT=/opt/vcpkg/bin
#
# If CMAKE_TOOLCHAIN_FILE is:
# - is set, we don't change it. This is the default case!
# - not set, but VCPKG_ROOT, we set the toolchain using the VCPKG_ROOT,
#   This avoids passing -DCMAKE_TOOLCHAIN_FILE. This is way shorter!

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    # do nothing, CMAKE_TOOLCHAIN_FILE is already set
elseif(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
else()
    message(FATAL_ERROR "One of -DCMAKE_TOOLCHAIN_FILE or the VCPKG_ROOT environment variable must be set.")
endif()

#
# VCPKG_FEATURE_FLAGS
#
# This env var can be set to a comma-separated list of off-by-default features in vcpkg.
#
if(NOT DEFINED VCPKG_FEATURE_FLAGS)
    set(VCPKG_FEATURE_FLAGS "manifests,versions,binarycaching,registries")
endif()

#
# VCPKG_TARGET_TRIPLET
#
# A triplet defines the build target environment in a compact string.
# [target-architecture]-[platform]-[linkage type]
# Examples: x86-windows, x64-windows-static, x64-linux.
#
# https://vcpkg.readthedocs.io/en/latest/users/triplets/
#
# The VCPKG_DEFAULT_TRIPLET is automatically set by vcpkg.cmake.
# The default triplets are: Windows: x86-windows, Linux: x64-linux, OSX: x64-osx.
#
# If you want to build for other platforms, e.g. build for Linux on Windows-x64 (canadian-cross builds)
# please set VCPKG_TARGET_TRIPLET as env var: export VCPKG_TARGET_TRIPLET=x64-linux
#
if(DEFINED ENV{VCPKG_TARGET_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_TARGET_TRIPLET}" CACHE STRING "")
endif()

#
# VCPKG_DEFAULT_TRIPLET
#
if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

iF(NOT DEFINED VCPKG_MANIFEST_FILE)
    set(VCPKG_MANIFEST_FILE "${CMAKE_SOURCE_DIR}/vcpkg.json")
endif()

# Define an additional source group for IDEs with vcpkg relevant files.
source_group("vcpkg" FILES
    "${CMAKE_SOURCE_DIR}/cmake/SetupVcpkg.cmake"
    "${CMAKE_SOURCE_DIR}/vcpkg.json"
)

#
# Check to make sure the VCPKG_TARGET_TRIPLET matches BUILD_SHARED_LIBS
#
if (DEFINED VCPKG_TARGET_TRIPLET AND "${VCPKG_TARGET_TRIPLET}" MATCHES ".*-static")
    if (BUILD_SHARED_LIBS)
        message(FATAL_ERROR "When the VCPKG_TARGET_TRIPLET ends with '-static' the BUILD_SHARED_LIBS must be 'OFF'.")
    endif()
else()
    if (NOT BUILD_SHARED_LIBS)
        # if VCPKG_TARGET_TRIPLET does not end with '-static' then set BUILD_SHARED_LIBS to 'ON' by default
        set(BUILD_SHARED_LIBS ON)
    endif()
endif()


#
# Print VCPKG configuration overview
#
message(STATUS "")
message(STATUS "[VCPKG]  Configuration Overview:")
message(STATUS "")
message(STATUS "[INFO]   ENV.VCPKG_ROOT                -> '$ENV{VCPKG_ROOT}'")
message(STATUS "[INFO]   BUILD_SHARED_LIBS             -> '${BUILD_SHARED_LIBS}'")
message(STATUS "[INFO]   CMAKE_TOOLCHAIN_FILE          -> '${CMAKE_TOOLCHAIN_FILE}'")

message(STATUS "")
message(STATUS "[VCPKG]  VCPKG_VERBOSE                 -> '${VCPKG_VERBOSE}'")
message(STATUS "[VCPKG]  VCPKG_INSTALL_OPTIONS         -> '${VCPKG_INSTALL_OPTIONS}'")
message(STATUS "[VCPKG]  VCPKG_APPLOCAL_DEPS           -> '${VCPKG_APPLOCAL_DEPS}'")
message(STATUS "[VCPKG]  VCPKG_FEATURE_FLAGS           -> '${VCPKG_FEATURE_FLAGS}'")
message(STATUS "[VCPKG]  VCPKG_MANIFEST_FILE           -> '${VCPKG_MANIFEST_FILE}'")
message(STATUS "[VCPKG]  VCPKG_INSTALLED_DIR           -> '${VCPKG_INSTALLED_DIR}'")
message(STATUS "[VCPKG]  VCPKG_TARGET_TRIPLET          -> '${VCPKG_TARGET_TRIPLET}'")
message(STATUS "")
