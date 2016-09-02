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

set(TINYXML_CMAKE_CONFIG_ARGS    
    -G ${CMAKE_GENERATOR}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_INSTALL_PREFIX:PATH=${DEPENDENCY_INSTALL_DIR}
    -DCMAKE_CXX_FLAGS:STRING=-DTIXML_USE_STL
    ${DEPENDENCY_EXTRACT_DIR}/src/tinyxml
)

ExternalProject_Add(
  tinyxml
  PREFIX                ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR          ${DEPENDENCY_DOWNLOAD_DIR}
  DOWNLOAD_NAME         tinyxml-v${TINYXML_VERSION}.zip
  URL                   https://sourceforge.net/projects/tinyxml/files/tinyxml/2.6.2/tinyxml_2_6_2.zip/download  
  URL_MD5               2a0aaf609c9e670ec9748cd01ed52dae
  CONFIGURE_COMMAND     ${CMAKE_COMMAND} ${TINYXML_CMAKE_CONFIG_ARGS}
  #CMAKE_ARGS           ${TINYXML_CMAKE_ARGS}
)