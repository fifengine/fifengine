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
#                                 Project
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
  "${PROJECT_SOURCE_DIR}/src/engine/version.hpp.in"
  "${PROJECT_SOURCE_DIR}/src/engine/version.hpp"
  @ONLY
)

#------------------------------------------------------------------------------
#                                 Options
#------------------------------------------------------------------------------

#option(BUILD_SHARED_LIBS     "Build using shared libraries"                OFF)

# TODO: ENABLE_OPENGL, ENABLE_FIFECHAN, ENABLE_CEGUI, ENABLE_LOGGING
# TODO: FIFE_PYTHON, FIFE_LIBRARY, FIFE_TESTS

option(opengl           "Enable OpenGL support"                              ON)
option(fifechan         "Enable Fifechan GUI subsystem"                      ON)
option(cegui            "Enable Crazy Eddie's GUI subsystem"                OFF)
option(logging          "Enable logging"                                     ON)
option(build-python     "Build python extension module"                      ON)
option(build-library    "Build C++ library"                                 OFF)
option(build-tests      "Build C++ tests"                                   OFF)

# Reminder: The variable USE_VCPKG exists, which controls SetupVcpkg.
# It is not a build option, because it must be set before project().

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
#                                 Configure
#------------------------------------------------------------------------------

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
endif(NOT CMAKE_BUILD_TYPE)

# We build a shared library by default. For a static library,
# set "-Dbuild-library=ON -DBUILD_SHARED_LIBS=OFF".
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

  # ensure we use minimal "windows.h" lib without the crazy min max macros
  add_definitions(-DWIN32_LEAN_AND_MEAN)
  add_definitions(-DNOMINMAX)

  # avoid "C1128: number of sections exceeded object file format limit: compile with /bigobj"
  add_compile_options(/bigobj)

endif()

# GCC and Clang compiler options
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wno-error")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g2 -Wall -Wextra -Wunused")
  endif()
endif()

#------------------------------------------------------------------------------
#                                 Defines
#------------------------------------------------------------------------------

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
message(STATUS "[INFO] Enable OpenGL support               -> '${opengl}'")
message(STATUS "[INFO] Enable Fifechan GUI subsystem       -> '${fifechan}'")
message(STATUS "[INFO] Enable Crazy Eddie's GUI subsystem  -> '${cegui}'")
message(STATUS "[INFO] Enable logging                      -> '${logging}'")
message(STATUS "[INFO] Build the python extension module   -> '${build-python}'")
message(STATUS "[INFO] Build C++ library                   -> '${build-library}'")
message(STATUS "[INFO] Build C++ tests                     -> '${build-tests}'")
message(STATUS "")
message(STATUS "[INFO] CMake version            -> '${CMAKE_VERSION}'.")
message(STATUS "[INFO] CXX Compiler ID          -> '${CMAKE_CXX_COMPILER_ID}'.")
message(STATUS "[INFO] CXX Compiler Flags       -> '${CMAKE_CXX_FLAGS}'.")
message(STATUS "---------------------------------------------------------------------")

#------------------------------------------------------------------------------
#                                 Source Files
#------------------------------------------------------------------------------

# tell compiler to search in the following folders for include files
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src/engine")
include_directories("${PROJECT_SOURCE_DIR}")

# Core
set(
  FIFE_CORE_SRC
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
)

# CEGUI v0.8.0+
set(
  FIFE_CEGUI_SRC
  ${PROJECT_SOURCE_DIR}/src/engine/gui/cegui/ceguimanager.cpp
  ${PROJECT_SOURCE_DIR}/src/engine/gui/cegui/base/ceguiinputprocessor.cpp
)

# FIFECHAN
set(
  FIFE_FCHAN_SRC
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

#------------------------------------------------------------------------------
#                         Source Groups for Source Files
#------------------------------------------------------------------------------

source_group("fife.core" FILES ${FIFE_CORE_SRC})
source_group("fife.core.gui.cegui" FILES ${FIFE_CEGUI_SRC})
source_group("fife.core.gui.fifechan" FILES ${FIFE_FCHAN_SRC})

#------------------------------------------------------------------------------
#                                 Header Files
#------------------------------------------------------------------------------

# Core
set(
  FIFE_CORE_HDR
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
)

# CEGUI
set(
  FIFE_CEGUI_HDR
  ${PROJECT_SOURCE_DIR}/src/engine/gui/cegui/ceguimanager.h
  ${PROJECT_SOURCE_DIR}/src/engine/gui/cegui/base/ceguiinputprocessor.h
)

# Fifechan
set(
  FIFE_FCHAN_HDR
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

#------------------------------------------------------------------------------
#                      Source Groups for Header Files
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
#                                 SWIG Files
#------------------------------------------------------------------------------

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
#                      Source Groups for SWIG Files
#------------------------------------------------------------------------------

source_group("fife.core"                    FILES ${FIFE_CORE_SWIG})
source_group("fife.core.gui.cegui"          FILES ${FIFE_CEGUI_SWIG})
source_group("fife.core.gui.fifechan.lib"   FILES ${FIFE_FCHAN_SWIG_LIB})
source_group("fife.core.gui.fifechan"       FILES ${FIFE_FCHAN_SWIG})

# TODO some files for this source_group are not found
#add_custom_target(fife.swig SOURCES ${FIFE_CORE_SWIG} ${FIFE_CEGUI_SWIG} ${FIFE_FCHAN_SWIG_LIB} ${FIFE_FCHAN_SWIG} ${FIFE_LROCKET_SWIG})

#------------------------------------------------------------------------------
#                                 SWIG WRAPPER Files
#------------------------------------------------------------------------------

# SWIG WRAPPERS FOR PYTHON
set(
  FIFE_SWIGWRAPPERS_PYTHON
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/extensions.i.templ
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fife.i.templ
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fife.i.templ.cm
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fifechan.i.templ
  ${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fifechan.i.templ.cm
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
find_package(tinyxml2 CONFIG REQUIRED)
find_package(Ogg CONFIG REQUIRED) # Ogg, not OGG
find_package(Vorbis CONFIG REQUIRED) # Vorbis, not VORBIS
find_package(GLEW CONFIG REQUIRED)

if(opengl)
  find_package(OpenGL REQUIRED)
  find_package(GLEW CONFIG REQUIRED)
endif(opengl)

if(cegui)
  find_package(CEGUI REQUIRED)

  if(NOT CEGUI_OpenGLRenderer_FOUND)
    message(FATAL_ERROR "FIFE needs CEGUI with the OpenGLRenderer enabled.")
  endif(NOT CEGUI_OpenGLRenderer_FOUND)

  if(NOT opengl)
    message(FATAL_ERROR "OpenGL cannot be disabled, when CEGUI is enabled.")
  endif(NOT opengl)

  # TODO remove (when CEGUI provides import target)
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

  if(NOT DEFINED PYTHON_INCLUDE_PATH)
    set(PYTHON_INCLUDE_PATH ${Python3_INCLUDE_DIRS})
  endif()
  if(NOT DEFINED PYTHON_LIBRARIES)
    set(PYTHON_LIBRARIES ${Python3_LIBRARIES})
  endif()

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

    set(CMAKE_SWIG_IMPORTS "${CMAKE_SWIG_IMPORTS}%import(module=\"fife.fifechan\") ${PROJECT_BINARY_DIR}/fifechan.i")

    configure_file(
      "${PROJECT_SOURCE_DIR}/src/swigwrappers/python/fifechan.i.templ.cm"
      "${PROJECT_BINARY_DIR}/fifechan.i"
    )
  endif(build-python)
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

#------------------------------------------------------------------------------
#                  Build fifengine as a Python extension module
#------------------------------------------------------------------------------

if(build-python)
  # Find python library and add include path for python headers
  find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
  message(STATUS "Python3 includes: ${Python3_INCLUDE_DIRS}")
  message(STATUS "Python3 libraries: ${Python3_LIBRARIES}")

  include_directories(${PYTHON_INCLUDE_PATH})

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

  set_source_files_properties("${PROJECT_BINARY_DIR}/fife.i" PROPERTIES CPLUSPLUS ON)
  set(FIFE_SOURCES ${FIFE_CORE_SRC})

  if(fifechan)
    list(APPEND FIFE_SOURCES ${FIFE_FCHAN_SRC})
  endif(fifechan)

  swig_add_library(fife_swig LANGUAGE python SOURCES "${PROJECT_BINARY_DIR}/fife.i" ${FIFE_SOURCES})

  if(APPLE)
    target_link_options(fife_swig PRIVATE -undefined dynamic_lookup)
  else(APPLE)
    target_link_libraries(fife_swig PRIVATE ${PYTHON_LIBRARIES})
  endif(APPLE)

  target_link_libraries(
    fife_swig
    PRIVATE
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

  if(opengl)
    target_link_libraries(fife_swig PRIVATE ${OPENGL_gl_LIBRARY})
    target_link_libraries(fife_swig PRIVATE GLEW::GLEW)
  endif(opengl)

  if(fifechan)
    set_source_files_properties("${PROJECT_BINARY_DIR}/fifechan.i" PROPERTIES CPLUSPLUS ON)

    swig_add_library(fifechan_swig LANGUAGE python SOURCES "${PROJECT_BINARY_DIR}/fifechan.i")

    if(APPLE)
      target_link_options(fifechan_swig PRIVATE -undefined dynamic_lookup)
    else(APPLE)
      target_link_libraries(fifechan_swig PRIVATE ${PYTHON_LIBRARIES})
    endif(APPLE)
    target_link_libraries(fifechan_swig PRIVATE ${FIFECHAN_LIBRARIES})
    target_link_libraries(fife_swig PRIVATE ${FIFECHAN_LIBRARIES})

    if(TARGET Fifechan::opengl)
      target_link_libraries(fife_swig PRIVATE Fifechan::opengl)
      target_link_libraries(fifechan_swig PRIVATE Fifechan::opengl)
    elseif(DEFINED FIFECHAN_opengl_LIBRARY AND EXISTS "${FIFECHAN_opengl_LIBRARY}")
      target_link_libraries(fife_swig PRIVATE "${FIFECHAN_opengl_LIBRARY}")
      target_link_libraries(fifechan_swig PRIVATE "${FIFECHAN_opengl_LIBRARY}")
    endif()
  endif(fifechan)

  if(cegui)
    target_link_libraries(fife_swig PRIVATE ${CEGUI_LIBRARIES})
    target_link_libraries(fife_swig PRIVATE ${CEGUI_OpenGLRenderer_LIBRARIES})
  endif(cegui)

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

  message(STATUS "    Python install directory (prefix-relative) is '${PYTHON_SITE_PACKAGES}/fife'.")

  # install pythonized fife (only if targets/files exist)
  if(TARGET _fife)
    install(TARGETS _fife DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  endif()
  if(EXISTS ${PROJECT_BINARY_DIR}/fife.py)
    install(FILES ${PROJECT_BINARY_DIR}/fife.py DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
  endif()
  install(FILES ${PROJECT_SOURCE_DIR}/src/python/fife/__init__.py DESTINATION ${PYTHON_SITE_PACKAGES}/fife)

  # copy all files/folders from the extension folder
  # but exclude extension folders of the optional dependencies: cegui, pychan
  install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/src/python/fife/extensions/
    DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions
    PATTERN "extensions/cegui" EXCLUDE
    PATTERN "extensions/pychan" EXCLUDE
    PATTERN "extensions/*"
  )

  if(cegui)
    # install fife's extension for cegui
    install(
      DIRECTORY ${PROJECT_SOURCE_DIR}/src/python/fife/extensions/cegui/
      DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions/cegui
    )
  endif(cegui)

  if(fifechan)
    # install pychan - fife's extension for fifechan
    install(
      DIRECTORY ${PROJECT_SOURCE_DIR}/src/python/fife/extensions/pychan/
      DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions/pychan
    )

    # install pythonized fifechan (only if swig target exists)
    if(TARGET fifechan_swig)
      install(TARGETS fifechan_swig DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
    endif()
    if(EXISTS ${PROJECT_BINARY_DIR}/fifechan.py)
      install(FILES ${PROJECT_BINARY_DIR}/fifechan.py DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
    endif()
  endif(fifechan)
endif(build-python)

#------------------------------------------------------------------------------
#                       Build fifengine as a library
#------------------------------------------------------------------------------

if(build-library)
  set(FIFE_SOURCES ${FIFE_CORE_SRC})

  if(fifechan)
    list(APPEND FIFE_SOURCES ${FIFE_FCHAN_SRC})
  endif(fifechan)

  add_library(fife ${FIFE_SOURCES})

  set_target_properties(fife PROPERTIES
    VERSION                ${FIFE_VERSION}
    SOVERSION              ${FIFE_VERSION}
  )

  show_build_target_properties(fife)

  target_link_libraries(
    fife
      PRIVATE
        ZLIB::ZLIB
        ${PNG_LIBRARIES}
        GLEW::GLEW
        Ogg::ogg
        Vorbis::vorbis
        Vorbis::vorbisfile
        Vorbis::vorbisenc
        OpenAL::OpenAL
        tinyxml2::tinyxml2
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<TARGET_NAME_IF_EXISTS:SDL2::SDL2>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<TARGET_NAME_IF_EXISTS:SDL2_ttf::SDL2_ttf>
  )

  if(opengl)
    target_link_libraries(fife PRIVATE ${OPENGL_gl_LIBRARY} GLEW::GLEW)
  endif(opengl)
  if(fifechan)
    target_link_libraries(fife PRIVATE ${FIFECHAN_LIBRARIES})
    if(opengl)
      if(TARGET Fifechan::opengl)
        target_link_libraries(fife PRIVATE Fifechan::opengl)
      elseif(DEFINED FIFECHAN_opengl_LIBRARY AND EXISTS "${FIFECHAN_opengl_LIBRARY}")
        target_link_libraries(fife PRIVATE "${FIFECHAN_opengl_LIBRARY}")
      endif()
    endif()
  endif(fifechan)
  if(cegui)
    target_link_libraries(fife PRIVATE ${CEGUI_LIBRARIES} ${CEGUI_OpenGLRenderer_LIBRARIES})
  endif(cegui)

  if(build-tests)
    include(CTest)
    add_subdirectory(${PROJECT_SOURCE_DIR}/tests/core_tests)
  endif(build-tests)

  install(TARGETS fife DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(FILES ${PROJECT_SOURCE_DIR}/src/engine/version.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fife/core/)

  macro(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)
    foreach(HEADER ${${HEADER_LIST}})
      get_filename_component(DIR ${HEADER} DIRECTORY)
      # Compute a relative path under the source `src/engine` tree so headers
      # install to e.g. ${includedir}/fife/gui/... instead of embedding the
      # full repository path. Fall back to the legacy `engine/` layout when
      # applicable.
      file(RELATIVE_PATH REL_PATH "${PROJECT_SOURCE_DIR}/src/engine" "${DIR}")
      if("${REL_PATH}" MATCHES "^\.\./")
        file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_LIST_DIR}/engine" "${DIR}")
      endif()
      install(FILES ${HEADER} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fife/${REL_PATH})
    endforeach(HEADER)
  endmacro(INSTALL_HEADERS_WITH_DIRECTORY)

  install_headers_with_directory(FIFE_CORE_HDR)
  if(fifechan)
    install_headers_with_directory(FIFE_FCHAN_HDR)
  endif(fifechan)
  if(cegui)
    install_headers_with_directory(FIFE_CEGUI_HDR)
  endif(cegui)
endif(build-library)

#------------------------------------------------------------------------------
#                      Compatibility build target alias
#------------------------------------------------------------------------------

# Some presets/build tasks request "--target fife" even when build-library is
# disabled and only the SWIG module target exists. Provide a compatibility
# target in that case.
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
