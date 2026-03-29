#------------------------------------------------------------------------------
#                                 Fifengine
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.25...4.2)

# include additional CMake scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

#-------------------------------------------------------------------
# Setup VCPKG and install Dependencies
#-------------------------------------------------------------------

# USE_VCPKG defaults to true, so that SetupVcpkg is included.
set(USE_VCPKG TRUE CACHE BOOL "Use vcpkg for dependencies")

# This configures VCPKG to install the dependencies defined in vcpkg.json
# automatically during the configure step of CMake (vcpkg install).
if(USE_VCPKG)
   include(SetupVcpkg)
endif()

#-------------------------------------------------------------------
# Gather Version Data
#-------------------------------------------------------------------

# vcpkg.json is the primary source for version data.
# FIFE_VERSION is used to set the project version.
# FIFE_VERSION and the other tokens are used in version.hpp.in.
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json VCPKG_JSON_STRING)
string(JSON FIFE_NAME     GET ${VCPKG_JSON_STRING} "name")
string(JSON FIFE_VERSION  GET ${VCPKG_JSON_STRING} "version-string")
string(JSON FIFE_LICENSE  GET ${VCPKG_JSON_STRING} "license")
string(JSON FIFE_DESC     GET ${VCPKG_JSON_STRING} "description")
string(JSON FIFE_HOMEPAGE GET ${VCPKG_JSON_STRING} "homepage")
string(TIMESTAMP CURRENT_YEAR "%Y")

#------------------------------------------------------------------------------
# Project
#------------------------------------------------------------------------------

project(fifengine VERSION ${FIFE_VERSION} LANGUAGES CXX)

#-------------------------------------------------------------------
# Versionize the project
#-------------------------------------------------------------------

# Here we populate the version.hpp file with version data.
# This must be done after project() is called,
# else the version variables are not available (PROJECT_* tokens).
# See the version.hpp.in file for the tokens that are replaced.
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/version.hpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/version.hpp"
  @ONLY
)

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------

option(BUILD_SHARED_LIBS      "Build using shared libraries"                 ON)

option(ENABLE_OPENGL          "Enable OpenGL support"                        ON)
option(ENABLE_FIFEGUI         "Enable Fifechan GUI subsystem"                ON)
option(ENABLE_CEGUI           "Enable Crazy Eddie's GUI subsystem"          OFF)
option(ENABLE_LOGGING         "Enable logging"                               ON)
option(BUILD_PYTHON_BINDINGS  "Build python extension module"               OFF)
option(BUILD_LIBRARY          "Build C++ library"                            ON)
option(BUILD_TESTS            "Build C++ tests"                             OFF)

# Reminder: The variable USE_VCPKG exists, which controls SetupVcpkg.
# It is not a build option, because it must be set before project().

#------------------------------------------------------------------------------
# Global compile definitions as INTERFACE library
#------------------------------------------------------------------------------

add_library(fife_options INTERFACE)

if(WIN32)
  target_compile_definitions(fife_options INTERFACE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

if(ENABLE_LOGGING)
  target_compile_definitions(fife_options INTERFACE LOG_ENABLED)
endif()

if(ENABLE_OPENGL)
  target_compile_definitions(fife_options INTERFACE HAVE_OPENGL)
endif()

if(ENABLE_CEGUI)
  target_compile_definitions(fife_options INTERFACE HAVE_CEGUI)
endif()

#-------------------------------------------------------------------
# CMake Includes
#-------------------------------------------------------------------

# Project local includes
include(ShowBuildTargetProperties)
if(WIN32)
  # Install runtime DLLs on Windows
  include(InstallRuntimeDeps)
endif()

# CMake provided includes
include(FeatureSummary)
include(GNUInstallDirs)

#------------------------------------------------------------------------------
# Configure
#------------------------------------------------------------------------------

# TODO is this still needed?
# Ensure SWIG python template files are available in the build tree.
# this allows out-of-source builds and the SWIG invocation to find
# `engine/swigwrappers/python/extensions.i.templ` without manual copying.
if(EXISTS "${CMAKE_SOURCE_DIR}/src/swigwrappers/python")
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/engine/swigwrappers/python")
  file(COPY "${CMAKE_SOURCE_DIR}/src/swigwrappers/python/" DESTINATION "${CMAKE_BINARY_DIR}/engine/swigwrappers/python")
  message(STATUS "Copied SWIG templates to ${CMAKE_BINARY_DIR}/engine/swigwrappers/python")
endif()

# Create a "Release" build by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Select build type. Options are: 'Debug', 'Release', 'RelWithDebInfo' and 'MinSizeRel'."
    FORCE
  )
  message(STATUS "Set CMAKE_BUILD_TYPE to default '${CMAKE_BUILD_TYPE}'.")
endif()

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
# Compiler Options
#------------------------------------------------------------------------------

# TODO enforce globally. change this to target_compile_options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC compiler options
# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170
if(WIN32 AND MSVC)
  add_compile_options(/nologo) # Suppress Startup Banner
  add_compile_options(/MP)     # Build on all cores
  add_compile_options(/Gm-)    # Disable minimal rebuild

  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(/W0) # Disable warnings

    add_compile_options(/GF) # Enables string pooling
    add_compile_options(/GS) # Buffer security checks

    # Enable optimizations /O2?
    add_compile_options(/Oi) # generate intrinsic functions

    if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	  add_compile_options(/Z7) # Enable debug symbols
	endif()
  else()
    # Debug build
    add_compile_options(/W3) # warning level: /W<level 0 to 4> or /Wall
    add_compile_options(/Od) # disable optimizations
  endif()

  # avoid "C1128: number of sections exceeded object file format limit: compile with /bigobj"
  add_compile_options(/bigobj)

endif()

# GCC and Clang compiler options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wno-error")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g2 -Wall -Wextra -Wunused")
  endif()
endif()

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
# Find Dependencies (non-vcpkg)
#------------------------------------------------------------------------------

# Add the install location of the non-vcpkg dependencies to CMAKE_PREFIX_PATH
# to allow find_package to find them. This is needed for SWIG and Fifechan.
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/out/fife-dependencies/install")

#------------------------------------------------------------------------------
# Display Status
#------------------------------------------------------------------------------

message(STATUS "---------------------------------------------------------------------")
message(STATUS "[ 🛠️ ] Building FIFE v${FIFE_VERSION} using build type '${CMAKE_BUILD_TYPE}'.")
message(STATUS "---------------------------------------------------------------------")
message(STATUS "[INFO] Source dir               -> '${PROJECT_SOURCE_DIR}'.")
message(STATUS "[INFO] Build  directory         -> '${PROJECT_BINARY_DIR}'.")
message(STATUS "[INFO] Installation path        -> '${CMAKE_INSTALL_PREFIX}' (set -DCMAKE_INSTALL_PREFIX=<path> to change).")
message(STATUS "[INFO] Dependencies folder      -> '${PROJECT_SOURCE_DIR}/out/fife-dependencies'.")
message(STATUS "")
message(STATUS "[INFO] Build shared libraries   -> '${BUILD_SHARED_LIBS}'.")
message(STATUS "")
message(STATUS "[INFO] Enable OpenGL support               -> '${ENABLE_OPENGL}'")
message(STATUS "[INFO] Enable Fifechan GUI subsystem       -> '${ENABLE_FIFEGUI}'")
message(STATUS "[INFO] Enable Crazy Eddie's GUI subsystem  -> '${ENABLE_CEGUI}'")
message(STATUS "[INFO] Enable logging                      -> '${ENABLE_LOGGING}'")
message(STATUS "[INFO] Build the python extension module   -> '${BUILD_PYTHON_BINDINGS}'")
message(STATUS "[INFO] Build C++ library                   -> '${BUILD_LIBRARY}'")
message(STATUS "[INFO] Build C++ tests                     -> '${BUILD_TESTS}'")
message(STATUS "")
message(STATUS "[INFO] CMake version            -> '${CMAKE_VERSION}'.")
message(STATUS "[INFO] CXX Compiler ID          -> '${CMAKE_CXX_COMPILER_ID}'.")
message(STATUS "[INFO] CXX Compiler Flags       -> '${CMAKE_CXX_FLAGS}'.")
message(STATUS "---------------------------------------------------------------------")

#------------------------------------------------------------------------------
# Source Files
#------------------------------------------------------------------------------

# Core
set(
  FIFE_CORE_SRC
  src/engine/audio/actionaudio.cpp
  src/engine/audio/soundclip.cpp
  src/engine/audio/soundclipmanager.cpp
  src/engine/audio/soundemitter.cpp
  src/engine/audio/soundmanager.cpp
  src/engine/audio/soundsource.cpp
  src/engine/audio/effects/soundeffect.cpp
  src/engine/audio/effects/soundeffectmanager.cpp
  src/engine/audio/effects/soundfilter.cpp
  src/engine/controller/engine.cpp
  src/engine/controller/enginesettings.cpp
  src/engine/eventchannel/eventmanager.cpp
  src/engine/eventchannel/joystick/joystick.cpp
  src/engine/eventchannel/joystick/joystickmanager.cpp
  src/engine/loaders/native/audio/ogg_loader.cpp
  src/engine/loaders/native/audio/sounddecoder_ogg.cpp
  src/engine/loaders/native/input/controllermappingloader.cpp
  src/engine/loaders/native/map/animationloader.cpp
  src/engine/loaders/native/map/atlasloader.cpp
  src/engine/loaders/native/map/maploader.cpp
  src/engine/loaders/native/map/objectloader.cpp
  src/engine/loaders/native/map/percentdonelistener.cpp
  src/engine/loaders/native/video/imageloader.cpp
  src/engine/loaders/native/video/resourceanimationloader.cpp
  src/engine/model/model.cpp
  src/engine/model/metamodel/action.cpp
  src/engine/model/metamodel/object.cpp
  src/engine/model/metamodel/timeprovider.cpp
  src/engine/model/metamodel/grids/cellgrid.cpp
  src/engine/model/metamodel/grids/hexgrid.cpp
  src/engine/model/metamodel/grids/squaregrid.cpp
  src/engine/model/structures/cell.cpp
  src/engine/model/structures/cellcache.cpp
  src/engine/model/structures/instance.cpp
  src/engine/model/structures/instancetree.cpp
  src/engine/model/structures/layer.cpp
  src/engine/model/structures/location.cpp
  src/engine/model/structures/map.cpp
  src/engine/model/structures/renderernode.cpp
  src/engine/model/structures/trigger.cpp
  src/engine/model/structures/triggercontroller.cpp
  src/engine/pathfinder/route.cpp
  src/engine/pathfinder/routepather/multilayersearch.cpp
  src/engine/pathfinder/routepather/routepather.cpp
  src/engine/pathfinder/routepather/routepathersearch.cpp
  src/engine/pathfinder/routepather/singlelayersearch.cpp
  src/engine/savers/native/input/controllermappingsaver.cpp
  src/engine/savers/native/map/mapsaver.cpp
  src/engine/util/base/exception.cpp
  src/engine/util/base/fifeclass.cpp
  src/engine/util/base/stringutils.cpp
  src/engine/util/log/logger.cpp
  src/engine/util/math/angles.cpp
  src/engine/util/resource/resource.cpp
  src/engine/util/time/timeevent.cpp
  src/engine/util/time/timemanager.cpp
  src/engine/util/time/timer.cpp
  src/engine/vfs/directoryprovider.cpp
  src/engine/vfs/filesystem.cpp
  src/engine/vfs/vfs.cpp
  src/engine/vfs/vfsdirectory.cpp
  src/engine/vfs/vfssource.cpp
  src/engine/vfs/vfssourceprovider.cpp
  src/engine/vfs/dat/dat1.cpp
  src/engine/vfs/dat/dat2.cpp
  src/engine/vfs/dat/lzssdecoder.cpp
  src/engine/vfs/dat/rawdatadat1.cpp
  src/engine/vfs/dat/rawdatadat2.cpp
  src/engine/vfs/raw/rawdata.cpp
  src/engine/vfs/raw/rawdatafile.cpp
  src/engine/vfs/raw/rawdatamemsource.cpp
  src/engine/vfs/raw/rawdatasource.cpp
  src/engine/vfs/zip/zipfilesource.cpp
  src/engine/vfs/zip/zipnode.cpp
  src/engine/vfs/zip/zipprovider.cpp
  src/engine/vfs/zip/zipsource.cpp
  src/engine/vfs/zip/ziptree.cpp
  src/engine/video/animation.cpp
  src/engine/video/animationmanager.cpp
  src/engine/video/atlasbook.cpp
  src/engine/video/color.cpp
  src/engine/video/cursor.cpp
  src/engine/video/devicecaps.cpp
  src/engine/video/image.cpp
  src/engine/video/imagemanager.cpp
  src/engine/video/renderbackend.cpp
  src/engine/video/fonts/fontbase.cpp
  src/engine/video/fonts/imagefontbase.cpp
  src/engine/video/fonts/subimagefont.cpp
  src/engine/video/fonts/textrenderpool.cpp
  src/engine/video/fonts/truetypefont.cpp
  src/engine/video/opengl/glimage.cpp
  src/engine/video/opengl/renderbackendopengl.cpp
  src/engine/video/sdl/renderbackendsdl.cpp
  src/engine/video/sdl/sdlblendingfunctions.cpp
  src/engine/video/sdl/sdlimage.cpp
  src/engine/view/camera.cpp
  src/engine/view/layercache.cpp
  src/engine/view/rendererbase.cpp
  src/engine/view/renderitem.cpp
  src/engine/view/visual.cpp
  src/engine/view/renderers/blockinginforenderer.cpp
  src/engine/view/renderers/cellrenderer.cpp
  src/engine/view/renderers/cellselectionrenderer.cpp
  src/engine/view/renderers/coordinaterenderer.cpp
  src/engine/view/renderers/floatingtextrenderer.cpp
  src/engine/view/renderers/genericrenderer.cpp
  src/engine/view/renderers/gridrenderer.cpp
  src/engine/view/renderers/instancerenderer.cpp
  src/engine/view/renderers/lightrenderer.cpp
  src/engine/view/renderers/offrenderer.cpp
  src/engine/view/renderers/quadtreerenderer.cpp
  src/engine/view/renderers/targetrenderer.cpp
)

# CEGUI v0.8.0+
set(
  FIFE_CEGUI_SRC
  src/engine/gui/cegui/ceguimanager.cpp
  src/engine/gui/cegui/base/ceguiinputprocessor.cpp
)

# FIFECHAN
set(
  FIFE_FCHAN_SRC
  src/engine/gui/fifechan/fifechanmanager.cpp
  src/engine/gui/fifechan/base/gui_font.cpp
  src/engine/gui/fifechan/base/gui_image.cpp
  src/engine/gui/fifechan/base/gui_imageloader.cpp
  src/engine/gui/fifechan/base/opengl/opengl_gui_graphics.cpp
  src/engine/gui/fifechan/base/sdl/sdl_gui_graphics.cpp
  src/engine/gui/fifechan/console/commandline.cpp
  src/engine/gui/fifechan/console/console.cpp
  src/engine/gui/fifechan/util/utf8/utf8stringeditor.cpp
  src/engine/gui/fifechan/widgets/animationicon.cpp
  src/engine/gui/fifechan/widgets/clicklabel.cpp
  src/engine/gui/fifechan/widgets/dockarea.cpp
  src/engine/gui/fifechan/widgets/panel.cpp
  src/engine/gui/fifechan/widgets/percentagebar.cpp
  src/engine/gui/fifechan/widgets/resizablewindow.cpp
)

#------------------------------------------------------------------------------
# Source Groups for Source Files
#------------------------------------------------------------------------------

source_group("fife.core"                FILES ${FIFE_CORE_SRC})
source_group("fife.core.gui.cegui"      FILES ${FIFE_CEGUI_SRC})
source_group("fife.core.gui.fifechan"   FILES ${FIFE_FCHAN_SRC})

#------------------------------------------------------------------------------
# Header Files
#------------------------------------------------------------------------------

# Core
set(
  FIFE_CORE_HDR
  src/engine/export.h
  src/engine/modules.h
  src/engine/audio/actionaudio.h
  src/engine/audio/fife_openal.h
  src/engine/audio/soundclip.h
  src/engine/audio/soundclipmanager.h
  src/engine/audio/soundconfig.h
  src/engine/audio/sounddecoder.h
  src/engine/audio/soundemitter.h
  src/engine/audio/soundmanager.h
  src/engine/audio/soundsource.h
  src/engine/audio/effects/soundeffect.h
  src/engine/audio/effects/soundeffectmanager.h
  src/engine/audio/effects/soundfilter.h
  src/engine/controller/engine.h
  src/engine/controller/enginesettings.h
  src/engine/eventchannel/eventmanager.h
  src/engine/eventchannel/base/event.h
  src/engine/eventchannel/base/ilistener.h
  src/engine/eventchannel/base/inputevent.h
  src/engine/eventchannel/command/command.h
  src/engine/eventchannel/command/commandids.h
  src/engine/eventchannel/command/icommandcontroller.h
  src/engine/eventchannel/command/icommandlistener.h
  src/engine/eventchannel/drop/dropevent.h
  src/engine/eventchannel/drop/idropcontroller.h
  src/engine/eventchannel/drop/idroplistener.h
  src/engine/eventchannel/joystick/ijoystickcontroller.h
  src/engine/eventchannel/joystick/ijoysticklistener.h
  src/engine/eventchannel/joystick/joystick.h
  src/engine/eventchannel/joystick/joystickevent.h
  src/engine/eventchannel/joystick/joystickmanager.h
  src/engine/eventchannel/key/ikeycontroller.h
  src/engine/eventchannel/key/ikeyfilter.h
  src/engine/eventchannel/key/ikeylistener.h
  src/engine/eventchannel/key/key.h
  src/engine/eventchannel/key/keyevent.h
  src/engine/eventchannel/mouse/imousecontroller.h
  src/engine/eventchannel/mouse/imouselistener.h
  src/engine/eventchannel/mouse/mouseevent.h
  src/engine/eventchannel/sdl/isdleventcontroller.h
  src/engine/eventchannel/sdl/isdleventlistener.h
  src/engine/eventchannel/source/eventsourcetypes.h
  src/engine/eventchannel/source/ieventsource.h
  src/engine/eventchannel/text/itextcontroller.h
  src/engine/eventchannel/text/itextlistener.h
  src/engine/eventchannel/text/text.h
  src/engine/eventchannel/text/textevent.h
  src/engine/loaders/native/audio/ogg_loader.h
  src/engine/loaders/native/audio/sounddecoder_ogg.h
  src/engine/loaders/native/input/controllermappingloader.h
  src/engine/loaders/native/map/animationloader.h
  src/engine/loaders/native/map/atlasloader.h
  src/engine/loaders/native/map/ianimationloader.h
  src/engine/loaders/native/map/iatlasloader.h
  src/engine/loaders/native/map/imaploader.h
  src/engine/loaders/native/map/iobjectloader.h
  src/engine/loaders/native/map/maploader.h
  src/engine/loaders/native/map/objectloader.h
  src/engine/loaders/native/map/percentdonelistener.h
  src/engine/loaders/native/video/imageloader.h
  src/engine/loaders/native/video/resourceanimationloader.h
  src/engine/model/model.h
  src/engine/model/metamodel/action.h
  src/engine/model/metamodel/ipather.h
  src/engine/model/metamodel/ivisual.h
  src/engine/model/metamodel/modelcoords.h
  src/engine/model/metamodel/object.h
  src/engine/model/metamodel/timeprovider.h
  src/engine/model/metamodel/grids/cellgrid.h
  src/engine/model/metamodel/grids/hexgrid.h
  src/engine/model/metamodel/grids/squaregrid.h
  src/engine/model/structures/cell.h
  src/engine/model/structures/cellcache.h
  src/engine/model/structures/instance.h
  src/engine/model/structures/instancetree.h
  src/engine/model/structures/layer.h
  src/engine/model/structures/location.h
  src/engine/model/structures/map.h
  src/engine/model/structures/renderernode.h
  src/engine/model/structures/trigger.h
  src/engine/model/structures/triggercontroller.h
  src/engine/pathfinder/route.h
  src/engine/pathfinder/routepather/multilayersearch.h
  src/engine/pathfinder/routepather/routepather.h
  src/engine/pathfinder/routepather/routepathersearch.h
  src/engine/pathfinder/routepather/singlelayersearch.h
  src/engine/savers/native/input/controllermappingsaver.h
  src/engine/savers/native/map/ianimationsaver.h
  src/engine/savers/native/map/iatlassaver.h
  src/engine/savers/native/map/imapsaver.h
  src/engine/savers/native/map/iobjectsaver.h
  src/engine/savers/native/map/mapsaver.h
  src/engine/util/base/exception.h
  src/engine/util/base/fifeclass.h
  src/engine/util/base/fife_stdint.h
  src/engine/util/base/sharedptr.h
  src/engine/util/base/singleton.h
  src/engine/util/base/stringutils.h
  src/engine/util/log/logger.h
  src/engine/util/math/angles.h
  src/engine/util/math/fife_math.h
  src/engine/util/math/matrix.h
  src/engine/util/resource/resource.h
  src/engine/util/resource/resourcemanager.h
  src/engine/util/structures/point.h
  src/engine/util/structures/priorityqueue.h
  src/engine/util/structures/purge.h
  src/engine/util/structures/quadtree.h
  src/engine/util/structures/rect.h
  src/engine/util/time/timeevent.h
  src/engine/util/time/timemanager.h
  src/engine/util/time/timer.h
  src/engine/util/utf8/utf8.h
  src/engine/util/utf8/utf8/checked.h
  src/engine/util/utf8/utf8/core.h
  src/engine/util/utf8/utf8/unchecked.h
  src/engine/vfs/directoryprovider.h
  src/engine/vfs/filesystem.h
  src/engine/vfs/vfs.h
  src/engine/vfs/vfsdirectory.h
  src/engine/vfs/vfssource.h
  src/engine/vfs/vfssourceprovider.h
  src/engine/vfs/dat/dat1.h
  src/engine/vfs/dat/dat2.h
  src/engine/vfs/dat/lzssdecoder.h
  src/engine/vfs/dat/rawdatadat1.h
  src/engine/vfs/dat/rawdatadat2.h
  src/engine/vfs/raw/rawdata.h
  src/engine/vfs/raw/rawdatafile.h
  src/engine/vfs/raw/rawdatamemsource.h
  src/engine/vfs/raw/rawdatasource.h
  src/engine/vfs/zip/zipfilesource.h
  src/engine/vfs/zip/zipnode.h
  src/engine/vfs/zip/zipprovider.h
  src/engine/vfs/zip/zipsource.h
  src/engine/vfs/zip/ziptree.h
  src/engine/video/animation.h
  src/engine/video/animationmanager.h
  src/engine/video/atlasbook.h
  src/engine/video/color.h
  src/engine/video/cursor.h
  src/engine/video/devicecaps.h
  src/engine/video/image.h
  src/engine/video/imagemanager.h
  src/engine/video/renderbackend.h
  src/engine/video/fonts/fontbase.h
  src/engine/video/fonts/ifont.h
  src/engine/video/fonts/imagefontbase.h
  src/engine/video/fonts/subimagefont.h
  src/engine/video/fonts/textrenderpool.h
  src/engine/video/fonts/truetypefont.h
  src/engine/video/opengl/fife_opengl.h
  src/engine/video/opengl/glimage.h
  src/engine/video/opengl/renderbackendopengl.h
  src/engine/video/sdl/renderbackendsdl.h
  src/engine/video/sdl/sdlblendingfunctions.h
  src/engine/video/sdl/sdlimage.h
  src/engine/view/camera.h
  src/engine/view/layercache.h
  src/engine/view/rendererbase.h
  src/engine/view/renderitem.h
  src/engine/view/visual.h
  src/engine/view/renderers/blockinginforenderer.h
  src/engine/view/renderers/cellrenderer.h
  src/engine/view/renderers/cellselectionrenderer.h
  src/engine/view/renderers/coordinaterenderer.h
  src/engine/view/renderers/floatingtextrenderer.h
  src/engine/view/renderers/genericrenderer.h
  src/engine/view/renderers/gridrenderer.h
  src/engine/view/renderers/instancerenderer.h
  src/engine/view/renderers/lightrenderer.h
  src/engine/view/renderers/offrenderer.h
  src/engine/view/renderers/quadtreerenderer.h
  src/engine/view/renderers/targetrenderer.h
  src/engine/gui/guimanager.h
)

# CEGUI
set(
  FIFE_CEGUI_HDR
  src/engine/gui/cegui/ceguimanager.h
  src/engine/gui/cegui/base/ceguiinputprocessor.h
)

# Fifechan
set(
  FIFE_FCHAN_HDR
  src/engine/gui/fifechan/fifechanmanager.h
  src/engine/gui/fifechan/base/gui_font.h
  src/engine/gui/fifechan/base/gui_image.h
  src/engine/gui/fifechan/base/gui_imageloader.h
  src/engine/gui/fifechan/base/opengl/opengl_gui_graphics.h
  src/engine/gui/fifechan/base/sdl/sdl_gui_graphics.h
  src/engine/gui/fifechan/console/commandline.h
  src/engine/gui/fifechan/console/console.h
  src/engine/gui/fifechan/util/utf8/utf8stringeditor.h
  src/engine/gui/fifechan/widgets/animationicon.h
  src/engine/gui/fifechan/widgets/clicklabel.h
  src/engine/gui/fifechan/widgets/dockarea.h
  src/engine/gui/fifechan/widgets/panel.h
  src/engine/gui/fifechan/widgets/percentagebar.h
  src/engine/gui/fifechan/widgets/resizablewindow.h
)

#------------------------------------------------------------------------------
# Source Groups for Header Files
#------------------------------------------------------------------------------

source_group("fife.core"                FILES ${FIFE_CORE_HDR})
source_group("fife.core.gui.cegui"      FILES ${FIFE_CEGUI_HDR})
source_group("fife.core.gui.fifechan"   FILES ${FIFE_FCHAN_HDR})

add_custom_target(fife.headers SOURCES
    ${FIFE_CORE_HDR}
    ${FIFE_CEGUI_HDR}
    ${FIFE_FCHAN_HDR}
)

#------------------------------------------------------------------------------
# SWIG Interface Files
#------------------------------------------------------------------------------

# The .i files are SWIG interface files.
# They define how the C++ code is exposed to the target language (here Python).
#
# 1. We have the following interface file sets:
#   - FIFE_CORE_SWIG: The main SWIG interface files for the core engine.
#   - FIFE_CEGUI_SWIG: The SWIG interface files for the CEGUI subsystem.
#   - FIFE_FCHAN_SWIG_LIB: The SWIG interface files for the FIFECHAN library (the base components).
#     These files declare the `%module fifechan` and are included in the seperate fifechan.i template.
#   - FIFE_FCHAN_SWIG: The SWIG interface files for the FIFE-specific extensions to FIFECHAN library (the actual GUI components).
#     The files declare the `%module fife` and become part of the main fife.i template.
# 2. During configure_file(), all files are concatenated into CMAKE_SWIG_INCLUSIONS
# 3. This is used in fife.i.templ, which is the main SWIG interface file.
#    It includes CMAKE_SWIG_INCLUSIONS, which means all the .i files are included in the SWIG processing.
#    This is the single fife.i file that SWIG uses to generate the bindings.
# 4. Then swig_add_library() is called to generate the Python bindings.

# Core
set(
  FIFE_CORE_SWIG
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
)

# CEGUI
set(FIFE_CEGUI_SWIG gui/cegui/ceguimanager.i)

# FIFECHAN
set(FIFE_FCHAN_SWIG_LIB gui/fifechan/lib/fifechanlistener.i gui/fifechan/lib/widgets.i)

# FIFECHAN SWIG
set(
  FIFE_FCHAN_SWIG
  gui/fifechan/base/gui_font.i
  gui/fifechan/base/gui_image.i
  gui/fifechan/console/console.i
  gui/fifechan/fifechanmanager.i
  gui/fifechan/widgets/widgets.i
)

#------------------------------------------------------------------------------
# Source Groups for SWIG Files
#------------------------------------------------------------------------------

source_group("fife.core"                    FILES ${FIFE_CORE_SWIG})
source_group("fife.core.gui.cegui"          FILES ${FIFE_CEGUI_SWIG})
source_group("fife.core.gui.fifechan.lib"   FILES ${FIFE_FCHAN_SWIG_LIB})
source_group("fife.core.gui.fifechan"       FILES ${FIFE_FCHAN_SWIG})

# TODO if enabled, the files are not found. ${CMAKE_CURRENT_SOURCE_DIR} needs to be used.
# add_custom_target(fife.swig SOURCES ${FIFE_CORE_SWIG} ${FIFE_CEGUI_SWIG} ${FIFE_FCHAN_SWIG_LIB} ${FIFE_FCHAN_SWIG})

#------------------------------------------------------------------------------
# SWIG WRAPPER Files
#------------------------------------------------------------------------------

# SWIG WRAPPERS FOR PYTHON
set(
  FIFE_SWIGWRAPPERS_PYTHON
  src/swigwrappers/python/extensions.i.templ
  src/swigwrappers/python/fife.i.templ
  src/swigwrappers/python/fife.i.templ.cm
  src/swigwrappers/python/fifechan.i.templ
  src/swigwrappers/python/fifechan.i.templ.cm
)

#------------------------------------------------------------------------------
# Source Groups for SWIG WRAPPER Files
#------------------------------------------------------------------------------

source_group("fife.swigwrappers.python" FILES ${FIFE_SWIGWRAPPERS_PYTHON})

add_custom_target(fife.swig-wrappers SOURCES ${FIFE_SWIGWRAPPERS_PYTHON})

#------------------------------------------------------------------------------
# Find Packages
#------------------------------------------------------------------------------

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(Ogg CONFIG REQUIRED) # Ogg, not OGG
find_package(Vorbis CONFIG REQUIRED) # Vorbis, not VORBIS
find_package(GLEW CONFIG REQUIRED)

if(ENABLE_OPENGL)
  find_package(OpenGL REQUIRED)
  find_package(GLEW CONFIG REQUIRED)
endif()

if(ENABLE_CEGUI)
  find_package(CEGUI REQUIRED)

  if(NOT CEGUI_OpenGLRenderer_FOUND)
    message(FATAL_ERROR "FIFE needs CEGUI with the OpenGLRenderer enabled.")
  endif(NOT CEGUI_OpenGLRenderer_FOUND)

  if(NOT ENABLE_OPENGL)
    message(FATAL_ERROR "OpenGL cannot be disabled, when CEGUI is enabled.")
  endif(NOT ENABLE_OPENGL)

  # TODO remove (when CEGUI provides import target)
  target_include_directories(fife_options INTERFACE ${CEGUI_INCLUDE_DIR})

  message("Using CEGUI v0.8+")
  set(CEGUI TRUE)
  set(FIFE_CORE_SRC ${FIFE_CORE_SRC} ${FIFE_CEGUI_SRC})
endif()

#------------------------------------------------------------------------------
# Build extensions (fifegui, cegui) as a Python extension module using SWIG
#------------------------------------------------------------------------------

if(BUILD_PYTHON_BINDINGS)
  # Setting up SWIG files
  set(SWIG_MODULE_fife_EXTRA_DEPS)
  set(CMAKE_SWIG_IMPORTS "")
  set(CMAKE_SWIG_INCLUSIONS "")
  set(CMAKE_SWIG_IMPORTS_FCHAN "")
  set(CMAKE_SWIG_INCLUSIONS_FCHAN "")

  find_package(Python3 COMPONENTS Development.Module REQUIRED)

  message(STATUS "Python includes:      ${Python3_INCLUDE_DIRS}")
  message(STATUS "Python module target: Python3::Module")

  foreach(filename ${FIFE_CORE_SWIG})
    list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "src/engine/${filename}")
    set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
  endforeach()
endif()

if(ENABLE_FIFEGUI)
  find_package(fifechan COMPONENTS sdl REQUIRED)
  if(ENABLE_OPENGL)
    find_package(fifechan COMPONENTS opengl REQUIRED)
  endif()

  if(BUILD_PYTHON_BINDINGS)
    set(SWIG_MODULE_fifechan_EXTRA_DEPS)

    # Files in FIFE_FCHAN_SWIG_LIB go into CMAKE_SWIG_INCLUSIONS_FCHAN (fifechan module)
    foreach(filename ${FIFE_FCHAN_SWIG_LIB})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "src/engine/${filename}")
      list(APPEND SWIG_MODULE_fifechan_EXTRA_DEPS "src/engine/${filename}")
      set(CMAKE_SWIG_INCLUSIONS_FCHAN "${CMAKE_SWIG_INCLUSIONS_FCHAN}%include ${filename}\n")
    endforeach()

    # Files in FIFE_FCHAN_SWIG go into CMAKE_SWIG_INCLUSIONS (fife module).
    foreach(filename ${FIFE_FCHAN_SWIG})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "src/engine/${filename}")
      list(APPEND SWIG_MODULE_fifechan_EXTRA_DEPS "src/engine/${filename}")
      set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
    endforeach()

    set(CMAKE_SWIG_IMPORTS "${CMAKE_SWIG_IMPORTS}%import(module=\"fife.fifechan\") ${PROJECT_BINARY_DIR}/fifechan.i")

    configure_file(
      "src/swigwrappers/python/fifechan.i.templ.cm"
      "${PROJECT_BINARY_DIR}/fifechan.i"
    )
  endif(BUILD_PYTHON_BINDINGS)
endif(ENABLE_FIFEGUI)

if(ENABLE_CEGUI)
  if(BUILD_PYTHON_BINDINGS)

    # Files in FIFE_CEGUI_SWIG go into CMAKE_SWIG_INCLUSIONS (fife module).
    foreach(filename ${FIFE_CEGUI_SWIG})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "src/engine/${filename}")
      set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
    endforeach()
  endif(BUILD_PYTHON_BINDINGS)
endif(ENABLE_CEGUI)

if(BUILD_PYTHON_BINDINGS)
  configure_file(
    "src/swigwrappers/python/fife.i.templ.cm"
    "${PROJECT_BINARY_DIR}/fife.i"
  )
  find_package(SWIG 4.0.0 REQUIRED)
  include(${SWIG_USE_FILE})
endif()

#------------------------------------------------------------------------------
# Build fifengine as a Python extension module using SWIG
#------------------------------------------------------------------------------
#
# Relevant CMake docs:
# - https://cmake.org/cmake/help/latest/module/FindPython3.html
# - https://cmake.org/cmake/help/latest/module/FindSWIG.html
# - https://cmake.org/cmake/help/latest/module/UseSWIG.html
# Relevant Swig docs:
# - https://www.swig.org/Doc4.4/Python.html

if(BUILD_PYTHON_BINDINGS)
  find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

  message(STATUS "Python executable:    ${Python3_EXECUTABLE}")
  message(STATUS "Python includes:      ${Python3_INCLUDE_DIRS}")
  message(STATUS "Python module target: Python3::Module")

  # Used SWIG flags:
  # -fastdispatch            # Faster overload dispatcher code
  # -dirvtable               # Pseudo virtual table for director dispatch
  # -fastproxy               # Direct C API calls instead of Python proxy wrappers
  # -keyword                 # Enable keyword arguments
  # -w511 -w473 -w404 -w314  # Warning suppression flags
  set(
    CMAKE_SWIG_FLAGS
    -fastdispatch
    -dirvtable
    -fastproxy
    -keyword
    -w511
    -w473
    -w404
    -w314
  )

  set(FIFE_SOURCES ${FIFE_CORE_SRC})

  if(ENABLE_FIFEGUI)
    list(APPEND FIFE_SOURCES ${FIFE_FCHAN_SRC})
  endif()

  set_property(SOURCE "${PROJECT_BINARY_DIR}/fife.i" PROPERTY CPLUSPLUS ON)

  swig_add_library(fife_swig LANGUAGE python SOURCES "${PROJECT_BINARY_DIR}/fife.i" ${FIFE_SOURCES})

  target_include_directories(fife_swig PRIVATE
    ${PROJECT_BINARY_DIR}
    src/engine
    ${PROJECT_SOURCE_DIR}
    ${Python3_INCLUDE_DIRS}
  )

  set_target_properties(fife_swig PROPERTIES
    SWIG_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR};${CMAKE_SOURCE_DIR}/src/engine;${CMAKE_SOURCE_DIR}"
  )

  if(APPLE)
    target_link_options(fife_swig PRIVATE -undefined dynamic_lookup)
  else()
    target_link_libraries(fife_swig PRIVATE Python3::Module)
  endif()

  target_link_libraries(
    fife_swig
    PRIVATE
      fife_options
      SDL2::SDL2
      $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
      $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
      OpenAL::OpenAL
      PNG::PNG
      Vorbis::vorbis
      Vorbis::vorbisfile
      Ogg::ogg
      tinyxml2::tinyxml2
  )

  if(ENABLE_OPENGL)
    target_link_libraries(fife_swig PRIVATE OpenGL::GL GLEW::GLEW)
  endif()

  if(ENABLE_FIFEGUI)
    set_property(SOURCE "${PROJECT_BINARY_DIR}/fifechan.i" PROPERTY CPLUSPLUS ON)

    swig_add_library(fifechan_swig LANGUAGE python SOURCES "${PROJECT_BINARY_DIR}/fifechan.i")

    target_include_directories(fifechan_swig PRIVATE
      ${PROJECT_BINARY_DIR}
      src/engine
      ${PROJECT_SOURCE_DIR}
      ${Python3_INCLUDE_DIRS}
    )

    set_target_properties(fifechan_swig PROPERTIES
      SWIG_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR};${CMAKE_SOURCE_DIR}/src/engine;${CMAKE_SOURCE_DIR}"
    )

    if(APPLE)
      target_link_options(fifechan_swig PRIVATE -undefined dynamic_lookup)
    else()
      target_link_libraries(fifechan_swig PRIVATE Python3::Module)
    endif()

    target_link_libraries(fifechan_swig PRIVATE fifechan::fifechan)
    target_link_libraries(fife_swig PRIVATE fifechan::fifechan)

    if(TARGET fifechan::fifechan_opengl)
      target_link_libraries(fife_swig PRIVATE fifechan::fifechan_opengl)
      target_link_libraries(fifechan_swig PRIVATE fifechan::fifechan_opengl)
    endif()
  endif(ENABLE_FIFEGUI)

  if(ENABLE_CEGUI)
    # TODO use targets
    target_link_libraries(fife_swig PRIVATE cegui::cegui)
    target_link_libraries(fife_swig PRIVATE ${CEGUI_OpenGLRenderer_LIBRARIES})
  endif()

  #---------------------------------------------------------------------------------
  # Install Python package into a prefix-relative site-packages path by default.
  # This keeps `cmake --install --prefix <dir>` working on CI and local builds.
  #---------------------------------------------------------------------------------

  if(NOT DEFINED PYTHON_SITE_PACKAGES)
    execute_process(
      COMMAND ${Python3_EXECUTABLE} -c "import os,sysconfig; p='/__cmake_prefix__'; pure=sysconfig.get_path('purelib', vars={'base': p, 'platbase': p}); print(os.path.relpath(pure, p), end='')"
      OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
    )
    # Slash-Fix: CMake would create a warning when using this variable in FILES or DIRECTORY without changing it.
    file(TO_CMAKE_PATH "${PYTHON_SITE_PACKAGES}" PYTHON_SITE_PACKAGES)
  endif()

  message(STATUS "Python install directory (prefix-relative) is '${PYTHON_SITE_PACKAGES}/fife'.")

  # install pythonized fife (only if targets/files exist)
  if(TARGET _fife)
    install(TARGETS _fife DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  endif()
  if(EXISTS ${PROJECT_BINARY_DIR}/fife.py)
    install(FILES ${PROJECT_BINARY_DIR}/fife.py DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  endif()
  install(FILES src/python/fife/__init__.py DESTINATION ${PYTHON_SITE_PACKAGES}/fife)

  # copy all files/folders from the extension folder
  # but exclude extension folders of the optional dependencies: cegui, pychan
  install(
    DIRECTORY src/python/fife/extensions/
    DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions
    PATTERN "extensions/cegui" EXCLUDE
    PATTERN "extensions/pychan" EXCLUDE
    PATTERN "extensions/*"
  )

  if(ENABLE_CEGUI)
    # install fife's extension for cegui
    install(
      DIRECTORY src/python/fife/extensions/cegui/
      DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions/cegui
    )
  endif()

  if(ENABLE_FIFEGUI)
    # install pychan - fife's extension for fifechan
    install(
      DIRECTORY src/python/fife/extensions/pychan/
      DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions/pychan
    )

    # install pythonized fifechan (only if swig target exists)
    if(TARGET fifechan_swig)
      install(TARGETS fifechan_swig DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
    endif()
    if(EXISTS ${PROJECT_BINARY_DIR}/fifechan.py)
      install(FILES ${PROJECT_BINARY_DIR}/fifechan.py DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
    endif()
  endif(ENABLE_FIFEGUI)
endif(BUILD_PYTHON_BINDINGS)

#------------------------------------------------------------------------------
# Build the fifengine C++ library
#------------------------------------------------------------------------------

if(BUILD_LIBRARY)

  add_library(fife)

  target_include_directories(fife PRIVATE
    ${PROJECT_BINARY_DIR}
    src/engine
    ${PROJECT_SOURCE_DIR}
  )

  # Provide a namespace alias for the library (fife::fife)
  add_library(fife::fife ALIAS fife)

  # Build conditional file lists for target_sources
  set(FIFE_PUBLIC_HEADERS ${FIFE_CORE_HDR})
  if(ENABLE_FIFEGUI)
    list(APPEND FIFE_PUBLIC_HEADERS ${FIFE_FCHAN_HDR})
  endif()
  if(ENABLE_CEGUI)
    list(APPEND FIFE_PUBLIC_HEADERS ${FIFE_CEGUI_HDR})
  endif()

  set(FIFE_PRIVATE_SOURCES ${FIFE_CORE_SRC})
  if(ENABLE_FIFEGUI)
    list(APPEND FIFE_PRIVATE_SOURCES ${FIFE_FCHAN_SRC})
  endif()

  # define public interface for the library
  target_sources(fife
    PUBLIC
    FILE_SET fife_public_headers TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
    FILES
      ${FIFE_PUBLIC_HEADERS}
  )

  # define private sources for the library
  target_sources(fife
    PRIVATE
      ${FIFE_PRIVATE_SOURCES}
  )

  set_target_properties(fife PROPERTIES
    VERSION                ${FIFE_VERSION}
    SOVERSION              ${FIFE_VERSION}
  )

  show_build_target_properties(fife)

  target_link_libraries(fife
    PRIVATE
      fife_options
      ZLIB::ZLIB
      PNG::PNG
      GLEW::GLEW
      Ogg::ogg
      Vorbis::vorbis
      Vorbis::vorbisfile
      Vorbis::vorbisenc
      OpenAL::OpenAL
      tinyxml2::tinyxml2
      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2>
      $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
      $<TARGET_NAME_IF_EXISTS:SDL2_ttf::SDL2_ttf>
  )

  target_link_libraries(fife
    PRIVATE
      $<$<BOOL:${ENABLE_FIFEGUI}>:fifechan::fifechan>

      $<$<BOOL:${ENABLE_OPENGL}>:OpenGL::GL>
      $<$<BOOL:${ENABLE_OPENGL}>:GLEW::GLEW>

      $<$<BOOL:${ENABLE_CEGUI}>:${CEGUI_LIBRARIES}>
      $<$<BOOL:${ENABLE_CEGUI}>:${CEGUI_OpenGLRenderer_LIBRARIES}>
  )

  # Fifechan + OpenGL
  # keep if/elseif for clarity, generator expressions make it hard to read otherwise
  if(ENABLE_FIFEGUI AND ENABLE_OPENGL)
    if(TARGET fifechan::fifechan_opengl)
      target_link_libraries(fife PRIVATE fifechan::fifechan_opengl)
    endif()
  endif()

  if(BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests/core_tests)
  endif()

  install(TARGETS fife
   EXPORT fife-targets

   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}

   FILE_SET fife_public_headers
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}

   PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE

   # Experimental!
   # EXPORT_PACKAGE_DEPENDENCIES
  )

  # TODO check if this install folder is correct
  install(FILES src/engine/version.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fife)

endif(BUILD_LIBRARY)

#------------------------------------------------------------------------------
# Compatibility build target alias
#------------------------------------------------------------------------------

# Some builds request the "fife" target even when only "fife_swig" exists.
# Provide a fallback target in that case.
if(NOT TARGET fife AND TARGET fife_swig)
  add_custom_target(fife DEPENDS fife_swig)
endif()

#------------------------------------------------------------------------------
# Install Runtime DLLs (Windows)
# Copies all DLLs and transitive dependencies to CMAKE_INSTALL_LIBDIR
#------------------------------------------------------------------------------
if(WIN32)
  install_fifechan_dlls()
  install_vcpkg_dlls()
  install_runtime_dlls()
endif()

#-------------------------------------------------------------------
# Display Feature Summary
#-------------------------------------------------------------------

feature_summary(
  WHAT ALL
  DESCRIPTION "[Build Summary]\n"
  FATAL_ON_MISSING_REQUIRED_PACKAGES
)
