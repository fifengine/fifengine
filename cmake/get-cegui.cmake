#------------------------------------------------------------------------------
# External Project:     ceGUI
# Website:              http://www.cegui.org.uk/
# Bitbucket:            https://bitbucket.org/cegui/cegui
#                       https://bitbucket.org/cegui/cegui-dependencies/  
#                       https://bitbucket.org/cegui/cegui-sdkbuilder                                          
# Appveyor:             https://ci.appveyor.com/project/cegui-ci/cegui/
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# WARNING for WIN32 MSVC prebuilds:
#
# cegui doesn't provide the prebuild cegui-sdk as a download!
# so we host a version of cegui-sdk-vc14-x86.zip as part 
# of the release artifacts for fife v0.4.1.
# See https://github.com/fifengine/fifengine/releases/tag/0.4.1
#
#------------------------------------------------------------------------------

include(ExternalProject)

set(CEGUI_VERSION "0.8")

# For Windows it's faster to download the pre-compiled development binaries.
if (WIN32)

  if(MSVC) # download pre-compiled dev package for Visual C++ 32/64-bit

    ExternalProject_Add(
      cegui
      PREFIX              ${DEPENDENCY_EXTRACT_DIR}
      DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
      #URL                https://some.where/cegui/releases/tag/${CEGUICHAN_VERSION}/cegui-sdk-v${CEGUI_VERSION}-VC14-x86.zip
      URL                 https://github.com/fifengine/fifengine/releases/download/0.4.1/cegui-sdk-VC14-x86.zip
      URL_MD5             79b026e2382622d6f75cbe40a1ab363b
      CONFIGURE_COMMAND   ""
      BUILD_COMMAND       ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/cegui/bin     ${DEPENDENCY_INSTALL_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/cegui/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/cegui/include ${DEPENDENCY_INSTALL_DIR}/include/cegui
    )

  endif(MSVC)

else() # build from source - https://bitbucket.org/cegui/cegui-dependencies/src

#  set(CEGUI_CMAKE_ARGS
#      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
#      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
#      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
#      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
#  )
#   
#  ExternalProject_Add(
#    cegui
#    PREFIX              ${DEPENDENCY_EXTRACT_DIR}
#    DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
#    URL                 https://bitbucket.org/cegui/cegui-dependencies/get/0ecdf3a9e49b.zip
#    URL_MD5             1
#    #CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
#    INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}
#    CMAKE_ARGS          ${FIFECHAN_CMAKE_ARGS}
#  )

endif()
