#------------------------------------------------------------------------------
# External Project:     GoogleTest
#
# GitHub:               https://github.com/google/googletest  
#------------------------------------------------------------------------------

include(ExternalProject)

set(GOOGLETEST_VERSION "1.8.0")

# build from source

set(GOOGLETEST_CMAKE_ARGS
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
  -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
  # configure
  # -DBUILD_GTEST=ON note: GMOCK builds GTEST already
  -DBUILD_GMOCK=ON
)

ExternalProject_Add(
  googletest
  PREFIX           ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR     ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME    googletest-v${GOOGLETEST_VERSION}.zip
  URL              https://github.com/google/googletest/archive/release-${GOOGLETEST_VERSION}.tar.gz
  URL_MD5          16877098823401d1bf2ed7891d7dce36
  CMAKE_ARGS       ${GOOGLETEST_CMAKE_ARGS}
)