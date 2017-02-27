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

ExternalProject_Add(googletest
  PREFIX           ${DEPENDENCY_EXTRACT_DIR}
  GIT_REPOSITORY   git@github.com:google/googletest.git
  GIT_TAG          release-${GOOGLETEST_VERSION}  
  CMAKE_ARGS       ${GOOGLETEST_CMAKE_ARGS}
)