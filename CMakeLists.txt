#------------------------------------------------------------------------------
#                                 Fifengine                                          
#------------------------------------------------------------------------------

cmake_minimum_required (VERSION 2.8.5)

project (fifengine C CXX)


#------------------------------------------------------------------------------
#                                 Version                                          
#------------------------------------------------------------------------------

set (FIFE_MAJOR_VERSION 0)
set (FIFE_MINOR_VERSION 4)
set (FIFE_PATCH_VERSION 1)

set(FIFE_VERSION ${FIFE_MAJOR_VERSION}.${FIFE_MINOR_VERSION}.${FIFE_PATCH_VERSION})

configure_file (
  "${PROJECT_SOURCE_DIR}/engine/core/version.h.in"
  "${PROJECT_SOURCE_DIR}/engine/core/version.h"
)

#------------------------------------------------------------------------------
#                                 Options                                          
#------------------------------------------------------------------------------

option(opengl           "Enable OpenGL support"                                 ON)
option(fifechan         "Enable Fifechan GUI subsystem"                         ON)
option(librocket        "Enable Librocket GUI subsystem"                        OFF)
option(cegui            "Enable Crazy Eddie's GUI subsystem"                    OFF)
option(zip              "Enable ZIP archive support"                            ON)
option(logging          "Enable logging"                                        ON)
option(rend-camzone     "Enable camera zone renderer"                           OFF)
option(rend-grid        "Enable camera grid renderer"                           OFF)
option(use-githash      "Try to determine the git hash for the current commit"  ON)
option(build-python     "Build the python extension module"                     ON)
option(build-library    "Build and install files to directly develop with c++"  OFF)

#------------------------------------------------------------------------------
#                                 Configure                                          
#------------------------------------------------------------------------------

# set path to our custom CMAKE scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# Create a "Release" build by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Select build type. Options are: 'Debug', 'Release', 'RelWithDebInfo' and 'MinSizeRel'."
    FORCE
  )
  message(STATUS "Set CMAKE_BUILD_TYPE to default '${CMAKE_BUILD_TYPE}'.")
endif(NOT CMAKE_BUILD_TYPE)

# if building a C++ library is enabled, then build a shared library by default
# if you need a static library, set "-Dbuild-library=ON -DBUILD_SHARED_LIBS=OFF"
if(build-library)
  set(BUILD_SHARED_LIBS ON CACHE BOOL "Build a shared or static library")
endif(build-library)

# Do not allow an in-source-tree build, request an out-of-source-tree build.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "#Please build outside of the source tree!\n                        
                       Make a separate folder for building and run CMake there:\n
                       mkdir _build; cd _build; cmake ..\n
                       Before that, cleanup:\nrm -rf CMakeCache.txt CMakeFiles")
endif()

# Do not allow the build directory to be in the source directory for older
# versions of cmake.
if (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_BINARY_DIR} MATCHES "^${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "# Please chose a different build directory.\n
    In cmake versions < 3 SWIG does not work well when the build directory is
    placed inside the source directory.
    Use something like this:\n
        mkdir ../_build; cd ../_build; cmake ../fifengine\n
    You can remove your current build folder.")
endif()

# Disable in-source builds and modifications to the source tree.
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# organize targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#------------------------------------------------------------------------------
#                                 Compiler Options                                         
#------------------------------------------------------------------------------

if(WIN32)

  if(MSVC)

    # ensure we use minimal "windows.h" lib without the crazy min max macros    
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"WIN32_LEAN_AND_MEAN\"")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"NOMINMAX\"")

    # disable SAFESEH - to avoid "LNK2026: module unsafe" on "OpenAL32.lib"
    SET(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} /D \"SAFESEH:NO\"")
    SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

    # use static libs - to avoid "LNK1104: cannot open file 'libboost_filesystem-vc140-mt-1_59.lib'"
    set(Boost_USE_STATIC_LIBS ON)     
    set(Boost_USE_MULTITHREADED ON)

  endif(MSVC)

endif(WIN32)

# defines

# define for using TinyXML with Standard Template Library (STL) support enabled
add_definitions(-DTIXML_USE_STL)

#------------------------------------------------------------------------------
#                                 Display Status                                         
#------------------------------------------------------------------------------

message("")
message("== Building FIFE version ${FIFE_VERSION} using build type '${CMAKE_BUILD_TYPE}'.")
message(STATUS "    Source directory is '${PROJECT_SOURCE_DIR}'.")
message(STATUS "    Build  directory is '${PROJECT_BINARY_DIR}'.")
message(STATUS "    Installation path is '${CMAKE_INSTALL_PREFIX}' (set -DCMAKE_INSTALL_PREFIX=<path> to change).")
message(STATUS "    CXX Compiler Flags: '${CMAKE_CXX_FLAGS}'.")
message(STATUS "    Dependencies folder: '${PROJECT_SOURCE_DIR}/../fifengine-dependencies'.")
message("")

#------------------------------------------------------------------------------
#                           Find Dependencies - Windows                                        
#------------------------------------------------------------------------------

# Tell CMake where to search for binaries (dlls), includes and libs on Windows
if(WIN32)
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}

    # help to find dependencies on Appveyor

    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/build-tools/swig"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/includes/bin"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/includes/include"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/includes/include/SDL2"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/includes/include/OpenAL"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/includes/lib/x86"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/includes/lib"

    # help to find dependencies on your local env (FifeSDK)

    "${PROJECT_SOURCE_DIR}/../swig"
    "${PROJECT_SOURCE_DIR}/../python"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/bin"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/include"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/include/boost"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/lib"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/lib/boost"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/lib/x86"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/include/SDL2"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/include/OpenAL"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/include/lib/x86"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/include/lib"
    
  )
endif(WIN32)

#------------------------------------------------------------------------------
#                                 Source Files                                         
#------------------------------------------------------------------------------

# tell compiler to search in the following folders for include files
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/engine/core")
include_directories("${PROJECT_SOURCE_DIR}")

# Core
set (FIFE_CORE_SRC
  ${PROJECT_SOURCE_DIR}/engine/core/audio/soundclip.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/audio/soundclipmanager.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/audio/soundemitter.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/audio/soundmanager.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/controller/engine.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/controller/enginesettings.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/eventmanager.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/ext/glee/GLee.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/audio/ogg_loader.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/audio/sounddecoder_ogg.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/animationloader.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/atlasloader.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/maploader.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/objectloader.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/percentdonelistener.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/video/imageloader.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/video/resourceanimationloader.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/model.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/action.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/object.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/timeprovider.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/grids/cellgrid.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/grids/hexgrid.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/grids/squaregrid.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/cell.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/cellcache.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/instance.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/instancetree.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/layer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/location.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/map.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/renderernode.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/trigger.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/triggercontroller.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/route.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/routepather/multilayersearch.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/routepather/routepather.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/routepather/routepathersearch.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/routepather/singlelayersearch.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/savers/native/map/mapsaver.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/util/base/exception.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/util/base/fifeclass.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/util/base/stringutils.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/util/log/logger.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/util/math/angles.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/util/resource/resource.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/util/time/timeevent.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/util/time/timemanager.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/util/time/timer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/directoryprovider.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/fife_boost_filesystem.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/vfs.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/vfsdirectory.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/vfssource.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/vfssourceprovider.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/dat1.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/dat2.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/lzssdecoder.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/rawdatadat1.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/rawdatadat2.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/raw/rawdata.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/raw/rawdatafile.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/raw/rawdatamemsource.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/raw/rawdatasource.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/animation.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/animationmanager.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/atlasbook.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/color.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/cursor.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/devicecaps.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/image.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/imagemanager.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/renderbackend.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/fontbase.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/imagefontbase.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/subimagefont.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/textrenderpool.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/truetypefont.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/opengl/glimage.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/opengl/renderbackendopengl.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/sdl/renderbackendsdl.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/sdl/sdlblendingfunctions.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/video/sdl/sdlimage.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/camera.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/layercache.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/rendererbase.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderitem.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/visual.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/blockinginforenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/cellrenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/cellselectionrenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/coordinaterenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/floatingtextrenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/genericrenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/gridrenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/instancerenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/lightrenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/offrenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/quadtreerenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/targetrenderer.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/hybrid/hybridguimanager.cpp
)

# ZIP
set (FIFE_ZIP_SRC
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipfilesource.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipnode.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipprovider.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipsource.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/ziptree.cpp
)

# CEGUI v0.8.0+
set (FIFE_CEGUI_SRC
  ${PROJECT_SOURCE_DIR}/engine/core/gui/cegui/ceguimanager.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/cegui/base/ceguiinputprocessor.cpp
)

# FIFECHAN
set (FIFE_FCHAN_SRC
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/fifechanmanager.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/gui_font.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/gui_image.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/gui_imageloader.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/opengl/opengl_gui_graphics.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/sdl/sdl_gui_graphics.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/console/commandline.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/console/console.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/util/utf8/utf8stringeditor.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/animationicon.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/clicklabel.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/dockarea.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/panel.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/percentagebar.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/resizablewindow.cpp
)

# LIBROCKET
set (FIFE_LROCKET_SRC
  ${PROJECT_SOURCE_DIR}/engine/core/gui/librocket/librocketmanager.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/librocket/base/librocketinputprocessor.cpp
  ${PROJECT_SOURCE_DIR}/engine/core/gui/librocket/base/librocketrenderinterface.cpp
)

#------------------------------------------------------------------------------
#                         Source Groups for Source Files                                    
#------------------------------------------------------------------------------

source_group("sources\\fife.core"                FILES ${FIFE_CORE_SRC})
source_group("sources\\fife.core.vfs.zip"        FILES ${FIFE_ZIP_SRC})
source_group("sources\\fife.core.gui.cegui"      FILES ${FIFE_CEGUI_SRC})
source_group("sources\\fife.core.gui.fifechan"   FILES ${FIFE_FCHAN_SRC})
source_group("sources\\fife.core.gui.librocket"  FILES ${FIFE_LROCKET_SRC})

#------------------------------------------------------------------------------
#                                 Header Files                                         
#------------------------------------------------------------------------------

# Core
set (FIFE_CORE_HDR
  ${PROJECT_SOURCE_DIR}/engine/core/export.h
  ${PROJECT_SOURCE_DIR}/engine/core/modules.h
  ${PROJECT_SOURCE_DIR}/engine/core/audio/fife_openal.h
  ${PROJECT_SOURCE_DIR}/engine/core/audio/soundclip.h
  ${PROJECT_SOURCE_DIR}/engine/core/audio/soundclipmanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/audio/soundconfig.h
  ${PROJECT_SOURCE_DIR}/engine/core/audio/sounddecoder.h
  ${PROJECT_SOURCE_DIR}/engine/core/audio/soundemitter.h
  ${PROJECT_SOURCE_DIR}/engine/core/audio/soundmanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/controller/engine.h
  ${PROJECT_SOURCE_DIR}/engine/core/controller/enginesettings.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/eventmanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/base/ec_event.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/base/ec_inputevent.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/command/ec_command.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/command/ec_commandids.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/command/ec_icommandcontroller.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/command/ec_icommandlistener.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/drop/ec_dropevent.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/drop/ec_idropcontroller.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/drop/ec_idroplistener.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/key/ec_ikeycontroller.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/key/ec_ikeyfilter.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/key/ec_ikeylistener.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/key/ec_key.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/key/ec_keyevent.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/mouse/ec_imousecontroller.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/mouse/ec_imousefilter.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/mouse/ec_imouselistener.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/mouse/ec_mouseevent.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/sdl/ec_isdleventcontroller.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/sdl/ec_isdleventlistener.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/source/ec_eventsourcetypes.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/source/ec_ieventsource.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/text/ec_itextcontroller.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/text/ec_itextlistener.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/text/ec_text.h
  ${PROJECT_SOURCE_DIR}/engine/core/eventchannel/text/ec_textevent.h
  ${PROJECT_SOURCE_DIR}/engine/core/ext/glee/GLee.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/audio/ogg_loader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/audio/sounddecoder_ogg.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/animationloader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/atlasloader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/ianimationloader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/iatlasloader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/imaploader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/iobjectloader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/maploader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/objectloader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/map/percentdonelistener.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/video/imageloader.h
  ${PROJECT_SOURCE_DIR}/engine/core/loaders/native/video/resourceanimationloader.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/model.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/action.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/ipather.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/ivisual.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/modelcoords.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/object.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/timeprovider.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/grids/cellgrid.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/grids/hexgrid.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/metamodel/grids/squaregrid.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/cell.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/cellcache.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/instance.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/instancetree.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/layer.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/location.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/map.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/renderernode.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/trigger.h
  ${PROJECT_SOURCE_DIR}/engine/core/model/structures/triggercontroller.h
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/route.h
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/routepather/multilayersearch.h
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/routepather/routepather.h
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/routepather/routepathersearch.h
  ${PROJECT_SOURCE_DIR}/engine/core/pathfinder/routepather/singlelayersearch.h
  ${PROJECT_SOURCE_DIR}/engine/core/savers/native/map/ianimationsaver.h
  ${PROJECT_SOURCE_DIR}/engine/core/savers/native/map/iatlassaver.h
  ${PROJECT_SOURCE_DIR}/engine/core/savers/native/map/imapsaver.h
  ${PROJECT_SOURCE_DIR}/engine/core/savers/native/map/iobjectsaver.h
  ${PROJECT_SOURCE_DIR}/engine/core/savers/native/map/mapsaver.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/base/exception.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/base/fifeclass.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/base/fife_stdint.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/base/sharedptr.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/base/singleton.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/base/stringutils.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/log/logger.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/math/angles.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/math/fife_math.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/math/matrix.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/resource/resource.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/resource/resourcemanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/structures/point.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/structures/priorityqueue.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/structures/purge.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/structures/quadtree.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/structures/rect.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/time/timeevent.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/time/timemanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/time/timer.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/utf8/utf8.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/utf8/utf8/checked.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/utf8/utf8/core.h
  ${PROJECT_SOURCE_DIR}/engine/core/util/utf8/utf8/unchecked.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/directoryprovider.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/fife_boost_filesystem.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/vfs.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/vfsdirectory.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/vfssource.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/vfssourceprovider.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/dat1.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/dat2.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/lzssdecoder.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/rawdatadat1.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/dat/rawdatadat2.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/raw/rawdata.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/raw/rawdatafile.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/raw/rawdatamemsource.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/raw/rawdatasource.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipfilesource.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipnode.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipprovider.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipsource.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/ziptree.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/animation.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/animationmanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/atlasbook.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/color.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/cursor.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/devicecaps.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/image.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/imagemanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/renderbackend.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/fontbase.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/ifont.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/imagefontbase.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/subimagefont.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/textrenderpool.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/fonts/truetypefont.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/opengl/fife_opengl.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/opengl/glimage.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/opengl/renderbackendopengl.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/sdl/renderbackendsdl.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/sdl/sdlblendingfunctions.h
  ${PROJECT_SOURCE_DIR}/engine/core/video/sdl/sdlimage.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/camera.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/layercache.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/rendererbase.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderitem.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/visual.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/blockinginforenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/cellrenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/cellselectionrenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/coordinaterenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/floatingtextrenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/genericrenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/gridrenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/instancerenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/lightrenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/offrenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/quadtreerenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/view/renderers/targetrenderer.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/guimanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/hybrid/hybridguimanager.h
)

# ZIP
set (FIFE_ZIP_HDR
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipfilesource.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipnode.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipprovider.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/zipsource.h
  ${PROJECT_SOURCE_DIR}/engine/core/vfs/zip/ziptree.h
)
  
# CEGUI
set (FIFE_CEGUI_HDR
  ${PROJECT_SOURCE_DIR}/engine/core/gui/cegui/ceguimanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/cegui/base/ceguiinputprocessor.h
)

# Fifechan
set (FIFE_FCHAN_HDR
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/fifechanmanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/gui_font.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/gui_image.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/gui_imageloader.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/opengl/opengl_gui_graphics.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/base/sdl/sdl_gui_graphics.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/console/commandline.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/console/console.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/util/utf8/utf8stringeditor.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/animationicon.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/clicklabel.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/dockarea.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/panel.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/percentagebar.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/fifechan/widgets/resizablewindow.h
)

# Librocket
set (FIFE_LROCKET_HDR
  ${PROJECT_SOURCE_DIR}/engine/core/gui/librocket/librocketmanager.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/librocket/base/librocketinputprocessor.h
  ${PROJECT_SOURCE_DIR}/engine/core/gui/librocket/base/librocketrenderinterface.h
)

#------------------------------------------------------------------------------
#                      Source Groups for Header Files                                    
#------------------------------------------------------------------------------

source_group("headers\\fife.core"                FILES ${FIFE_CORE_HDR})
source_group("headers\\fife.core.vfs.zip"        FILES ${FIFE_ZIP_HDR})
source_group("headers\\fife.core.gui.cegui"      FILES ${FIFE_CEGUI_HDR})
source_group("headers\\fife.core.gui.fifechan"   FILES ${FIFE_FCHAN_HDR})
source_group("headers\\fife.core.gui.librocket"  FILES ${FIFE_LROCKET_HDR})

#------------------------------------------------------------------------------
#                                 SWIG files                                         
#------------------------------------------------------------------------------

# Core
set (FIFE_CORE_SWIG
  version.i
  audio/soundclip.i
  audio/soundclipmanager.i
  audio/soundemitter.i
  audio/soundmanager.i
  controller/engine.i
  eventchannel/eventchannel.i
  loaders/native/map/ianimationloader.i
  loaders/native/map/iatlasloader.i
  loaders/native/map/imaploader.i
  loaders/native/map/iobjectloader.i
  loaders/native/map/maploader.i
  loaders/native/map/percentdonelistener.i
  model/metamodel/action.i
  model/metamodel/ipather.i
  model/metamodel/ivisual.i
  model/metamodel/modelcoords.i
  model/metamodel/object.i
  model/metamodel/timeprovider.i
  model/metamodel/grids/cellgrids.i
  model/structures/cell.i
  model/structures/cellcache.i
  model/structures/instance.i
  model/structures/layer.i
  model/structures/location.i
  model/structures/map.i
  model/structures/renderernode.i
  model/structures/trigger.i
  model/model.i
  pathfinder/route.i
  pathfinder/routepather/routepather.i
  savers/native/map/ianimationsaver.i
  savers/native/map/iatlassaver.i
  savers/native/map/imapsaver.i
  savers/native/map/iobjectsaver.i
  savers/native/map/mapsaver.i
  util/base/utilbase.i
  util/log/logger.i
  util/math/math.i
  util/resource/resource.i
  util/structures/utilstructures.i
  util/time/timeevent.i
  util/time/timemanager.i
  vfs/vfs.i
  vfs/raw/rawdata.i
  video/video.i
  video/fonts/fonts.i
  view/camera.i
  view/rendererbase.i
  view/visual.i
  view/renderers/blockinginforenderer.i
  view/renderers/cellrenderer.i
  view/renderers/cellselectionrenderer.i
  view/renderers/coordinaterenderer.i
  view/renderers/floatingtextrenderer.i
  view/renderers/genericrenderer.i
  view/renderers/gridrenderer.i
  view/renderers/instancerenderer.i
  view/renderers/lightrenderer.i
  view/renderers/offrenderer.i
  view/renderers/targetrenderer.i
  gui/guimanager.i
  gui/hybrid/hybridguimanager.i
)

# CEGUI
set (FIFE_CEGUI_SWIG
  gui/cegui/ceguimanager.i
)

# FIFECHAN
set (FIFE_FCHAN_SWIG_LIB
  gui/fifechan/lib/fifechanlistener.i
  gui/fifechan/lib/widgets.i
)

set (FIFE_FCHAN_SWIG
  gui/fifechan/base/gui_font.i
  gui/fifechan/base/gui_image.i
  gui/fifechan/console/console.i
  gui/fifechan/fifechanmanager.i
  gui/fifechan/widgets/widgets.i
)

# LIBROCKET
set (FIFE_LROCKET_SWIG
  gui/librocket/librocketmanager.i
)

#------------------------------------------------------------------------------
#                      Source Groups for SWIG Files                                    
#------------------------------------------------------------------------------

source_group("swig\\fife.core"                FILES ${FIFE_CORE_SWIG})
source_group("swig\\fife.core.gui.cegui"      FILES ${FIFE_CEGUI_HDR})
source_group("swig\\fife.core.gui.fifechan"   FILES ${FIFE_FCHAN_SWIG_LIB})
source_group("swig\\fife.core.gui.fifechan"   FILES ${FIFE_FCHAN_SWIG})
source_group("swig\\fife.core.gui.librocket"  FILES ${FIFE_LROCKET_SWIG})

#------------------------------------------------------------------------------
#                                 Find Packages                                         
#------------------------------------------------------------------------------

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Boost 1.33.1 COMPONENTS system filesystem regex)
find_package(PNG REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(TinyXML REQUIRED)

if(cegui)
  find_package(CEGUI REQUIRED)

  if(NOT CEGUI_OpenGLRenderer_FOUND)
    message(FATAL_ERROR "FIFE needs CEGUI with the OpenGLRenderer enabled.")
  endif(NOT CEGUI_OpenGLRenderer_FOUND)

  if(NOT opengl)
    message(FATAL_ERROR "OpenGL cannot be disabled, when CEGUI is enabled.")
  endif(NOT opengl)

  include_directories(${CEGUI_INCLUDE_DIR})

  message("Using CEGUI v0.8+")
  set(CEGUI TRUE)
  set(FIFE_CORE_SRC ${FIFE_CORE_SRC} ${FIFE_CEGUI_SRC})

  find_package(GLEW REQUIRED)
  include_directories(${GLEW_INCLUDE_DIRS})
endif(cegui)

if(build-python)
  # Setting up SWIG files
  set(SWIG_MODULE_fife_EXTRA_DEPS)
  set(CMAKE_SWIG_IMPORTS "")
  set(CMAKE_SWIG_INCLUSIONS "")
  set(CMAKE_SWIG_IMPORTS_FCHAN "")
  set(CMAKE_SWIG_INCLUSIONS_FCHAN "")

  foreach(filename ${FIFE_CORE_SWIG})
    list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/engine/core/${filename}")
    set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
  endforeach(filename ${FIFE_CORE_SWIG})
endif(build-python)

if(fifechan)

  find_package(FifeChan COMPONENTS sdl REQUIRED)
 
  if(build-python)
    set(SWIG_MODULE_fifechan_EXTRA_DEPS)
    foreach(filename ${FIFE_FCHAN_SWIG_LIB})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/engine/core/${filename}")
      list(APPEND SWIG_MODULE_fifechan_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/engine/core/${filename}")
      set(CMAKE_SWIG_INCLUSIONS_FCHAN "${CMAKE_SWIG_INCLUSIONS_FCHAN}%include ${filename}\n")
    endforeach(filename ${FIFE_FCHAN_SWIG_LIB})
    foreach(filename ${FIFE_FCHAN_SWIG})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/engine/core/${filename}")
      list(APPEND SWIG_MODULE_fifechan_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/engine/core/${filename}")
      set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
    endforeach(filename ${FIFE_FCHAN_SWIG})
    set(CMAKE_SWIG_IMPORTS "${CMAKE_SWIG_IMPORTS}%import ${PROJECT_BINARY_DIR}/fifechan.i")
  endif(build-python)

  if(opengl)
    find_package(FifeChan COMPONENTS opengl REQUIRED)
    add_definitions(-DHAVE_OPENGL)
  endif(opengl)
  if(zip)
    set(FIFE_CORE_SRC ${FIFE_CORE_SRC} ${FIFE_ZIP_SRC})
    add_definitions(-DHAVE_ZIP)
  endif(zip)
  if(logging)
    add_definitions(-DLOG_ENABLED)
  endif(logging)
  if(rend-camzone)
    add_definitions(-DRENDER_CAMZONES)
  endif(rend-camzone)
  if(rend-grid)
    add_definitions(-DRENDER_GRID)
  endif(rend-grid)
  if(use-githash)
    execute_process(
      COMMAND git rev-parse --short=8 HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_COMMIT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    add_definitions(-DFIFE_GIT_HASH=${GIT_COMMIT_HASH})
  endif(use-githash)
  include_directories(${FIFECHAN_INCLUDE_DIR})
  link_directories(${FIFECHAN_LIBRARIES})
  if(build-python)
    configure_file (
      "${PROJECT_SOURCE_DIR}/engine/swigwrappers/python/fifechan.i.templ.cm"
      "${PROJECT_BINARY_DIR}/fifechan.i"
      )
   endif(build-python)
endif(fifechan)

if(cegui)  
  if(build-python)
    foreach(filename ${FIFE_CEGUI_SWIG})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/engine/core/${filename}")
      set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
    endforeach(filename ${FIFE_CEGUI_SWIG})    
   endif(build-python)
endif(cegui)

# TODO: Add the other gui libraries

if(build-python)
  configure_file (
    "${PROJECT_SOURCE_DIR}/engine/swigwrappers/python/fife.i.templ.cm"
    "${PROJECT_BINARY_DIR}/fife.i"
  )

  # find SWIG 
  find_package(SWIG REQUIRED)

  # protect user from working with Swig v3.0.8, which is broken
  if("${SWIG_VERSION}" VERSION_EQUAL "3.0.8")
    message(FATAL_ERROR "You are using SWIG v3.0.8.\n 
                         This version doesn't work with fifengine.\n 
                         Please upgrade SWIG.")
  endif()

  # include the use swig file
  include(${SWIG_USE_FILE})

endif(build-python)

# Find ogg dependency
find_package(PkgConfig QUIET)
pkg_check_modules(PC_OGG QUIET ogg)
find_path(OGG_INCLUDE_DIRS NAMES ogg/ogg.h HINTS ${PC_OGG_INCLUDE_DIRS} PATH_SUFFIXES ogg)
find_library(OGG_LIBRARIES NAMES ogg HINTS ${PC_OGG_LIBRARY_DIRS})

# Find vorbis dependency
find_package(PkgConfig QUIET)
pkg_check_modules(PC_VORBISFILE QUIET vorbisfile)
find_path(VORBIS_INCLUDE_DIRS NAMES vorbis/vorbisfile.h HINTS ${PC_VORBIS_INCLUDE_DIRS} PATH_SUFFIXES vorbis)
find_library(VORBIS_LIBRARY NAMES vorbis HINTS ${PC_VORBIS_LIBRARY_DIRS})

# Find vorbisfile dependency
find_package(PkgConfig QUIET)
pkg_check_modules(PC_VORBISFILE QUIET vorbisfile)
find_library(VORBISFILE_LIBRARY NAMES vorbisfile HINTS ${PC_VORBIS_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OGG DEFAULT_MSG OGG_INCLUDE_DIRS OGG_LIBRARIES)
find_package_handle_standard_args(vorbisfile DEFAULT_MSG VORBIS_INCLUDE_DIRS VORBIS_LIBRARY)
find_package_handle_standard_args(vorbisfile DEFAULT_MSG VORBIS_INCLUDE_DIRS VORBISFILE_LIBRARY)

include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${OPENAL_INCLUDE_DIR})
include_directories(${OGG_INCLUDE_DIRS})
include_directories(${VORBIS_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})

add_definitions(${PNG_DEFINITIONS})
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})

#------------------------------------------------------------------------------
#                  Build fifengine as a Python extension module                                       
#------------------------------------------------------------------------------

if(build-python)
  # Find python library and add include path for python headers
  find_package (PythonInterp 2 EXACT)
  set(PYTHON_LIBRARY "PYTHON_LIBRARY-NOTFOUND")
  set(PYTHON_INCLUDE_DIR "PYTHON_INCLUDE_DIR-NOTFOUND")
  find_package (PythonLibs ${PYTHON_VERSION_STRING} EXACT)
  include_directories (${PYTHON_INCLUDE_PATH})

  set (CMAKE_SWIG_FLAGS -modern -modernargs -w511 -w473 -w404)

  set_source_files_properties("${PROJECT_BINARY_DIR}/fife.i" PROPERTIES CPLUSPLUS ON)

  swig_add_module(fife python "${PROJECT_BINARY_DIR}/fife.i"  ${FIFE_CORE_SRC} ${FIFE_FCHAN_SRC})
  swig_link_libraries(fife ${PYTHON_LIBRARIES})
  swig_link_libraries(fife ${SDL2_LIBRARY}
                           ${SDL2_IMAGE_LIBRARIES}
                           ${SDL2_TTF_LIBRARIES}
                           ${OPENAL_LIBRARY})
  swig_link_libraries(fife ${Boost_LIBRARIES})
  swig_link_libraries(fife ${PNG_LIBRARIES})
  swig_link_libraries(fife ${VORBISFILE_LIBRARY})
  swig_link_libraries(fife ${VORBIS_LIBRARY})
  swig_link_libraries(fife ${OGG_LIBRARIES})
  swig_link_libraries(fife ${TinyXML_LIBRARIES})
  swig_link_libraries(fife ${OPENGL_gl_LIBRARY})

  if(fifechan)
    set_source_files_properties("${PROJECT_BINARY_DIR}/fifechan.i" PROPERTIES CPLUSPLUS ON)
    swig_add_module(fifechan python "${PROJECT_BINARY_DIR}/fifechan.i")
    swig_link_libraries(fifechan ${PYTHON_LIBRARIES})
    swig_link_libraries(fifechan ${FIFECHAN_LIBRARIES})
    swig_link_libraries(fife ${FIFECHAN_LIBRARIES})
  endif(fifechan)

  if(cegui)
    swig_link_libraries(fife ${CEGUI_LIBRARIES})
    swig_link_libraries(fife ${CEGUI_OpenGLRenderer_LIBRARIES})
  endif(cegui)

  if(NOT DEFINED PYTHON_SITE_PACKAGES)
    #
    # Install Python package locally, e.g. into "C:\Python27\Lib\site-packages\fife"
    #
    execute_process(
      COMMAND  ${PYTHON_EXECUTABLE} -c "import sys; from distutils.sysconfig import get_python_lib; sys.stdout.write(get_python_lib(True))"
      OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
    )

    # Slash-Fix: Cmake would create a warning, when using the variable in FILES or DIRECTORY without changing it.
    file(TO_CMAKE_PATH ${PYTHON_SITE_PACKAGES} PYTHON_SITE_PACKAGES)
 endif(NOT DEFINED PYTHON_SITE_PACKAGES)

  message(STATUS "    Python install directory is '${PYTHON_SITE_PACKAGES}/fife'.")

  install(TARGETS     _fife                                               DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  install(FILES       ${PROJECT_BINARY_DIR}/fife.py                       DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  install(FILES       ${CMAKE_SOURCE_DIR}/engine/python/fife/__init__.py  DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  install(DIRECTORY   ${CMAKE_SOURCE_DIR}/engine/python/fife/extensions   DESTINATION ${PYTHON_SITE_PACKAGES}/fife)

  if(fifechan)
    install(TARGETS   _fifechan                                           DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
    install(FILES     ${PROJECT_BINARY_DIR}/fifechan.py                   DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  endif(fifechan)
endif(build-python)

#------------------------------------------------------------------------------
#                       Build fifengine as a library                                        
#------------------------------------------------------------------------------

if(build-library)
  add_library(fife ${FIFE_CORE_SRC} ${FIFE_FCHAN_SRC})
  set_target_properties(fife PROPERTIES SOVERSION ${FIFE_VERSION})

  target_link_libraries(fife ${SDL2_LIBRARY}
                             ${SDL2_IMAGE_LIBRARIES}
                             ${SDL2_TTF_LIBRARIES}
                             ${OPENAL_LIBRARY})
  target_link_libraries(fife ${Boost_LIBRARIES})
  target_link_libraries(fife ${PNG_LIBRARIES})
  target_link_libraries(fife ${VORBISFILE_LIBRARY})
  target_link_libraries(fife ${VORBIS_LIBRARY})
  target_link_libraries(fife ${OGG_LIBRARIES})
  target_link_libraries(fife ${TinyXML_LIBRARIES})
  target_link_libraries(fife ${OPENGL_gl_LIBRARY})
  target_link_libraries(fife ${FIFECHAN_LIBRARIES})

  include(GNUInstallDirs)
  install(TARGETS fife DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(FILES ${PROJECT_SOURCE_DIR}/engine/core/version.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fife/core/)
  MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)
  
      FOREACH(HEADER ${${HEADER_LIST}})
          get_filename_component(DIR ${HEADER} DIRECTORY)
          string(REPLACE ${CMAKE_CURRENT_LIST_DIR}/engine "" DIR ${DIR})
          INSTALL(FILES ${HEADER} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fife/${DIR})
      ENDFOREACH(HEADER)

  ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)
  
  INSTALL_HEADERS_WITH_DIRECTORY(FIFE_CORE_HDR)
  if(fifechan)
    INSTALL_HEADERS_WITH_DIRECTORY(FIFE_FCHAN_HDR)
  endif(fifechan)
  if(cegui)
    INSTALL_HEADERS_WITH_DIRECTORY(FIFE_CEGUI_HDR)
  endif(cegui)
  if(rocket)
    INSTALL_HEADERS_WITH_DIRECTORY(FIFE_LROCKET_HDR)
  endif(rocket)
endif(build-library)
