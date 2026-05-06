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
# Apply versioning
#-------------------------------------------------------------------

# Populate version.hpp with version data.
# This must be done after project() is called,
# otherwise PROJECT_* variables are not available.
# See version.hpp.in for the replaced tokens.
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/src/fife/version.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/src/fife/version.hpp"
  @ONLY
)

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
include(CMakePackageConfigHelpers)

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------

option(BUILD_SHARED_LIBS      "Build using shared libraries"                 ON)

option(ENABLE_OPENGL          "Enable OpenGL support"                        ON)
option(ENABLE_FIFEGUI         "Enable FifeGUI subsystem"                     ON)
option(ENABLE_LOGGING         "Enable Logging"                               ON)

option(BUILD_PYTHON_BINDINGS  "Build Python bindings"                        ON)
option(BUILD_LIBRARY          "Build C++ library"                            ON)
option(BUILD_TESTS            "Build C++ tests"                              ON)

# Reminder: The variable USE_VCPKG exists, which controls SetupVcpkg.
# It is not a build option, because it must be set before project().

#------------------------------------------------------------------------------
# Configure
#------------------------------------------------------------------------------

# set the default build type to 'Debug' if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting default CMAKE_BUILD_TYPE to 'Debug'")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Do not allow an in-source-tree build, request an out-of-source-tree build.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "#Please build outside of the source tree!\n
                       Make an out-of-source-tree folder for building and run CMake there:\n
                       mkdir ../out/build; cd ../out/build; cmake ../../fifengine\n
                       Before that, cleanup:\nrm -rf CMakeCache.txt CMakeFiles")
endif()

# Disable in-source builds and modifications to the source tree.
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Create compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable color diagnostics (-fcolor-diagnostics).
SET(CMAKE_COLOR_DIAGNOSTICS ON)

# organize targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#------------------------------------------------------------------------------
# Global compile definitions as INTERFACE library
#------------------------------------------------------------------------------

add_library(fife_options INTERFACE)

if(WIN32)
  target_compile_definitions(fife_options INTERFACE
    _UNICODE
    UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    WINVER=0x0A00
    _WIN32_WINNT=0x0A00
  )
endif()

if(ENABLE_LOGGING)
  target_compile_definitions(fife_options INTERFACE LOG_ENABLED)
endif()

if(ENABLE_OPENGL)
  target_compile_definitions(fife_options INTERFACE HAVE_OPENGL)
endif()

#------------------------------------------------------------------------------
# Compiler Options
#------------------------------------------------------------------------------

# TODO enforced globally. change this to target_compile_options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# MSVC compiler options
# TODO this should probably be moved into a seperate file and be a function to set it on a target.
# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170
if(MSVC)
  add_compile_options(/nologo) # Suppress Startup Banner
  add_compile_options(/MP)     # Build on all cores
  add_compile_options(/Gm-)    # Disable minimal rebuild
  add_compile_options(/utf-8)  # Use UTF-8 encoding for source files and compiler output

  add_compile_options(/permissive-)     # enforce C++ standard conformance
  add_compile_options(/Zc:__cplusplus)  # report correct C++ standard in __cplusplus macro
  add_compile_options(/Zc:preprocessor) # use the C++20 compliant preprocessor
  add_compile_options(/EHsc)            # Enable structured C++ exceptions

  # avoid "C1128: number of sections exceeded object file format limit
  add_compile_options(/bigobj)

  add_compile_options(/diagnostics:caret) # Show the source line for warnings and errors
  add_compile_options(/diagnostics:color)

  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(/W0) # Disable warnings

    add_compile_options(/GF) # Enables string pooling
    add_compile_options(/GS) # Buffer security checks

    # Enable optimizations /O2?
    add_compile_options(/Oi) # generate intrinsic functions

  elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

    add_compile_options(/W4) # warning level: /W<level 0 to 4> or /Wall
    add_compile_options(/O2)

  elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")

    # Just My Code debugging (disable stepping into system headers and external code)
    add_compile_options(/JMC)

    add_compile_options(/W4) # warning level: /W<level 0 to 4> or /Wall
    add_compile_options(/Od) # disable optimizations

  endif()

  # ignore warnings from external headers
  # https://devblogs.microsoft.com/cppblog/broken-warnings-theory/
  # /external:W0             turn off compiler warnings on external headers
  # /external:anglebrackets  let any file included with angle brackets be external
  add_compile_options(
    "/experimental:external"
    "/external:W0"
    "/external:anglebrackets"
    "/external:IC:\\Program Files\\Microsoft Visual Studio"
    "/external:IC:\\Program Files (x86)\\Windows Kits"
  )
  # Disable MSVC warnings
  add_compile_options(/wd4514) # unreferenced inline function has been removed
  add_compile_options(/wd4710) # function not inlined
  add_compile_options(/wd4820) # Disable warning about padding added to structures for alignment
  add_compile_options(/wd5045) # Compiler will insert Spectre mitigation
  add_compile_options(/wd4191) # Disable warning about unsafe type casting. 6k+ warnings. Could be UB though ;)
  add_compile_options(/wd4251) # needs to have dll-interface to be used by clients of class
  add_compile_options(/wd4275) # non dll-interface class used as base for dll-interface class

endif()

# GCC and Clang compiler options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8")
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wno-error")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g2 -Wall -Wextra -Wunused")
  endif()
endif()

#------------------------------------------------------------------------------
# Find Dependencies (non-vcpkg)
#------------------------------------------------------------------------------

# Add the install location of the non-vcpkg dependencies to CMAKE_PREFIX_PATH
# to allow find_package to find them. This is needed for Fifechan and SWIG
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/out/fife-dependencies/${VCPKG_TARGET_TRIPLET}/install")

if(WIN32)
  list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/out/fife-dependencies/${VCPKG_TARGET_TRIPLET}/build-tools/swig")
else()
  list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/out/fife-dependencies/${VCPKG_TARGET_TRIPLET}/build-tools/swig/bin")
endif()

#------------------------------------------------------------------------------
# Display Status
#------------------------------------------------------------------------------

message(STATUS "---------------------------------------------------------------------")
message(STATUS "[ 🛠️ ] Building FIFE v${FIFE_VERSION} using build type '${CMAKE_BUILD_TYPE}'.")
message(STATUS "---------------------------------------------------------------------")
message(STATUS "")
message(STATUS "[INFO] Source dir                  -> '${PROJECT_SOURCE_DIR}'.")
message(STATUS "[INFO] Build dir                   -> '${PROJECT_BINARY_DIR}'.")
message(STATUS "[INFO] Installation path           -> '${CMAKE_INSTALL_PREFIX}'")
message(STATUS "                                      To change the installation path set -DCMAKE_INSTALL_PREFIX=<path>.")
message(STATUS "")
message(STATUS "[INFO] Dependencies folder         -> '${PROJECT_SOURCE_DIR}/out/fife-dependencies/install'.")
message(STATUS "[INFO] CMAKE_PREFIX_PATH           -> '${CMAKE_PREFIX_PATH}'.")
message(STATUS "")
message(STATUS "[INFO] Build shared libraries      -> '${BUILD_SHARED_LIBS}'.")
message(STATUS "")
message(STATUS "[INFO] Enable OpenGL support       -> '${ENABLE_OPENGL}'")
message(STATUS "[INFO] Enable FifeGUI subsystem    -> '${ENABLE_FIFEGUI}'")
message(STATUS "[INFO] Enable logging              -> '${ENABLE_LOGGING}'")
message(STATUS "[INFO] Build the Python bindings   -> '${BUILD_PYTHON_BINDINGS}'")
message(STATUS "[INFO] Build C++ library           -> '${BUILD_LIBRARY}'")
message(STATUS "[INFO] Build C++ tests             -> '${BUILD_TESTS}'")
message(STATUS "")
message(STATUS "[INFO] CMake version               -> '${CMAKE_VERSION}'.")
message(STATUS "[INFO] CXX Compiler ID             -> '${CMAKE_CXX_COMPILER_ID}'.")
message(STATUS "[INFO] CXX Compiler Flags          -> '${CMAKE_CXX_FLAGS}'.")
message(STATUS "---------------------------------------------------------------------")

#------------------------------------------------------------------------------
# Source Files
#------------------------------------------------------------------------------

set(
  FIFE_SOURCES
  src/fife/audio/actionaudio.cpp
  src/fife/audio/soundclip.cpp
  src/fife/audio/soundclipmanager.cpp
  src/fife/audio/soundemitter.cpp
  src/fife/audio/soundmanager.cpp
  src/fife/audio/soundsource.cpp
  src/fife/audio/effects/soundeffect.cpp
  src/fife/audio/effects/soundeffectmanager.cpp
  src/fife/audio/effects/soundfilter.cpp
  src/fife/controller/engine.cpp
  src/fife/controller/enginesettings.cpp
  src/fife/eventchannel/eventmanager.cpp
  src/fife/eventchannel/joystick/joystick.cpp
  src/fife/eventchannel/joystick/joystickmanager.cpp
  src/fife/loaders/native/audio/ogg_loader.cpp
  src/fife/loaders/native/audio/sounddecoder_ogg.cpp
  src/fife/loaders/native/input/controllermappingloader.cpp
  src/fife/loaders/native/map/animationloader.cpp
  src/fife/loaders/native/map/atlasloader.cpp
  src/fife/loaders/native/map/maploader.cpp
  src/fife/loaders/native/map/objectloader.cpp
  src/fife/loaders/native/map/percentdonelistener.cpp
  src/fife/loaders/native/video/imageloader.cpp
  src/fife/loaders/native/video/resourceanimationloader.cpp
  src/fife/model/model.cpp
  src/fife/model/metamodel/action.cpp
  src/fife/model/metamodel/object.cpp
  src/fife/model/metamodel/timeprovider.cpp
  src/fife/model/metamodel/grids/cellgrid.cpp
  src/fife/model/metamodel/grids/hexgrid.cpp
  src/fife/model/metamodel/grids/squaregrid.cpp
  src/fife/model/structures/cell.cpp
  src/fife/model/structures/cellcache.cpp
  src/fife/model/structures/instance.cpp
  src/fife/model/structures/instancetree.cpp
  src/fife/model/structures/layer.cpp
  src/fife/model/structures/location.cpp
  src/fife/model/structures/map.cpp
  src/fife/model/structures/renderernode.cpp
  src/fife/model/structures/trigger.cpp
  src/fife/model/structures/triggercontroller.cpp
  src/fife/pathfinder/route.cpp
  src/fife/pathfinder/routepather/multilayersearch.cpp
  src/fife/pathfinder/routepather/routepather.cpp
  src/fife/pathfinder/routepather/routepathersearch.cpp
  src/fife/pathfinder/routepather/singlelayersearch.cpp
  src/fife/savers/native/input/controllermappingsaver.cpp
  src/fife/savers/native/map/mapsaver.cpp
  src/fife/util/base/exception.cpp
  src/fife/util/base/fifeclass.cpp
  src/fife/util/base/stringutils.cpp
  src/fife/util/log/logger.cpp
  src/fife/util/math/angles.cpp
  src/fife/util/resource/resource.cpp
  src/fife/util/time/timeevent.cpp
  src/fife/util/time/timemanager.cpp
  src/fife/util/time/timer.cpp
  src/fife/vfs/directoryprovider.cpp
  src/fife/vfs/filesystem.cpp
  src/fife/vfs/vfs.cpp
  src/fife/vfs/vfsdirectory.cpp
  src/fife/vfs/vfssource.cpp
  src/fife/vfs/vfssourceprovider.cpp
  src/fife/vfs/dat/dat1.cpp
  src/fife/vfs/dat/dat2.cpp
  src/fife/vfs/dat/lzssdecoder.cpp
  src/fife/vfs/dat/rawdatadat1.cpp
  src/fife/vfs/dat/rawdatadat2.cpp
  src/fife/vfs/raw/rawdata.cpp
  src/fife/vfs/raw/rawdatafile.cpp
  src/fife/vfs/raw/rawdatamemsource.cpp
  src/fife/vfs/raw/rawdatasource.cpp
  src/fife/vfs/zip/zipfilesource.cpp
  src/fife/vfs/zip/zipnode.cpp
  src/fife/vfs/zip/zipprovider.cpp
  src/fife/vfs/zip/zipsource.cpp
  src/fife/vfs/zip/ziptree.cpp
  src/fife/video/animation.cpp
  src/fife/video/animationmanager.cpp
  src/fife/video/atlasbook.cpp
  src/fife/video/color.cpp
  src/fife/video/cursor.cpp
  src/fife/video/devicecaps.cpp
  src/fife/video/image.cpp
  src/fife/video/imagemanager.cpp
  src/fife/video/renderbackend.cpp
  src/fife/video/fonts/fontbase.cpp
  src/fife/video/fonts/imagefontbase.cpp
  src/fife/video/fonts/subimagefont.cpp
  src/fife/video/fonts/textrenderpool.cpp
  src/fife/video/fonts/truetypefont.cpp
  src/fife/video/opengl/glimage.cpp
  src/fife/video/opengl/renderbackendopengl.cpp
  src/fife/video/sdl/renderbackendsdl.cpp
  src/fife/video/sdl/sdlblendingfunctions.cpp
  src/fife/video/sdl/sdlimage.cpp
  src/fife/view/camera.cpp
  src/fife/view/layercache.cpp
  src/fife/view/rendererbase.cpp
  src/fife/view/renderitem.cpp
  src/fife/view/visual.cpp
  src/fife/view/renderers/blockinginforenderer.cpp
  src/fife/view/renderers/cellrenderer.cpp
  src/fife/view/renderers/cellselectionrenderer.cpp
  src/fife/view/renderers/coordinaterenderer.cpp
  src/fife/view/renderers/floatingtextrenderer.cpp
  src/fife/view/renderers/genericrenderer.cpp
  src/fife/view/renderers/gridrenderer.cpp
  src/fife/view/renderers/instancerenderer.cpp
  src/fife/view/renderers/lightrenderer.cpp
  src/fife/view/renderers/offrenderer.cpp
  src/fife/view/renderers/quadtreerenderer.cpp
  src/fife/view/renderers/targetrenderer.cpp
)

set(
  FIFE_FIFEGUI_SOURCES
  src/fife/gui/fifechan/fifechanmanager.cpp
  src/fife/gui/fifechan/base/gui_font.cpp
  src/fife/gui/fifechan/base/gui_image.cpp
  src/fife/gui/fifechan/base/gui_imageloader.cpp
  src/fife/gui/fifechan/base/opengl/opengl_gui_graphics.cpp
  src/fife/gui/fifechan/base/sdl/sdl_gui_graphics.cpp
  src/fife/gui/fifechan/console/commandline.cpp
  src/fife/gui/fifechan/console/console.cpp
  src/fife/gui/fifechan/util/utf8/utf8stringeditor.cpp
  src/fife/gui/fifechan/widgets/animationicon.cpp
  src/fife/gui/fifechan/widgets/clicklabel.cpp
  src/fife/gui/fifechan/widgets/dockarea.cpp
  src/fife/gui/fifechan/widgets/panel.cpp
  src/fife/gui/fifechan/widgets/percentagebar.cpp
  src/fife/gui/fifechan/widgets/resizablewindow.cpp
)

#------------------------------------------------------------------------------
# Source Groups for Source Files
#------------------------------------------------------------------------------

source_group("fife"                FILES ${FIFE_SOURCES})
source_group("fife.gui.fifegui"    FILES ${FIFE_FIFEGUI_SOURCES})

#------------------------------------------------------------------------------
# Header Files
#------------------------------------------------------------------------------

set(
  FIFE_VERSION_HEADER
  "${CMAKE_CURRENT_BINARY_DIR}/src/fife/version.hpp"
)

set(
  FIFE_CORE_HEADERS
  src/fife/platform.h
  src/fife/modules.h
  src/fife/audio/actionaudio.h
  src/fife/audio/fife_openal.h
  src/fife/audio/soundclip.h
  src/fife/audio/soundclipmanager.h
  src/fife/audio/soundconfig.h
  src/fife/audio/sounddecoder.h
  src/fife/audio/soundemitter.h
  src/fife/audio/soundmanager.h
  src/fife/audio/soundsource.h
  src/fife/audio/effects/soundeffect.h
  src/fife/audio/effects/soundeffectmanager.h
  src/fife/audio/effects/soundfilter.h
  src/fife/controller/engine.h
  src/fife/controller/enginesettings.h
  src/fife/eventchannel/eventmanager.h
  src/fife/eventchannel/base/event.h
  src/fife/eventchannel/base/ilistener.h
  src/fife/eventchannel/base/inputevent.h
  src/fife/eventchannel/command/command.h
  src/fife/eventchannel/command/commandids.h
  src/fife/eventchannel/command/icommandcontroller.h
  src/fife/eventchannel/command/icommandlistener.h
  src/fife/eventchannel/drop/dropevent.h
  src/fife/eventchannel/drop/idropcontroller.h
  src/fife/eventchannel/drop/idroplistener.h
  src/fife/eventchannel/joystick/ijoystickcontroller.h
  src/fife/eventchannel/joystick/ijoysticklistener.h
  src/fife/eventchannel/joystick/joystick.h
  src/fife/eventchannel/joystick/joystickevent.h
  src/fife/eventchannel/joystick/joystickmanager.h
  src/fife/eventchannel/key/ikeycontroller.h
  src/fife/eventchannel/key/ikeyfilter.h
  src/fife/eventchannel/key/ikeylistener.h
  src/fife/eventchannel/key/key.h
  src/fife/eventchannel/key/keyevent.h
  src/fife/eventchannel/mouse/imousecontroller.h
  src/fife/eventchannel/mouse/imouselistener.h
  src/fife/eventchannel/mouse/mouseevent.h
  src/fife/eventchannel/sdl/isdleventcontroller.h
  src/fife/eventchannel/sdl/isdleventlistener.h
  src/fife/eventchannel/source/eventsourcetypes.h
  src/fife/eventchannel/source/ieventsource.h
  src/fife/eventchannel/text/itextcontroller.h
  src/fife/eventchannel/text/itextlistener.h
  src/fife/eventchannel/text/text.h
  src/fife/eventchannel/text/textevent.h
  src/fife/loaders/native/audio/ogg_loader.h
  src/fife/loaders/native/audio/sounddecoder_ogg.h
  src/fife/loaders/native/input/controllermappingloader.h
  src/fife/loaders/native/map/animationloader.h
  src/fife/loaders/native/map/atlasloader.h
  src/fife/loaders/native/map/ianimationloader.h
  src/fife/loaders/native/map/iatlasloader.h
  src/fife/loaders/native/map/imaploader.h
  src/fife/loaders/native/map/iobjectloader.h
  src/fife/loaders/native/map/maploader.h
  src/fife/loaders/native/map/objectloader.h
  src/fife/loaders/native/map/percentdonelistener.h
  src/fife/loaders/native/video/imageloader.h
  src/fife/loaders/native/video/resourceanimationloader.h
  src/fife/model/model.h
  src/fife/model/metamodel/action.h
  src/fife/model/metamodel/ipather.h
  src/fife/model/metamodel/ivisual.h
  src/fife/model/metamodel/modelcoords.h
  src/fife/model/metamodel/object.h
  src/fife/model/metamodel/timeprovider.h
  src/fife/model/metamodel/grids/cellgrid.h
  src/fife/model/metamodel/grids/hexgrid.h
  src/fife/model/metamodel/grids/squaregrid.h
  src/fife/model/structures/cell.h
  src/fife/model/structures/cellcache.h
  src/fife/model/structures/instance.h
  src/fife/model/structures/instancetree.h
  src/fife/model/structures/layer.h
  src/fife/model/structures/location.h
  src/fife/model/structures/map.h
  src/fife/model/structures/renderernode.h
  src/fife/model/structures/trigger.h
  src/fife/model/structures/triggercontroller.h
  src/fife/pathfinder/route.h
  src/fife/pathfinder/routepather/multilayersearch.h
  src/fife/pathfinder/routepather/routepather.h
  src/fife/pathfinder/routepather/routepathersearch.h
  src/fife/pathfinder/routepather/singlelayersearch.h
  src/fife/savers/native/input/controllermappingsaver.h
  src/fife/savers/native/map/ianimationsaver.h
  src/fife/savers/native/map/iatlassaver.h
  src/fife/savers/native/map/imapsaver.h
  src/fife/savers/native/map/iobjectsaver.h
  src/fife/savers/native/map/mapsaver.h
  src/fife/util/base/exception.h
  src/fife/util/base/fifeclass.h
  src/fife/util/base/fife_stdint.h
  src/fife/util/base/sharedptr.h
  src/fife/util/base/singleton.h
  src/fife/util/base/stringutils.h
  src/fife/util/log/logger.h
  src/fife/util/math/angles.h
  src/fife/util/math/fife_math.h
  src/fife/util/math/matrix.h
  src/fife/util/resource/resource.h
  src/fife/util/resource/resourcemanager.h
  src/fife/util/structures/point.h
  src/fife/util/structures/priorityqueue.h
  src/fife/util/structures/purge.h
  src/fife/util/structures/quadtree.h
  src/fife/util/structures/rect.h
  src/fife/util/time/timeevent.h
  src/fife/util/time/timemanager.h
  src/fife/util/time/timer.h
  src/fife/vfs/directoryprovider.h
  src/fife/vfs/filesystem.h
  src/fife/vfs/vfs.h
  src/fife/vfs/vfsdirectory.h
  src/fife/vfs/vfssource.h
  src/fife/vfs/vfssourceprovider.h
  src/fife/vfs/dat/dat1.h
  src/fife/vfs/dat/dat2.h
  src/fife/vfs/dat/lzssdecoder.h
  src/fife/vfs/dat/rawdatadat1.h
  src/fife/vfs/dat/rawdatadat2.h
  src/fife/vfs/raw/rawdata.h
  src/fife/vfs/raw/rawdatafile.h
  src/fife/vfs/raw/rawdatamemsource.h
  src/fife/vfs/raw/rawdatasource.h
  src/fife/vfs/zip/zipfilesource.h
  src/fife/vfs/zip/zipnode.h
  src/fife/vfs/zip/zipprovider.h
  src/fife/vfs/zip/zipsource.h
  src/fife/vfs/zip/ziptree.h
  src/fife/video/animation.h
  src/fife/video/animationmanager.h
  src/fife/video/atlasbook.h
  src/fife/video/color.h
  src/fife/video/cursor.h
  src/fife/video/devicecaps.h
  src/fife/video/image.h
  src/fife/video/imagemanager.h
  src/fife/video/renderbackend.h
  src/fife/video/fonts/fontbase.h
  src/fife/video/fonts/ifont.h
  src/fife/video/fonts/imagefontbase.h
  src/fife/video/fonts/subimagefont.h
  src/fife/video/fonts/textrenderpool.h
  src/fife/video/fonts/truetypefont.h
  src/fife/video/opengl/fife_opengl.h
  src/fife/video/opengl/glimage.h
  src/fife/video/opengl/renderbackendopengl.h
  src/fife/video/sdl/renderbackendsdl.h
  src/fife/video/sdl/sdlblendingfunctions.h
  src/fife/video/sdl/sdlimage.h
  src/fife/view/camera.h
  src/fife/view/layercache.h
  src/fife/view/rendererbase.h
  src/fife/view/renderitem.h
  src/fife/view/visual.h
  src/fife/view/renderers/blockinginforenderer.h
  src/fife/view/renderers/cellrenderer.h
  src/fife/view/renderers/cellselectionrenderer.h
  src/fife/view/renderers/coordinaterenderer.h
  src/fife/view/renderers/floatingtextrenderer.h
  src/fife/view/renderers/genericrenderer.h
  src/fife/view/renderers/gridrenderer.h
  src/fife/view/renderers/instancerenderer.h
  src/fife/view/renderers/lightrenderer.h
  src/fife/view/renderers/offrenderer.h
  src/fife/view/renderers/quadtreerenderer.h
  src/fife/view/renderers/targetrenderer.h
  src/fife/gui/guimanager.h
)

set(
  FIFE_FIFEGUI_HEADERS
  src/fife/gui/fifechan/fifechanmanager.h
  src/fife/gui/fifechan/base/gui_font.h
  src/fife/gui/fifechan/base/gui_image.h
  src/fife/gui/fifechan/base/gui_imageloader.h
  src/fife/gui/fifechan/base/opengl/opengl_gui_graphics.h
  src/fife/gui/fifechan/base/sdl/sdl_gui_graphics.h
  src/fife/gui/fifechan/console/commandline.h
  src/fife/gui/fifechan/console/console.h
  src/fife/gui/fifechan/util/utf8/utf8stringeditor.h
  src/fife/gui/fifechan/widgets/animationicon.h
  src/fife/gui/fifechan/widgets/clicklabel.h
  src/fife/gui/fifechan/widgets/dockarea.h
  src/fife/gui/fifechan/widgets/panel.h
  src/fife/gui/fifechan/widgets/percentagebar.h
  src/fife/gui/fifechan/widgets/resizablewindow.h
)

#------------------------------------------------------------------------------
# Source Groups for Header Files
#------------------------------------------------------------------------------

source_group("fife"               FILES ${FIFE_CORE_HEADERS})
source_group("fife.gui.fifegui"   FILES ${FIFE_FIFEGUI_HEADERS})

add_custom_target(fife.headers SOURCES
    ${FIFE_CORE_HEADERS}
    ${FIFE_FIFEGUI_HEADERS}
)

#------------------------------------------------------------------------------
# SWIG Interface Files
#------------------------------------------------------------------------------

# The .i files are SWIG interface files.
# They define how the C++ code is exposed to the target language (here Python).
#
# 1. We have the following interface file sets:
#   - FIFE_SWIG: The main SWIG interface files for the core engine.
#   - FIFE_FIFEGUI_SWIG_LIB: The SWIG interface files for the FIFECHAN library (the base components).
#     These files declare the `%module fifechan` and are included in the seperate fifechan.i template.
#   - FIFE_FIFEGUI_SWIG: The SWIG interface files for the FIFE-specific extensions to FIFECHAN library (the actual GUI components).
#     The files declare the `%module fife` and become part of the main fife.i template.
# 2. We have a double substitution setup for the SWIG interface files:
#   - The .i.templ.cm files act as both CMake templates and SWIG interface templates.
#   - configure_file() performs the CMake substitution, replacing @...@ variables,
#     like CMAKE_SWIG_INCLUSIONS and generating the final fife.i with all .i files included.
#   - SWIG then processes this generated fife.i, resolving the includes and generating bindings.
# 3. Then swig_add_library() is called to generate the Python bindings.

set(
  FIFE_SWIG
  src/fife/version.i
  src/fife/audio/effects/soundeffect.i
  src/fife/audio/effects/soundfilter.i
  src/fife/audio/soundclip.i
  src/fife/audio/soundclipmanager.i
  src/fife/audio/soundemitter.i
  src/fife/audio/soundmanager.i
  src/fife/controller/engine.i
  src/fife/eventchannel/eventchannel.i
  src/fife/gui/guimanager.i
  src/fife/loaders/native/map/ianimationloader.i
  src/fife/loaders/native/map/iatlasloader.i
  src/fife/loaders/native/map/imaploader.i
  src/fife/loaders/native/map/iobjectloader.i
  src/fife/loaders/native/map/maploader.i
  src/fife/loaders/native/map/percentdonelistener.i
  src/fife/model/metamodel/action.i
  src/fife/model/metamodel/grids/cellgrids.i
  src/fife/model/metamodel/ipather.i
  src/fife/model/metamodel/ivisual.i
  src/fife/model/metamodel/modelcoords.i
  src/fife/model/metamodel/object.i
  src/fife/model/metamodel/timeprovider.i
  src/fife/model/model.i
  src/fife/model/structures/cell.i
  src/fife/model/structures/cellcache.i
  src/fife/model/structures/instance.i
  src/fife/model/structures/layer.i
  src/fife/model/structures/location.i
  src/fife/model/structures/map.i
  src/fife/model/structures/renderernode.i
  src/fife/model/structures/trigger.i
  src/fife/pathfinder/route.i
  src/fife/pathfinder/routepather/routepather.i
  src/fife/savers/native/map/ianimationsaver.i
  src/fife/savers/native/map/iatlassaver.i
  src/fife/savers/native/map/imapsaver.i
  src/fife/savers/native/map/iobjectsaver.i
  src/fife/savers/native/map/mapsaver.i
  src/fife/util/base/utilbase.i
  src/fife/util/log/logger.i
  src/fife/util/math/math.i
  src/fife/util/resource/resource.i
  src/fife/util/structures/utilstructures.i
  src/fife/util/time/timeevent.i
  src/fife/util/time/timemanager.i
  src/fife/vfs/raw/rawdata.i
  src/fife/vfs/vfs.i
  src/fife/video/fonts/fonts.i
  src/fife/video/video.i
  src/fife/view/camera.i
  src/fife/view/rendererbase.i
  src/fife/view/renderers/blockinginforenderer.i
  src/fife/view/renderers/cellrenderer.i
  src/fife/view/renderers/cellselectionrenderer.i
  src/fife/view/renderers/coordinaterenderer.i
  src/fife/view/renderers/floatingtextrenderer.i
  src/fife/view/renderers/genericrenderer.i
  src/fife/view/renderers/gridrenderer.i
  src/fife/view/renderers/instancerenderer.i
  src/fife/view/renderers/lightrenderer.i
  src/fife/view/renderers/offrenderer.i
  src/fife/view/renderers/targetrenderer.i
  src/fife/view/visual.i
)

set(
  FIFE_FIFEGUI_SWIG
  src/fife/gui/fifechan/base/gui_font.i
  src/fife/gui/fifechan/base/gui_image.i
  src/fife/gui/fifechan/console/console.i
  src/fife/gui/fifechan/fifechanmanager.i
  src/fife/gui/fifechan/widgets/widgets.i
)

set(
  FIFE_FIFEGUI_SWIG_LIB
  src/fife/gui/fifechan/lib/fifechanlistener.i
  src/fife/gui/fifechan/lib/widgets.i
)


#------------------------------------------------------------------------------
# Source Groups for SWIG Files
#------------------------------------------------------------------------------

source_group("fife.swig"                   FILES ${FIFE_SWIG})
source_group("fife.swig.gui.fifegui"       FILES ${FIFE_FIFEGUI_SWIG})
source_group("fife.swig.gui.fifegui.lib"   FILES ${FIFE_FIFEGUI_SWIG_LIB})

add_custom_target(fife.swig SOURCES ${FIFE_SWIG} ${FIFE_FIFEGUI_SWIG} ${FIFE_FIFEGUI_SWIG_LIB})

#------------------------------------------------------------------------------
# SWIG WRAPPER Files
#------------------------------------------------------------------------------

set(
  FIFE_SWIGWRAPPERS_PYTHON
  src/swigwrappers/python/extensions.i.templ
  src/swigwrappers/python/fife.i.templ.cm
  src/swigwrappers/python/fifechan.i.templ.cm
)

#------------------------------------------------------------------------------
# Source Groups for SWIG WRAPPER Files
#------------------------------------------------------------------------------

source_group("fife.swigwrappers.python" FILES ${FIFE_SWIGWRAPPERS_PYTHON})

add_custom_target(fife.swig.wrappers SOURCES ${FIFE_SWIGWRAPPERS_PYTHON})

#------------------------------------------------------------------------------
# Find Packages
#------------------------------------------------------------------------------

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)
find_package(Ogg CONFIG REQUIRED) # Ogg, not OGG

if(WIN32)
  find_package(Vorbis CONFIG REQUIRED) # Vorbis, not VORBIS
  find_package(Freetype CONFIG REQUIRED)
  find_package(GLEW CONFIG REQUIRED)
endif()

#------------------------------------------------------------------------------
# Packages requiring PKGCONFIG support to find them
#------------------------------------------------------------------------------
# For all packages without CMake config files:
# We try a quiet find_package first, this will fail and Package_FOUND
# will be false, then we try to find it using pkg-config.
#------------------------------------------------------------------------------
# Yes, looking at you upstream 💩!
#------------------------------------------------------------------------------

if(UNIX)

  find_package(PkgConfig REQUIRED)
  find_package(Vorbis CONFIG QUIET) # Vorbis, not VORBIS
  find_package(Freetype CONFIG QUIET)
  find_package(GLEW CONFIG QUIET)

  if (NOT Vorbis_FOUND)
    message(STATUS "Vorbis CMake config not found, trying pkg-config.")
    pkg_check_modules(VORBIS REQUIRED vorbis)
    pkg_check_modules(VORBISFILE QUIET vorbisfile)
    pkg_check_modules(VORBISENC QUIET vorbisenc)

    if(NOT VORBISFILE_FOUND)
      message(STATUS "vorbisfile pkg-config entry not found, falling back to vorbis link flags.")
      set(VORBISFILE_LINK_LIBRARIES "${VORBIS_LINK_LIBRARIES}")
      set(VORBISFILE_INCLUDE_DIRS "${VORBIS_INCLUDE_DIRS}")
    endif()

    if(NOT VORBISENC_FOUND)
      message(STATUS "vorbisenc pkg-config entry not found, falling back to vorbis link flags.")
      set(VORBISENC_LINK_LIBRARIES "${VORBIS_LINK_LIBRARIES}")
      set(VORBISENC_INCLUDE_DIRS "${VORBIS_INCLUDE_DIRS}")
    endif()

    message(STATUS "VORBIS_LINK_LIBRARIES=${VORBIS_LINK_LIBRARIES}")
    message(STATUS "VORBIS_INCLUDE_DIRS=${VORBIS_INCLUDE_DIRS}")
    message(STATUS "VORBISFILE_LINK_LIBRARIES=${VORBISFILE_LINK_LIBRARIES}")
    message(STATUS "VORBISFILE_INCLUDE_DIRS=${VORBISFILE_INCLUDE_DIRS}")
    message(STATUS "VORBISENC_LINK_LIBRARIES=${VORBISENC_LINK_LIBRARIES}")
    message(STATUS "VORBISENC_INCLUDE_DIRS=${VORBISENC_INCLUDE_DIRS}")
  endif()

  if(NOT Freetype_FOUND)
    message(STATUS "Freetype CMake config not found, trying pkg-config.")
    pkg_check_modules(FREETYPE REQUIRED freetype2)
  endif()

  if(NOT GLEW_FOUND)
    message(STATUS "GLEW CMake config not found, trying pkg-config.")
    pkg_check_modules(GLEW REQUIRED glew)

    message(STATUS "GLEW_INCLUDE_DIRS=${GLEW_INCLUDE_DIRS}")
    message(STATUS "GLEW_LIBRARIES=${GLEW_LIBRARIES}")
    message(STATUS "GLEW_LIB=${GLEW_LIB}")
  endif()

endif()

#------------------------------------------------------------------------------
# Define CMake Target fallbacks for dependencies without CMake config files.
#------------------------------------------------------------------------------
# Yes, looking at you upstream 💩!
#------------------------------------------------------------------------------

# Vorvis:vorbis target fallback
if (NOT TARGET Vorbis::vorbis)
    message(STATUS "Creating fallback Vorbis::vorbis target, because Vorbis CMake config not found.")
    add_library(Vorbis::vorbis INTERFACE IMPORTED)
    set_target_properties(Vorbis::vorbis PROPERTIES
      INTERFACE_LINK_LIBRARIES "${VORBIS_LINK_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${VORBIS_INCLUDE_DIRS}"
    )
endif()

# Vorbis::vorbisfile target fallback
# Vorbisfile typically links to libvorbisfile + libvorbis + libogg
if (NOT TARGET Vorbis::vorbisfile)
    message(STATUS "Creating fallback Vorbis::vorbisfile target, because Vorbis CMake config not found.")
    add_library(Vorbis::vorbisfile INTERFACE IMPORTED)
    set_target_properties(Vorbis::vorbisfile PROPERTIES
      INTERFACE_LINK_LIBRARIES "${VORBISFILE_LINK_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${VORBISFILE_INCLUDE_DIRS}"
    )
endif()

# Vorbis::vorbisenc target fallback
if (NOT TARGET Vorbis::vorbisenc)
    message(STATUS "Creating fallback Vorbis::vorbisenc target, because Vorbis CMake config not found.")
    add_library(Vorbis::vorbisenc INTERFACE IMPORTED)
    set_target_properties(Vorbis::vorbisenc PROPERTIES
      INTERFACE_LINK_LIBRARIES "${VORBISENC_LINK_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${VORBISENC_INCLUDE_DIRS}"
    )
endif()

# Freetype::Freetype target fallback
if (NOT TARGET Freetype::Freetype)
    message(STATUS "Creating fallback Freetype::Freetype target, because Freetype CMake config not found.")
    add_library(Freetype::Freetype INTERFACE IMPORTED)
    set_target_properties(Freetype::Freetype PROPERTIES
      INTERFACE_LINK_LIBRARIES "${FREETYPE_LINK_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${FREETYPE_INCLUDE_DIRS}"
    )
endif()

if (NOT TARGET GLEW::GLEW)
    message(STATUS "Creating fallback GLEW::GLEW target, because GLEW CMake config not found.")
    add_library(GLEW::GLEW UNKNOWN IMPORTED)

    # Because this is a list, but we need a file:
    # GLEW_LIBRARIES=GLEW;GL;X11;GLU;OpenGL
    file(GLOB GLEW_SO "${GLEW_LIBDIR}/libGLEW.so*")
    list(GET GLEW_SO 0 GLEW_LIB)

    set_target_properties(GLEW::GLEW PROPERTIES
        IMPORTED_LOCATION_RELEASE "${GLEW_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIRS}"
    )
endif()

# ------------------------------------------------------------------------------
# Optional dependencies (based on BUILD options)
# ------------------------------------------------------------------------------

if(ENABLE_OPENGL)
  find_package(OpenGL REQUIRED)
endif()

#------------------------------------------------------------------------------
# Build extensions (fifegui) as a Python extension module using SWIG
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

  foreach(filename ${FIFE_SWIG})
    list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${filename}")
    set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
  endforeach()
endif()

if(ENABLE_FIFEGUI)
  find_package(fifechan COMPONENTS sdl REQUIRED)
  if(ENABLE_OPENGL)
    find_package(fifechan COMPONENTS opengl REQUIRED)
  endif()

  # Guard: honor BUILD_SHARED_LIBS, but fail early when shared builds are
  # incompatible with static-only fifechan archives on Linux.
  if(UNIX AND NOT APPLE AND BUILD_SHARED_LIBS)
    get_target_property(FIFECHAN_IMPORTED_LOCATION_RELEASE fifechan::fifechan IMPORTED_LOCATION_RELEASE)
    get_target_property(FIFECHAN_IMPORTED_LOCATION fifechan::fifechan IMPORTED_LOCATION)
    set(FIFECHAN_IMPORTED_LIB "${FIFECHAN_IMPORTED_LOCATION_RELEASE}")
    if(NOT FIFECHAN_IMPORTED_LIB)
      set(FIFECHAN_IMPORTED_LIB "${FIFECHAN_IMPORTED_LOCATION}")
    endif()

    if(FIFECHAN_IMPORTED_LIB MATCHES "\\.a$")
        message(FATAL_ERROR
          "Library type mismatch:\n"
          "BUILD_SHARED_LIBS=ON requires fifechan to be built as a shared library.\n"
          "However, only a static library was found: '${FIFECHAN_IMPORTED_LIB}'.\n\n"
          "To fix this:\n"
          "  1. Rebuild fifechan with -DBUILD_SHARED_LIBS=ON.\n"
          "  2. Or configure this project with -DBUILD_SHARED_LIBS=OFF.\n"
        )
    endif()
  endif()

  if(BUILD_PYTHON_BINDINGS)
    set(SWIG_MODULE_fifechan_EXTRA_DEPS)

    # Files in FIFE_FIFEGUI_SWIG_LIB go into CMAKE_SWIG_INCLUSIONS_FCHAN (fifechan module)
    foreach(filename ${FIFE_FIFEGUI_SWIG_LIB})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${filename}")
      list(APPEND SWIG_MODULE_fifechan_EXTRA_DEPS "${filename}")
      set(CMAKE_SWIG_INCLUSIONS_FCHAN "${CMAKE_SWIG_INCLUSIONS_FCHAN}%include ${filename}\n")
    endforeach()

    # Files in FIFE_FIFEGUI_SWIG go into CMAKE_SWIG_INCLUSIONS (fife module).
    foreach(filename ${FIFE_FIFEGUI_SWIG})
      list(APPEND SWIG_MODULE_fife_EXTRA_DEPS "${filename}")
      list(APPEND SWIG_MODULE_fifechan_EXTRA_DEPS "${filename}")
      set(CMAKE_SWIG_INCLUSIONS "${CMAKE_SWIG_INCLUSIONS}%include ${filename}\n")
    endforeach()

    set(CMAKE_SWIG_IMPORTS "${CMAKE_SWIG_IMPORTS}%import(module=\"fife.fifechan\") ${PROJECT_BINARY_DIR}/fifechan.i")

    configure_file(
      "src/swigwrappers/python/fifechan.i.templ.cm"
      "${PROJECT_BINARY_DIR}/fifechan.i"
      @ONLY
    )
  endif(BUILD_PYTHON_BINDINGS)
endif(ENABLE_FIFEGUI)

if(BUILD_PYTHON_BINDINGS)
  configure_file(
    "src/swigwrappers/python/fife.i.templ.cm"
    "${PROJECT_BINARY_DIR}/fife.i"
    @ONLY
  )
  find_package(SWIG 4.2.1...4.4.1 REQUIRED)
  message(STATUS "SWIG version found: ${SWIG_VERSION}")
  include(${SWIG_USE_FILE})
endif()

#------------------------------------------------------------------------------
# Build fifengine as a Python extension module using SWIG
#------------------------------------------------------------------------------
#
# We have 2 swig_add_library() targets:
#   - _fife_swig     : main fife module
#   - _fifechan_swig : fifechan module
#
# 1. `_fife_swig` is the generated Python extension module.
#    It contains the SWIG-generated wrapper code and is linked directly with
#    the FIFE engine sources and their dependencies. The engine is built into
#    the extension module itself and the resulting module is self-contained,
#    so that when distributing the Python bindings, no separate `fife.so`
#    or `fife.dll` needs to be distributed.
#
# 2. `_fifechan_swig` is the generated Python extension module for FifeGUI.
#    It is separate from `_fife_swig` to allow importing the fifechan module
#    separately in Python, and to avoid exposing the fifechan internals in the
#    main fife module. The fifechan module is linked with the fifechan library
#    and its dependencies, but not with the main fife engine sources.
#    fife_swig requires fifechan_swig, because the main fife.i imports the
#    fifechan.i module. The dependencies need to be shipped: fifechan.so/dll
#    (optionally: fifechan_sdl3.so/dll, fifechan_opengl.so/dll).
#
# Relevant CMake docs:
#   - https://cmake.org/cmake/help/latest/module/FindPython3.html
#   - https://cmake.org/cmake/help/latest/module/FindSWIG.html
#   - https://cmake.org/cmake/help/latest/module/UseSWIG.html
#
# Relevant Swig docs:
#   - https://www.swig.org/Doc4.4/Python.html
#
#------------------------------------------------------------------------------

if(BUILD_PYTHON_BINDINGS)

  # We need the following components from the Python3 package:
  # - Interpreter → Python runtime (python3 executable)
  #   (requires python3 to be installed)
  # - Development.Module → headers and libraries for building extension modules
  #   (requires python3.X-dev to be installed, e.g. python3.13-dev)
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

  if(ENABLE_FIFEGUI)
    list(APPEND FIFE_SOURCES ${FIFE_FIFEGUI_SOURCES})
  endif()

  set_property(SOURCE "${PROJECT_BINARY_DIR}/fife.i" PROPERTY CPLUSPLUS ON)

  if(MSVC)
    # SWIG runtime code checks for macro define __GNUC__ in generated wrappers.
    # We need to define it for the generated sources,
    # so MSVC does not emit "undefined macro" warning C4668 for __GNUC__.
    set_property(
      SOURCE "${PROJECT_BINARY_DIR}/fife.i"
      APPEND PROPERTY SWIG_GENERATED_COMPILE_DEFINITIONS "__GNUC__=0"
    )
  endif()

  swig_add_library(fife_swig LANGUAGE python SOURCES "${PROJECT_BINARY_DIR}/fife.i")

  target_include_directories(fife_swig PRIVATE
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/src/fife
    ${Python3_INCLUDE_DIRS}
  )

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(fife_swig PRIVATE
      -Wno-unused-parameter
      -Wno-sometimes-uninitialized
    )
  endif()

  set_target_properties(fife_swig PROPERTIES
    SWIG_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR};${PROJECT_SOURCE_DIR}/src/fife;${PROJECT_SOURCE_DIR}"
  )

  target_link_libraries(
    fife_swig
    PRIVATE
      Python3::Module
      fife::fife
  )

  if(ENABLE_FIFEGUI)
    set_property(SOURCE "${PROJECT_BINARY_DIR}/fifechan.i" PROPERTY CPLUSPLUS ON)

    if(MSVC)
      # SWIG runtime code checks for macro define __GNUC__ in generated wrappers.
      # We need to define it for the generated sources,
      # so MSVC does not emit "undefined macro" warning C4668 for __GNUC__.
      set_property(
        SOURCE "${PROJECT_BINARY_DIR}/fifechan.i"
        APPEND PROPERTY SWIG_GENERATED_COMPILE_DEFINITIONS "__GNUC__=0"
      )
    endif()

    swig_add_library(fifechan_swig LANGUAGE python SOURCES "${PROJECT_BINARY_DIR}/fifechan.i")

    target_include_directories(fifechan_swig PRIVATE
      ${PROJECT_BINARY_DIR}
      src/fife
      ${PROJECT_SOURCE_DIR}
      ${Python3_INCLUDE_DIRS}
    )

    set_target_properties(fifechan_swig PROPERTIES
      SWIG_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR};${CMAKE_SOURCE_DIR}/src/fife;${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(fifechan_swig
      PRIVATE
        Python3::Module
        fifechan::fifechan
        $<IF:$<TARGET_EXISTS:fifechan::fifechan_sdl3>,fifechan::fifechan_sdl3,>
        $<IF:$<TARGET_EXISTS:fifechan::fifechan_opengl>,fifechan::fifechan_opengl,>
    )

  endif(ENABLE_FIFEGUI)

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
  if(TARGET fife_swig)
    install(TARGETS fife_swig DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
    install(FILES ${PROJECT_BINARY_DIR}/fife.py DESTINATION ${PYTHON_SITE_PACKAGES}/fife OPTIONAL)
  endif()
  install(FILES src/python/fife/__init__.py DESTINATION ${PYTHON_SITE_PACKAGES}/fife)

  # copy all files/folders from the extension folder
  # but exclude extension folders of the optional dependencies: pychan
  install(
    DIRECTORY src/python/fife/extensions/
    DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions
    PATTERN "extensions/pychan" EXCLUDE
    PATTERN "extensions/*"
  )

  if(ENABLE_FIFEGUI)
    # install pychan - fife's extension for fifechan
    install(
      DIRECTORY src/python/fife/extensions/pychan/
      DESTINATION ${PYTHON_SITE_PACKAGES}/fife/extensions/pychan
    )

    # install pythonized fifechan (only if swig target exists)
    if(TARGET fifechan_swig)
      install(TARGETS fifechan_swig DESTINATION ${PYTHON_SITE_PACKAGES}/fife)
      install(FILES ${PROJECT_BINARY_DIR}/fifechan.py DESTINATION ${PYTHON_SITE_PACKAGES}/fife OPTIONAL)
    endif()
  endif(ENABLE_FIFEGUI)
endif(BUILD_PYTHON_BINDINGS)

#------------------------------------------------------------------------------
# Build the fifengine C++ library (object library)
#------------------------------------------------------------------------------

if(BUILD_LIBRARY)

  # Create object library with all sources
  add_library(fife_lib OBJECT)
  add_library(fife::fife_lib ALIAS fife_lib)

  set_target_properties(fife_lib PROPERTIES
    CXX_STANDARD              20
    CXX_STANDARD_REQUIRED     ON
    POSITION_INDEPENDENT_CODE ON
  )

  target_sources(fife_lib
    PRIVATE
      ${FIFE_SOURCES}
      ${FIFE_FIFEGUI_SOURCES}
  )

  target_include_directories(fife_lib
    PRIVATE
      ${PROJECT_SOURCE_DIR}/src/fife
  )

#------------------------------------------------------------------------------
# FifeGUI core library (shared or static)
#------------------------------------------------------------------------------

  add_library(fife $<TARGET_OBJECTS:fife_lib>)

  # Provide a namespace alias for the library (fife::fife)
  add_library(fife::fife ALIAS fife)

  # expand conditionally outside target_sources(),
  # because generator expressions doesnt work there. 💩
  set(FIFE_PUBLIC_HEADERS ${FIFE_CORE_HEADERS})
  if(ENABLE_FIFEGUI)
    list(APPEND FIFE_PUBLIC_HEADERS ${FIFE_FIFEGUI_HEADERS})
  endif()

  # Define public interface for the library
  target_sources(fife
    PUBLIC
      FILE_SET fife_public_headers TYPE HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
      FILES
        ${FIFE_PUBLIC_HEADERS}

    PUBLIC
      FILE_SET generated_headers TYPE HEADERS
      BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/src
      FILES
        ${FIFE_VERSION_HEADER}
  )

  set_target_properties(fife PROPERTIES
    VERSION                ${FIFE_VERSION}
    SOVERSION              ${FIFE_VERSION}
    CLEAN_DIRECT_OUTPUT    1                        # Allow creating static and shared libraries without conflict
    OUTPUT_NAME            ${PROJECT_NAME}          # Avoid conflicts between library and binary target names
    LINKER_LANGUAGE        CXX
    # Verify that all headers in the header set can be included on their own.
    # This creates the target: fife_verify_interface_header_sets
    VERIFY_INTERFACE_HEADER_SETS ON
  )

  target_link_libraries(fife
    PUBLIC
      fife_options
      ZLIB::ZLIB
      PNG::PNG
      $<$<BOOL:${ENABLE_OPENGL}>:OpenGL::GL>
      $<$<BOOL:${ENABLE_OPENGL}>:GLEW::GLEW>
      Ogg::ogg
      Vorbis::vorbis
      Vorbis::vorbisfile
      Vorbis::vorbisenc
      OpenAL::OpenAL
      tinyxml2::tinyxml2
      utf8cpp::utf8cpp
      $<TARGET_NAME_IF_EXISTS:SDL3::SDL3main>
      $<TARGET_NAME_IF_EXISTS:SDL3::SDL3>
      $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image>,SDL3_image::SDL3_image,SDL3_image::SDL3_image-static>
      $<IF:$<TARGET_EXISTS:SDL3_ttf::SDL3_ttf>,SDL3_ttf::SDL3_ttf,SDL3_ttf::SDL3_ttf-static>
      $<$<BOOL:${ENABLE_FIFEGUI}>:fifechan::fifechan>
  )

  if(ENABLE_FIFEGUI AND ENABLE_OPENGL AND TARGET fifechan::fifechan_opengl)
    target_link_libraries(fife PUBLIC fifechan::fifechan_opengl)
  endif()

  if(ENABLE_FIFEGUI AND TARGET fifechan::fifechan_sdl3)
    target_link_libraries(fife PUBLIC fifechan::fifechan_sdl3)
  endif()

  target_include_directories(fife
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/fife>
      $<INSTALL_INTERFACE:include>
  )

  # Create unique compile PDB per-target to avoid MSVC C1041 conflicts
  # TODO install step is still missing
  if(MSVC)
    set_target_properties(fife PROPERTIES
      MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>"
      COMPILE_PDB_NAME "fife_compile_%p"
      COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/compile_pdbs/$<CONFIG>"
    )

    # Enable /FS to allow multiple cl.exe instances to safely write PDBs.
    target_compile_options(fife PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/FS>)
  endif()

  # The library exports symbols
  target_compile_definitions(fife PRIVATE
    $<$<BOOL:${BUILD_SHARED_LIBS}>:FIFE_BUILD>
  )

  # The consumers (including SWIG) need to know when we're building/using
  # a static library, to avoid incorrect symbol import/export.
  target_compile_definitions(fife PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:FIFE_STATIC>
  )

  if(BUILD_TESTS)
    # Disable adding CTest build targets like "NightlyStart".
    set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

    include(CTest)
    add_subdirectory(tests/core_tests)
  endif()

  show_build_target_properties(fife)

  # scikit-build-core wheel builds bundle runtime DLLs via delvewheel in CI.
  if(SKBUILD AND SKBUILD_STATE STREQUAL "wheel")
    message(STATUS "Skipping C++ library installation to avoid duplicate wheel content (SKBUILD_STATE=wheel).")
  else()
   install(TARGETS fife
     EXPORT fife-targets

     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}

     FILE_SET fife_public_headers
     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}

     FILE_SET generated_headers
     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}

     PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE
      GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE

     # Experimental!
     # EXPORT_PACKAGE_DEPENDENCIES
    )
  endif()

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
  # scikit-build-core wheel builds bundle runtime DLLs via delvewheel in CI.
  if(SKBUILD AND SKBUILD_STATE STREQUAL "wheel")
    message(STATUS "Skipping CMake based runtime DLL installation to avoid duplicate wheel content (SKBUILD_STATE=wheel).")
  else()
    install_fifechan_dlls()
    install_vcpkg_dlls()
    install_runtime_dlls()
  endif()
endif()

#-------------------------------------------------------------------
# Display Feature Summary
#-------------------------------------------------------------------

feature_summary(
  WHAT ALL
  DESCRIPTION "[Build Summary]\n"
  FATAL_ON_MISSING_REQUIRED_PACKAGES
)
