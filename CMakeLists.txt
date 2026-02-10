#------------------------------------------------------------------------------
#                                 Fifengine
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

#------------------------------------------------------------------------------
#                                 Version
#------------------------------------------------------------------------------

set(FIFE_MAJOR_VERSION 0)
set(FIFE_MINOR_VERSION 4)
set(FIFE_PATCH_VERSION 2)
set(FIFE_VERSION_SHORT ${FIFE_MAJOR_VERSION}.${FIFE_MINOR_VERSION}.${FIFE_PATCH_VERSION})

# check if "git" is installed
find_program(GIT_TOOL git DOC "Git version control")
mark_as_advanced(GIT_TOOL)

# get git commit hash, if source folder is a git repository
find_file(GIT_DIR NAMES .git PATHS ${CMAKE_SOURCE_DIR} NO_DEFAULT_PATH)

if(GIT_TOOL AND GIT_DIR)
  execute_process(
    COMMAND git rev-parse --short=7 HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  set(FIFE_GIT_HASH ${GIT_COMMIT_HASH})
  unset(GIT_DIR)
else()
  # not a git repo, possibly a source download
  set(FIFE_GIT_HASH "")
endif()

if(GIT_COMMIT_HASH)
  set(FIFE_VERSION ${FIFE_VERSION_SHORT}+build.${FIFE_GIT_HASH})
else()
  set(FIFE_VERSION ${FIFE_VERSION_SHORT})
endif()

#------------------------------------------------------------------------------
#                                 Project
#------------------------------------------------------------------------------

project(fifengine
  VERSION ${FIFE_MAJOR_VERSION}.${FIFE_MINOR_VERSION}.${FIFE_PATCH_VERSION}
  LANGUAGES CXX
)

#------------------------------------------------------------------------------
#                                 Versionize
#------------------------------------------------------------------------------


message("== Updating version.h with version information and git meta data.")

configure_file (
  "${PROJECT_SOURCE_DIR}/src/engine/version.h.in"
  "${PROJECT_SOURCE_DIR}/src/engine/version.h"
)

#------------------------------------------------------------------------------
#                                 Options
#------------------------------------------------------------------------------

option(opengl           "Enable OpenGL support"                                 ON)
option(fifechan         "Enable Fifechan GUI subsystem"                         ON)
option(librocket        "Enable Librocket GUI subsystem"                        OFF)
option(cegui            "Enable Crazy Eddie's GUI subsystem"                    OFF)
option(logging          "Enable logging"                                        ON)
option(build-python     "Build the python extension module"                     ON)
option(build-library    "Build and install files to directly develop with c++"  OFF)

#------------------------------------------------------------------------------
#                                 Configure
#------------------------------------------------------------------------------

# set path to our custom CMAKE scripts
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(SetupVCPKG)
include(FeatureSummary)
if(fifechan)
  include(get-fifechan)
endif(fifechan)

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
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "#Please build outside of the source tree!\n
                       Make an out-of-source-tree folder for building and run CMake there:\n
                       mkdir ../_build; cd ../_build; cmake ../fifengine\n
                       Before that, cleanup:\nrm -rf CMakeCache.txt CMakeFiles")
endif()

# Disable in-source builds and modifications to the source tree.
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# organize targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#------------------------------------------------------------------------------
#                                 Compiler Options
#------------------------------------------------------------------------------

# MSVC compiler options
if(WIN32 AND MSVC)
  # build on all cores
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /nologo")

  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0 /O2")
  else()
    # warning level: /W<level 0 to 4> or /Wall
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /Od")
  endif()

  # ensure we use minimal "windows.h" lib without the crazy min max macros
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"WIN32_LEAN_AND_MEAN\"")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"NOMINMAX\"")

  # avoid "C1128: number of sections exceeded object file format limit: compile with /bigobj"
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

  # disable SAFESEH - to avoid "LNK2026: module unsafe" on "OpenAL32.lib"
  set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} /D /SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

  # Note: c++11 is implicitly enabled on MSVC >=2013
endif()

# GCC and Clang compiler options
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++17 -fPIC ${CMAKE_CXX_FLAGS}")

  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wno-error")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g2 -Wall -Wextra -Wunused")
  endif()
endif()

#------------------------------------------------------------------------------
#                                 Defines
#------------------------------------------------------------------------------

# define for using TinyXML with Standard Template Library (STL) support enabled
add_definitions(-DTIXML_USE_STL)

if(logging)
  add_definitions(-DLOG_ENABLED)
endif(logging)

if(opengl)
  add_definitions(-DHAVE_OPENGL)
endif(opengl)

if(cegui)
  add_definitions(-DHAVE_CEGUI)
endif(cegui)

#------------------------------------------------------------------------------
#                                 Display Status
#------------------------------------------------------------------------------

message("")
message("== Building FIFE v${FIFE_VERSION} using build type '${CMAKE_BUILD_TYPE}'.")
message("------------------------------------------------------------------------------")
message(STATUS "Source directory    -> '${PROJECT_SOURCE_DIR}'.")
message(STATUS "Build  directory    -> '${PROJECT_BINARY_DIR}'.")
message(STATUS "Installation path   -> '${CMAKE_INSTALL_PREFIX}' (set -DCMAKE_INSTALL_PREFIX=<path> to change).")
message(STATUS "Dependencies folder -> '${PROJECT_SOURCE_DIR}/../fifengine-dependencies'.")
message("")
message(STATUS "CMake version       -> '${CMAKE_VERSION}'.")
message(STATUS "CXX Compiler ID     -> '${CMAKE_CXX_COMPILER_ID}'.")
message(STATUS "CXX Compiler Flags  -> '${CMAKE_CXX_FLAGS}'.")
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
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/includes/include/AL"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/includes/lib"

    # help to find dependencies on your local env (FifeSDK)

    "${PROJECT_SOURCE_DIR}/../swig"
    "${PROJECT_SOURCE_DIR}/../python"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/bin"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/include"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/include/AL"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/include/SDL2"
    "${PROJECT_SOURCE_DIR}/../fifengine-dependencies/lib"

  )
endif(WIN32)

# Use vcpkg installed packages
set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
message(STATUS "Use CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

#------------------------------------------------------------------------------
#                                 Source Files
#------------------------------------------------------------------------------

# tell compiler to search in the following folders for include files
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src/engine")
include_directories("${PROJECT_SOURCE_DIR}")

# Core
set(FIFE_CORE_SRC
  ${PROJECT_SOURCE_DIR}/src/engine/audio/actionaudio.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundclip.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundclipmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundemitter.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundsource.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/audio/effects/soundeffect.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/audio/effects/soundeffectmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/audio/effects/soundfilter.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/controller/engine.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/controller/enginesettings.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/eventmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/joystick/joystick.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/joystick/joystickmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/audio/ogg_loader.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/audio/sounddecoder_ogg.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/input/controllermappingloader.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/animationloader.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/atlasloader.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/maploader.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/objectloader.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/percentdonelistener.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/video/imageloader.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/video/resourceanimationloader.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/model.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/action.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/object.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/timeprovider.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/grids/cellgrid.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/grids/hexgrid.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/grids/squaregrid.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/cell.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/cellcache.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/instance.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/instancetree.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/layer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/location.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/map.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/renderernode.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/trigger.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/triggercontroller.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/route.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/routepather/multilayersearch.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/routepather/routepather.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/routepather/routepathersearch.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/routepather/singlelayersearch.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/savers/native/input/controllermappingsaver.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/savers/native/map/mapsaver.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/util/base/exception.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/util/base/fifeclass.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/util/base/stringutils.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/util/log/logger.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/util/math/angles.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/util/resource/resource.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/util/time/timeevent.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/util/time/timemanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/util/time/timer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/directoryprovider.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/filesystem.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/vfs.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/vfsdirectory.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/vfssource.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/vfssourceprovider.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/dat1.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/dat2.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/lzssdecoder.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/rawdatadat1.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/rawdatadat2.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/raw/rawdata.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/raw/rawdatafile.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/raw/rawdatamemsource.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/raw/rawdatasource.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/zipfilesource.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/zipnode.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/zipprovider.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/zipsource.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/ziptree.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/animation.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/animationmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/atlasbook.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/color.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/cursor.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/devicecaps.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/image.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/imagemanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/renderbackend.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/fontbase.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/imagefontbase.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/subimagefont.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/textrenderpool.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/truetypefont.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/opengl/glimage.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/opengl/renderbackendopengl.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/sdl/renderbackendsdl.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/sdl/sdlblendingfunctions.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/video/sdl/sdlimage.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/camera.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/layercache.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/rendererbase.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderitem.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/visual.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/blockinginforenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/cellrenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/cellselectionrenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/coordinaterenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/floatingtextrenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/genericrenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/gridrenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/instancerenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/lightrenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/offrenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/quadtreerenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/targetrenderer.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/hybrid/hybridguimanager.cpp
)

# CEGUI v0.8.0+
set(FIFE_CEGUI_SRC
  ${PROJECT_SOURCE_DIR}/src/engine/gui/cegui/ceguimanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/cegui/base/ceguiinputprocessor.cpp
)

# FIFECHAN
set(FIFE_FCHAN_SRC
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/fifechanmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/gui_font.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/gui_image.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/gui_imageloader.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/opengl/opengl_gui_graphics.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/sdl/sdl_gui_graphics.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/console/commandline.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/console/console.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/util/utf8/utf8stringeditor.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/animationicon.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/clicklabel.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/dockarea.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/panel.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/percentagebar.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/resizablewindow.cpp
)

# LIBROCKET
set(FIFE_LROCKET_SRC
  ${PROJECT_SOURCE_DIR}/src/engine/gui/librocket/librocketmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/librocket/base/librocketinputprocessor.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/librocket/base/librocketrenderinterface.cpp
)

#------------------------------------------------------------------------------
#                         Source Groups for Source Files
#------------------------------------------------------------------------------

source_group("fife.core"                FILES ${FIFE_CORE_SRC})
source_group("fife.core.gui.cegui"      FILES ${FIFE_CEGUI_SRC})
source_group("fife.core.gui.fifechan"   FILES ${FIFE_FCHAN_SRC})
source_group("fife.core.gui.librocket"  FILES ${FIFE_LROCKET_SRC})

#------------------------------------------------------------------------------
#                                 Header Files
#------------------------------------------------------------------------------

# Core
set(FIFE_CORE_HDR
  ${PROJECT_SOURCE_DIR}/src/engine/export.h
  ${PROJECT_SOURCE_DIR}/src/engine/modules.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/actionaudio.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/fife_openal.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundclip.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundclipmanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundconfig.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/sounddecoder.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundemitter.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundmanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/soundsource.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/effects/soundeffect.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/effects/soundeffectmanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/audio/effects/soundfilter.h
  ${PROJECT_SOURCE_DIR}/src/engine/controller/engine.h
  ${PROJECT_SOURCE_DIR}/src/engine/controller/enginesettings.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/eventmanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/base/event.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/base/ilistener.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/base/inputevent.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/command/command.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/command/commandids.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/command/icommandcontroller.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/command/icommandlistener.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/drop/dropevent.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/drop/idropcontroller.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/drop/idroplistener.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/joystick/ijoystickcontroller.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/joystick/ijoysticklistener.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/joystick/joystick.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/joystick/joystickevent.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/joystick/joystickmanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/key/ikeycontroller.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/key/ikeyfilter.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/key/ikeylistener.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/key/key.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/key/keyevent.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/mouse/imousecontroller.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/mouse/imouselistener.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/mouse/mouseevent.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/sdl/isdleventcontroller.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/sdl/isdleventlistener.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/source/eventsourcetypes.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/source/ieventsource.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/text/itextcontroller.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/text/itextlistener.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/text/text.h
  ${PROJECT_SOURCE_DIR}/src/engine/eventchannel/text/textevent.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/audio/ogg_loader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/audio/sounddecoder_ogg.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/input/controllermappingloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/animationloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/atlasloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/ianimationloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/iatlasloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/imaploader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/iobjectloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/maploader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/objectloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/map/percentdonelistener.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/video/imageloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/loaders/native/video/resourceanimationloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/model.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/action.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/ipather.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/ivisual.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/modelcoords.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/object.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/timeprovider.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/grids/cellgrid.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/grids/hexgrid.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/metamodel/grids/squaregrid.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/cell.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/cellcache.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/instance.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/instancetree.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/layer.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/location.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/map.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/renderernode.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/trigger.h
  ${PROJECT_SOURCE_DIR}/src/engine/model/structures/triggercontroller.h
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/route.h
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/routepather/multilayersearch.h
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/routepather/routepather.h
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/routepather/routepathersearch.h
  ${PROJECT_SOURCE_DIR}/src/engine/pathfinder/routepather/singlelayersearch.h
  ${PROJECT_SOURCE_DIR}/src/engine/savers/native/input/controllermappingsaver.h
  ${PROJECT_SOURCE_DIR}/src/engine/savers/native/map/ianimationsaver.h
  ${PROJECT_SOURCE_DIR}/src/engine/savers/native/map/iatlassaver.h
  ${PROJECT_SOURCE_DIR}/src/engine/savers/native/map/imapsaver.h
  ${PROJECT_SOURCE_DIR}/src/engine/savers/native/map/iobjectsaver.h
  ${PROJECT_SOURCE_DIR}/src/engine/savers/native/map/mapsaver.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/base/exception.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/base/fifeclass.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/base/fife_stdint.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/base/sharedptr.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/base/singleton.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/base/stringutils.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/log/logger.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/math/angles.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/math/fife_math.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/math/matrix.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/resource/resource.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/resource/resourcemanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/structures/point.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/structures/priorityqueue.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/structures/purge.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/structures/quadtree.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/structures/rect.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/time/timeevent.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/time/timemanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/time/timer.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/utf8/utf8.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/utf8/utf8/checked.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/utf8/utf8/core.h
  ${PROJECT_SOURCE_DIR}/src/engine/util/utf8/utf8/unchecked.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/directoryprovider.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/filesystem.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/vfs.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/vfsdirectory.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/vfssource.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/vfssourceprovider.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/dat1.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/dat2.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/lzssdecoder.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/rawdatadat1.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/dat/rawdatadat2.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/raw/rawdata.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/raw/rawdatafile.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/raw/rawdatamemsource.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/raw/rawdatasource.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/zipfilesource.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/zipnode.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/zipprovider.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/zipsource.h
  ${PROJECT_SOURCE_DIR}/src/engine/vfs/zip/ziptree.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/animation.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/animationmanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/atlasbook.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/color.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/cursor.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/devicecaps.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/image.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/imagemanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/renderbackend.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/fontbase.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/ifont.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/imagefontbase.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/subimagefont.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/textrenderpool.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/fonts/truetypefont.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/opengl/fife_opengl.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/opengl/glimage.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/opengl/renderbackendopengl.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/sdl/renderbackendsdl.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/sdl/sdlblendingfunctions.h
  ${PROJECT_SOURCE_DIR}/src/engine/video/sdl/sdlimage.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/camera.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/layercache.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/rendererbase.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderitem.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/visual.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/blockinginforenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/cellrenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/cellselectionrenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/coordinaterenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/floatingtextrenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/genericrenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/gridrenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/instancerenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/lightrenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/offrenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/quadtreerenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/view/renderers/targetrenderer.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/guimanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/hybrid/hybridguimanager.h
)

# CEGUI
set(FIFE_CEGUI_HDR
  ${PROJECT_SOURCE_DIR}/src/engine/gui/cegui/ceguimanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/cegui/base/ceguiinputprocessor.h
)

# Fifechan
set(FIFE_FCHAN_HDR
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/fifechanmanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/gui_font.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/gui_image.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/gui_imageloader.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/opengl/opengl_gui_graphics.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/base/sdl/sdl_gui_graphics.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/console/commandline.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/console/console.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/util/utf8/utf8stringeditor.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/animationicon.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/clicklabel.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/dockarea.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/panel.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/percentagebar.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/fifechan/widgets/resizablewindow.h
)

# Librocket
set(FIFE_LROCKET_HDR
  ${PROJECT_SOURCE_DIR}/src/engine/gui/librocket/librocketmanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/librocket/base/librocketinputprocessor.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/librocket/base/librocketrenderinterface.h
)

#------------------------------------------------------------------------------
#                      Source Groups for Header Files
#------------------------------------------------------------------------------

source_group("fife.core"                FILES ${FIFE_CORE_HDR})
source_group("fife.core.gui.cegui"      FILES ${FIFE_CEGUI_HDR})
source_group("fife.core.gui.fifechan"   FILES ${FIFE_FCHAN_HDR})
source_group("fife.core.gui.librocket"  FILES ${FIFE_LROCKET_HDR})

add_custom_target(fife.headers SOURCES ${FIFE_CORE_HDR} ${FIFE_CEGUI_HDR} ${FIFE_FCHAN_HDR} ${FIFE_LROCKET_HDR})

#------------------------------------------------------------------------------
#                                 SWIG Files
#------------------------------------------------------------------------------

# Core
set(FIFE_CORE_SWIG
  version.i
  audio/effects/soundeffect.i
  audio/effects/soundfilter.i
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
set(FIFE_CEGUI_SWIG
  gui/cegui/ceguimanager.i
)

# FIFECHAN
set(FIFE_FCHAN_SWIG_LIB
  gui/fifechan/lib/fifechanlistener.i
  gui/fifechan/lib/widgets.i
)

set(FIFE_FCHAN_SWIG
  gui/fifechan/base/gui_font.i
  gui/fifechan/base/gui_image.i
  gui/fifechan/console/console.i
  gui/fifechan/fifechanmanager.i
  gui/fifechan/widgets/widgets.i
)

# LIBROCKET
set(FIFE_LROCKET_SWIG
  gui/librocket/librocketmanager.i
)

#------------------------------------------------------------------------------
#                      Source Groups for SWIG Files
#------------------------------------------------------------------------------

source_group("fife.core"                    FILES ${FIFE_CORE_SWIG})
source_group("fife.core.gui.cegui"          FILES ${FIFE_CEGUI_SWIG})
source_group("fife.core.gui.fifechan.lib"   FILES ${FIFE_FCHAN_SWIG_LIB})
source_group("fife.core.gui.fifechan"       FILES ${FIFE_FCHAN_SWIG})
source_group("fife.core.gui.librocket"      FILES ${FIFE_LROCKET_SWIG})

# TODO some files for this source_group are not found
#add_custom_target(fife.swig SOURCES ${FIFE_CORE_SWIG} ${FIFE_CEGUI_SWIG} ${FIFE_FCHAN_SWIG_LIB} ${FIFE_FCHAN_SWIG} ${FIFE_LROCKET_SWIG})

#------------------------------------------------------------------------------
#                                 SWIG WRAPPER Files
#------------------------------------------------------------------------------

# SWIG WRAPPERS FOR PYTHON
SET(FIFE_SWIGWRAPPERS_PYTHON
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/extensions.i.templ
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fife.i.templ
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fife.i.templ.cm
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fifechan.i.templ
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fifechan.i.templ.cm
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/librocket.i.templ
)

#------------------------------------------------------------------------------
#                      Source Groups for SWIG WRAPPER Files
#------------------------------------------------------------------------------

source_group("fife.swigwrappers.python" FILES ${FIFE_SWIGWRAPPERS_PYTHON})

add_custom_target(fife.swig-wrappers SOURCES ${FIFE_SWIGWRAPPERS_PYTHON})

#------------------------------------------------------------------------------
#                                 Find Packages
#------------------------------------------------------------------------------

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(TinyXML CONFIG REQUIRED)
find_package(Ogg CONFIG REQUIRED) # Ogg, not OGG
find_package(VORBIS CONFIG REQUIRED)
find_package(GLEW REQUIRED)

if(opengl)
  find_package(OpenGL REQUIRED)
  include_directories(${OPENGL_INCLUDE_DIR})
  find_package(GLEW REQUIRED)
  include_directories(${GLEW_INCLUDE_DIR})
endif(opengl)

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
endif(cegui)

if(build-python)
  # Setting up SWIG files
  set(SWIG_MODULE_fife_EXTRA_DEPS)
  set(CMAKE_SWIG_IMPORTS "")
  set(CMAKE_SWIG_INCLUSIONS "")
  set(CMAKE_SWIG_IMPORTS_FCHAN "")
  set(CMAKE_SWIG_INCLUSIONS_FCHAN "")

  # Find python library and add include path for python headers
  find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
  message(STATUS "Python3 includes: ${Python3_INCLUDE_DIRS}")
  message(STATUS "Python3 libraries: ${Python3_LIBRARIES}")

  foreach(filename ${FIFE_CORE_SWIG})
    list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/src/engine/${filename}")
    set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
  endforeach(filename ${FIFE_CORE_SWIG})
endif(build-python)

if(fifechan)

  find_package(FIFECHAN COMPONENTS sdl REQUIRED)
  if(opengl)
    find_package(FIFECHAN COMPONENTS opengl REQUIRED)
  endif(opengl)

  if(build-python)
    set(SWIG_MODULE_fifechan_EXTRA_DEPS)
    foreach(filename ${FIFE_FCHAN_SWIG_LIB})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/src/engine/${filename}")
      list(APPEND SWIG_MODULE_fifechan_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/src/engine/${filename}")
      set(CMAKE_SWIG_INCLUSIONS_FCHAN "${CMAKE_SWIG_INCLUSIONS_FCHAN}%include ${filename}\n")
    endforeach(filename ${FIFE_FCHAN_SWIG_LIB})

    foreach(filename ${FIFE_FCHAN_SWIG})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/src/engine/${filename}")
      list(APPEND SWIG_MODULE_fifechan_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/src/engine/${filename}")
      set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
    endforeach(filename ${FIFE_FCHAN_SWIG})
    if (PYTHON_VERSION_MAJOR LESS 3)
      set(CMAKE_SWIG_IMPORTS "${CMAKE_SWIG_IMPORTS}%import ${PROJECT_BINARY_DIR}/fifechan.i")
    else (PYTHON_VERSION_MAJOR LESS 3)
      set(CMAKE_SWIG_IMPORTS "${CMAKE_SWIG_IMPORTS}%import(module=\"fife.fifechan\") ${PROJECT_BINARY_DIR}/fifechan.i")
    endif (PYTHON_VERSION_MAJOR LESS 3)


    configure_file(
      "${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fifechan.i.templ.cm"
      "${PROJECT_BINARY_DIR}/fifechan.i"
    )
  endif(build-python)

  include_directories(${FIFECHAN_INCLUDE_DIR})

endif(fifechan)

if(cegui)
  if(build-python)
    foreach(filename ${FIFE_CEGUI_SWIG})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${PROJECT_SOURCE_DIR}/src/engine/${filename}")
      set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
    endforeach(filename ${FIFE_CEGUI_SWIG})
   endif(build-python)
endif(cegui)

if(build-python)

  configure_file(
    "${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fife.i.templ.cm"
    "${PROJECT_BINARY_DIR}/fife.i"
  )

  find_package(SWIG 4.0.0 REQUIRED)

  include(${SWIG_USE_FILE})

endif(build-python)

include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${OPENALSOFT_INCLUDE_DIR})
include_directories(${OGG_INCLUDE_DIRS})
include_directories(${VORBIS_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})

add_definitions(${PNG_DEFINITIONS})

#------------------------------------------------------------------------------
#                  Build fifengine as a Python extension module
#------------------------------------------------------------------------------

if(build-python)
  # Find python library and add include path for python headers
  find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
  message(STATUS "Python3 includes: ${Python3_INCLUDE_DIRS}")
  message(STATUS "Python3 libraries: ${Python3_LIBRARIES}")

  #set(PYTHON_LIBRARY "PYTHON_LIBRARY-NOTFOUND")
 #set(PYTHON_INCLUDE_DIR "PYTHON_INCLUDE_DIR-NOTFOUND")

  include_directories(${PYTHON_INCLUDE_PATH})

  set(CMAKE_SWIG_FLAGS -fastdispatch -dirvtable -fastproxy -keyword -w511 -w473 -w404 -w314)

  set_source_files_properties("${PROJECT_BINARY_DIR}/fife.i" PROPERTIES CPLUSPLUS ON)
  set(FIFE_SOURCES ${FIFE_CORE_SRC})
  if(fifechan)
    set(FIFE_SOURCES ${FIFE_SOURCES} ${FIFE_FCHAN_SRC})
  endif(fifechan)
  # CMake 3.8: swig_add_module is deprecated, use swig_add_library instead.
  if(${CMAKE_VERSION} VERSION_LESS 3.8)
    swig_add_module(fife python "${PROJECT_BINARY_DIR}/fife.i" ${FIFE_SOURCES})
  else(${CMAKE_VERSION} VERSION_LESS 3.8)
    swig_add_library(fife LANGUAGE python SOURCES "${PROJECT_BINARY_DIR}/fife.i" ${FIFE_SOURCES})
  endif(${CMAKE_VERSION} VERSION_LESS 3.8)

  if(APPLE)
    set(APPLE_DYNAMIC_LOOKUP_FLAG "-undefined dynamic_lookup")
    swig_link_libraries(fife ${APPLE_DYNAMIC_LOOKUP_FLAG} )
  else(APPLE)
    swig_link_libraries(fife ${PYTHON_LIBRARIES})
  endif(APPLE)
  swig_link_libraries(fife ${SDL2_LIBRARY}
                           ${SDL2_IMAGE_LIBRARIES}
                           ${SDL2_TTF_LIBRARIES}
                           ${OPENALSOFT_LIBRARY})
  swig_link_libraries(fife ${PNG_LIBRARIES})
  swig_link_libraries(fife ${VORBISFILE_LIBRARY})
  swig_link_libraries(fife ${VORBIS_LIBRARY})
  swig_link_libraries(fife ${OGG_LIBRARIES})
  swig_link_libraries(fife ${TinyXML_LIBRARIES})

  if(opengl)
    swig_link_libraries(fife ${OPENGL_gl_LIBRARY})
    swig_link_libraries(fife ${GLEW_LIBRARY})
  endif(opengl)

  if(fifechan)
    set_source_files_properties("${PROJECT_BINARY_DIR}/fifechan.i" PROPERTIES CPLUSPLUS ON)

    # CMake 3.8: swig_add_module is deprecated, use swig_add_library instead.
    if(${CMAKE_VERSION} VERSION_LESS 3.8)
      swig_add_module(fifechan python "${PROJECT_BINARY_DIR}/fifechan.i")
    else(${CMAKE_VERSION} VERSION_LESS 3.8)
      swig_add_library(fifechan LANGUAGE python SOURCES "${PROJECT_BINARY_DIR}/fifechan.i")
    endif(${CMAKE_VERSION} VERSION_LESS 3.8)

    if(APPLE)
      swig_link_libraries(fifechan ${APPLE_DYNAMIC_LOOKUP_FLAG})
    else(APPLE)
      swig_link_libraries(fifechan ${PYTHON_LIBRARIES})
    endif(APPLE)
    swig_link_libraries(fifechan ${FIFECHAN_LIBRARIES})
    swig_link_libraries(fife ${FIFECHAN_LIBRARIES})
  endif(fifechan)

  if(cegui)
    swig_link_libraries(fife ${CEGUI_LIBRARIES})
    swig_link_libraries(fife ${CEGUI_OpenGLRenderer_LIBRARIES})
  endif(cegui)

   #---------------------------------------------------------------------------------
   # Install Python package locally, e.g. into "C:\Python27\Lib\site-packages\fife" #---------------------------------------------------------------------------------

   if(NOT DEFINED PYTHON_SITE_PACKAGES)
    # get path to your local python site packages folder
    execute_process(
      COMMAND  ${PYTHON_EXECUTABLE} -c "import sys; from distutils.sysconfig import get_python_lib; sys.stdout.write(get_python_lib(True))"
      OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
    )
    # Slash-Fix: Cmake would create a warning, when using the variable in FILES or DIRECTORY without changing it.
    file(TO_CMAKE_PATH ${PYTHON_SITE_PACKAGES} PYTHON_SITE_PACKAGES)
  endif(NOT DEFINED PYTHON_SITE_PACKAGES)

  message(STATUS "    Python install directory is '${PYTHON_SITE_PACKAGES}/fife'.")

  # install pythonized fife
  install(TARGETS     _fife															DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  install(FILES       ${PROJECT_BINARY_DIR}/fife.py									DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  install(FILES       ${CMAKE_SOURCE_DIR}/engine/python/fife/__init__.py			DESTINATION ${PYTHON_SITE_PACKAGES}/fife)

  # copy all files/folders from the extension folder
  # but exclude extension folders of the optional dependencies: cegui, librocket, pychan
  install(DIRECTORY   ${CMAKE_SOURCE_DIR}/engine/python/fife/extensions/			DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions
	PATTERN "extensions/cegui"	   EXCLUDE
	PATTERN "extensions/librocket" EXCLUDE
	PATTERN "extensions/pychan"    EXCLUDE
    PATTERN "extensions/*"
  )

  if(cegui)
    # install fife's extension for cegui
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/engine/python/fife/extensions/cegui/		DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions/cegui)
  endif(cegui)

  if(librocket)
    # install fife's extension for librocket
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/engine/python/fife/extensions/librocket/	DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions/librocket)
  endif(librocket)

  if(fifechan)
    # install pychan - fife's extension for fifechan
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/engine/python/fife/extensions/pychan/		DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions/pychan)

    # install pythonized fifechan
    install(TARGETS   _fifechan                           DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
    install(FILES     ${PROJECT_BINARY_DIR}/fifechan.py   DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  endif(fifechan)
endif(build-python)

#------------------------------------------------------------------------------
#                       Build fifengine as a library
#------------------------------------------------------------------------------

if(build-library)
  set(FIFE_SOURCES ${FIFE_CORE_SRC})
  if(fifechan)
    set(FIFE_SOURCES ${FIFE_SOURCES} ${FIFE_FCHAN_SRC})
  endif(fifechan)

  add_library(fife ${FIFE_SOURCES})
  set_target_properties(fife PROPERTIES SOVERSION ${FIFE_VERSION})

if(VCPKG_TARGET_TRIPLET)
    # Add your code here for VCPKG-specific configurations
    target_link_libraries(fife PRIVATE
        ${ZLIB_LIBRARIES}
        ${PNG_LIBRARIES}
        ${GLEW_LIBRARY}
        ${OGG_LIBRARIES}
        ${VORBIS_LIBRARY}
        ${VORBISFILE_LIBRARY}
        ${VORBISENC_LIBRARY}
        ${OPENALSOFT_LIBRARY}
    )
    target_link_libraries(fife PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2>
    )
    target_link_libraries(fife PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2_image::SDL2_image>
    )
    target_link_libraries(fife PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2_ttf::SDL2_ttf>
    )
    target_link_libraries(fife PRIVATE ${TinyXML_LIBRARIES})
else()

target_link_libraries(fife PRIVATE
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${OPENALSOFT_LIBRARY}
    ${PNG_LIBRARIES}
    ${VORBISFILE_LIBRARY}
    ${VORBIS_LIBRARY}
    ${OGG_LIBRARIES}
    ${TinyXML_LIBRARIES}
)

endif()

  if(opengl)
    target_link_libraries(fife PRIVATE ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})
  endif(opengl)
  if(fifechan)
    target_link_libraries(fife PRIVATE ${FIFECHAN_LIBRARIES})
  endif(fifechan)
  if(cegui)
    target_link_libraries(fife PRIVATE ${CEGUI_LIBRARIES} ${CEGUI_OpenGLRenderer_LIBRARIES})
  endif(cegui)

  # include support for GNU / multiarch path names
  # https://www.gnu.org/prep/standards/html_node/Directory-Variables.html
  include(GNUInstallDirs)

  install(TARGETS fife DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(FILES ${PROJECT_SOURCE_DIR}/src/engine/version.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fife/core/)

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
  if(librocket)
    INSTALL_HEADERS_WITH_DIRECTORY(FIFE_LROCKET_HDR)
  endif(librocket)
endif(build-library)

#-------------------------------------------------------------------
# Display Feature Summary
#-------------------------------------------------------------------

feature_summary(
  WHAT ALL
  DESCRIPTION "[Build Summary]\n"
  FATAL_ON_MISSING_REQUIRED_PACKAGES
)
