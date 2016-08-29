#------------------------------------------------------------------------------
# External Project:     zlib
# Downloads:            http://zlib.net/
# Github:               https://github.com/madler/zlib                                   
#------------------------------------------------------------------------------

include(ExternalProject)

set(ZLIB_VERSION "1.2.8")

set(ZLIB_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      #-DCMAKE_PREFIX_PATH=${DEPENDENCY_INSTALL_DIR} ${CMAKE_PREFIX_PATH}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

ExternalProject_Add(
  zlib
  PREFIX ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
  URL https://github.com/madler/zlib/archive/v${ZLIB_VERSION}.zip    
  URL_MD5 9b568ed0e7f95311dda39595995570ff
  CMAKE_ARGS ${ZLIB_CMAKE_ARGS}
)

# rename "downloads/v1.2.3.zip" to "downloads/zlib-v1.2.3.zip"
ExternalProject_Add_Step(
  zlib 
  RenameDownloadFile
  COMMAND ${CMAKE_COMMAND} -E rename "${DEPENDENCY_DOWNLOAD_DIR}/v${ZLIB_VERSION}.zip" "${DEPENDENCY_DOWNLOAD_DIR}/zlib-v${ZLIB_VERSION}.zip"
  DEPENDEES install
)