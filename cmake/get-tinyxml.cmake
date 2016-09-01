#------------------------------------------------------------------------------
# External Project:     TinyXML v1
# Website:              https://sourceforge.net/projects/tinyxml 
# Downloads:            https://sourceforge.net/projects/tinyxml/files/tinyxml/                                 
#------------------------------------------------------------------------------

include(ExternalProject)

set(TINYXML_VERSION "2.6.2")

set(TINYXML_CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
      #-DCMAKE_PREFIX_PATH=${DEPENDENCY_INSTALL_DIR} ${CMAKE_PREFIX_PATH}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
)

ExternalProject_Add(
  tinyxml
  PREFIX          ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR    ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME   tinyxml-v${ZLIB_VERSION}.zip
  URL             https://sourceforge.net/projects/tinyxml/files/tinyxml/2.6.2/tinyxml_2_6_2.zip/download  
  URL_MD5         1
  CMAKE_ARGS      ${TINYXML_CMAKE_ARGS}
)