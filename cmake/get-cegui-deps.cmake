#------------------------------------------------------------------------------
# External Project:     FifeChan / FifeGUI
# Downloads:            https://github.com/fifengine/fifechan/releases                                            
#------------------------------------------------------------------------------

include(ExternalProject)

set(CEGUI_DEPS_VERSION "0-8")

set(CEGUI_DEPS_CMAKE_ARGS
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
    -DCEGUI_BUILD_CORONA:BOOL=OFF
    -DCEGUI_BUILD_DEVIL:BOOL=OFF
    -DCEGUI_BUILD_EFFECTS11:BOOL=OFF
    -DCEGUI_BUILD_EXPAT:BOOL=ON
    -DCEGUI_BUILD_FREEIMAGE:BOOL=OFF
    -DCEGUI_BUILD_FREETYPE2:BOOL=ON
    -DCEGUI_BUILD_GLEW:BOOL=ON
    -DCEGUI_BUILD_GLFW:BOOL=ON
    -DCEGUI_BUILD_GLM:BOOL=ON
    -DCEGUI_BUILD_LUA:BOOL=OFF
    -DCEGUI_BUILD_MINIZIP:BOOL=OFF
    -DCEGUI_BUILD_PCRE:BOOL=ON
    -DCEGUI_BUILD_SILLY:BOOL=ON
    -DCEGUI_BUILD_TINYXML:BOOL=OFF
    -DCEGUI_BUILD_TOLUAPP:BOOL=OFF
    -DCEGUI_BUILD_XERCES:BOOL=OFF
    -DCEGUI_BUILD_ZLIB:BOOL=OFF
  )
if (WIN32)

  ExternalProject_Add(
    cegui_deps
    PREFIX              ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
    URL                 https://bitbucket.org/cegui/cegui-dependencies/get/v0-8.zip
    URL_MD5             ce65368e7d25abb0b0f6263bceb7d4ef
    INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}
    CMAKE_ARGS          ${CEGUI_DEPS_CMAKE_ARGS}
    INSTALL_COMMAND
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/cegui_deps-build/dependencies/lib          ${DEPENDENCY_INSTALL_DIR}/lib
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/cegui_deps-build/dependencies/bin          ${DEPENDENCY_INSTALL_DIR}/bin
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/cegui_deps-build/dependencies/include      ${DEPENDENCY_INSTALL_DIR}/include
  )
endif()
