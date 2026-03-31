#------------------------------------------------------------------------------
#                           Fifengine Dependencies
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.25...4.2)

# include additional CMake scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../CMake")

project(fife-dependencies)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# Canonical output root for dependency artifacts, suffixed with triplet.
set(
  FIFE_DEPENDENCIES_ROOT
  "${CMAKE_CURRENT_SOURCE_DIR}/../out/fife-dependencies/${VCPKG_TARGET_TRIPLET}"
  CACHE PATH "Root directory for dependency artifacts (with subfolders downloads/extracted/install/build-tools)"
)
get_filename_component(FIFE_DEPENDENCIES_ROOT "${FIFE_DEPENDENCIES_ROOT}" ABSOLUTE)

# Override CMAKE_INSTALL_PREFIX if not set by caller or environment
# to ensure dependencies are installed in a known location.
if(NOT DEFINED CMAKE_INSTALL_PREFIX AND
   NOT DEFINED ENV{CMAKE_INSTALL_PREFIX})
  set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT 1)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${FIFE_DEPENDENCIES_ROOT}/install" CACHE PATH "default install path" FORCE)
endif()

# Fallback locations used when the caller does not provide DEPENDENCY_*.
# In the explicit workflow/build.sh flow these are usually passed in.
if(NOT DEPENDENCY_DOWNLOAD_DIR)
  set(DEPENDENCY_DOWNLOAD_DIR "${FIFE_DEPENDENCIES_ROOT}/downloads")
endif()

if(NOT DEPENDENCY_INSTALL_DIR)
  set(DEPENDENCY_INSTALL_DIR "${FIFE_DEPENDENCIES_ROOT}/install")
endif()

if(NOT DEPENDENCY_EXTRACT_DIR)
  set(DEPENDENCY_EXTRACT_DIR "${FIFE_DEPENDENCIES_ROOT}/extracted")
endif()

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------

option(FIFE_DEPENDENCY_FIFECHAN "Build and install dependency: FIFECHAN"     ON)
option(FIFE_DEPENDENCY_SWIG     "Build and install dependency: SWIG"         ON)

#------------------------------------------------------------------------------
# Display Status
#------------------------------------------------------------------------------

message(STATUS "---------------------------------------------------------------------")
message(STATUS "[ 🛠️ ] Building ${PROJECT_NAME} using build type '${CMAKE_BUILD_TYPE}'.")
message(STATUS "---------------------------------------------------------------------")
message(STATUS "")
message(STATUS "[INFO] Source dir                 -> '${PROJECT_SOURCE_DIR}'.")
message(STATUS "[INFO] Build  dir                 -> '${PROJECT_BINARY_DIR}'.")
message(STATUS "[INFO] Installation path          -> '${CMAKE_INSTALL_PREFIX}' (set -DCMAKE_INSTALL_PREFIX=<path> to change).")
message(STATUS "[INFO] DEPENDENCY_DOWNLOAD_DIR    -> '${DEPENDENCY_DOWNLOAD_DIR}'")
message(STATUS "[INFO] DEPENDENCY_EXTRACT_DIR     -> '${DEPENDENCY_EXTRACT_DIR}'")
message(STATUS "[INFO] DEPENDENCY_INSTALL_DIR     -> '${DEPENDENCY_INSTALL_DIR}'")
message(STATUS "")
message(STATUS "Build Options")
message(STATUS "")
message(STATUS "[INFO] FIFE_DEPENDENCY_FIFECHAN   -> '${FIFE_DEPENDENCY_FIFECHAN}'")
message(STATUS "[INFO] FIFE_DEPENDENCY_SWIG       -> '${FIFE_DEPENDENCY_SWIG}'")
message(STATUS "---------------------------------------------------------------------")

#------------------------------------------------------------------------------
# Check Options + Include external dependency scripts
#------------------------------------------------------------------------------

if(FIFE_DEPENDENCY_FIFECHAN)
  include(get-fifechan)
endif()

if(FIFE_DEPENDENCY_SWIG)
  include(get-swig)
endif()
