################################################################################
# FindCEGUI
#
# Locate CEGUI LIBRARIES
#
# This module defines
#    CEGUI_FOUND, if false, do not try to link to CEGUI
#    CEGUI_INCLUDE_DIR, where to find headers.
#    CEGUI_LIBRARIES, the LIBRARIES to link against
#    CEGUI_BINARY_REL - location of the main CEGUI binary (win32 non-static only, release)
#    CEGUI_BINARY_DBG - location of the main CEGUI binaries (win32 non-static only, debug)
#
#
#         Modules :
#    CEGUI_${COMPONENT}_FOUND - ${COMPONENT} is available
#    CEGUI_${COMPONENT}_INCLUDE_DIRS - additional include directories for ${COMPONENT}
#    CEGUI_${COMPONENT}_LIBRARIES - link these to use ${COMPONENT}
#    CEGUI_${COMPONENT}_BINARY_REL - location of the component binary (win32 non-static only, release)
#    CEGUI_${COMPONENT}_BINARY_DBG - location of the component binary (win32 non-static only, debug)
#
# VERSION 0.7.X:
#        WindowsRenderer:
#                Falagard
#        Renderer:
#                Direct3D9Renderer Direct3D10Renderer Direct3D11Renderer IrrlichtRenderer NullRenderer OgreRenderer OpenGLRenderer
#        ImageCodec:
#                CoronaImageCodec DevILImageCodec FreeImageImageCodec SILLYImageCodec STBImageCodec TGAImageCodec
#        Parser:
#                ExpatParser LibxmlParser RapidXMLParser TinyXMLParser XercesParser)
#        Script:
#                LuaScriptModule toluapp

# VERSION 0.8.X:
#        WindowsRenderer:
#                CoreWindowRendererSet
#        Renderer:
#                Direct3D9Renderer Direct3D10Renderer Direct3D11Renderer IrrlichtRenderer NullRenderer OgreRenderer OpenGLRenderer OpenGL3Renderer OpenGLESRenderer
#        ImageCodec:
#                CoronaImageCodec DevILImageCodec FreeImageImageCodec SILLYImageCodec STBImageCodec TGAImageCodec PVRImageCodec
#        Parser:
#                ExpatParser LibXMLParser RapidXMLParser TinyXMLParser XercesParser)
#        Script:
#                LuaScriptModule
#
# author: Guillaume Smaha
#
# People have asked us to include a FindCEGUI that is high quality that they will
# be able to copy and use to find CEGUI, so we have finally done that.
#
# If you have improvements for this scripts, please consider sending them upstream.
# See https://bitbucket.org/cegui/cegui
################################################################################


################################################################################
# Get the value of a preprocessor entry
################################################################################
macro(get_preprocessor_entry CONTENTS KEYWORD VARIABLE)
    string(REGEX MATCH
        "# *define +${KEYWORD} +((\"([^\n]*)\")|([^ \n]*))"
        PREPROC_TEMP_VAR
        ${${CONTENTS}}
    )
    if (CMAKE_MATCH_3)
        set(${VARIABLE} ${CMAKE_MATCH_3})
    else ()
        set(${VARIABLE} ${CMAKE_MATCH_4})
    endif ()
endmacro()

################################################################################
# Check if exist a preprocessor entry
################################################################################
macro(has_preprocessor_entry CONTENTS KEYWORD VARIABLE)
    string(REGEX MATCH
        "\n *# *define +(${KEYWORD})"
        PREPROC_TEMP_VAR
        ${${CONTENTS}}
    )
    if (CMAKE_MATCH_1)
        set(${VARIABLE} TRUE)
    else ()
        set(${VARIABLE} FALSE)
    endif ()
endmacro()

################################################################################
# Replace the value of a preprocessor entry
################################################################################
macro(replace_preprocessor_entry VARIABLE KEYWORD NEW_VALUE)
    string(REGEX REPLACE
        "(// *)?# *define +${KEYWORD} +[^ \n]*"
        "#define ${KEYWORD} ${NEW_VALUE}"
        ${VARIABLE}_TEMP
        ${${VARIABLE}}
    )
    set(${VARIABLE} ${${VARIABLE}_TEMP})
endmacro()

################################################################################
# Define the value of preprocessor entry
################################################################################
macro(set_preprocessor_entry VARIABLE KEYWORD ENABLE)
    if (${ENABLE})
        set(TMP_REPLACE_STR "#define ${KEYWORD}")
    else ()
        set(TMP_REPLACE_STR "// #define ${KEYWORD}")
    endif ()
    string(REGEX REPLACE
        "(// *)?# *define +${KEYWORD} *\n"
        ${TMP_REPLACE_STR}
        ${VARIABLE}_TEMP
        ${${VARIABLE}}
    )
    set(${VARIABLE} ${${VARIABLE}_TEMP})
endmacro()

################################################################################
# Begin processing of package
################################################################################
macro(findpkg_begin PREFIX)
    if (NOT ${PREFIX}_FIND_QUIETLY)
        message(STATUS "Looking for ${PREFIX}...")
    endif ()
endmacro(findpkg_begin)

################################################################################
# Display a status message unless FIND_QUIETLY is set
################################################################################
macro(pkg_message PREFIX)
    if (NOT ${PREFIX}_FIND_QUIETLY)
        message(STATUS ${ARGN})
    endif ()
endmacro(pkg_message)

################################################################################
# Get environment variable, define it as ENV_$var and make sure backslashes are converted to forward slashes
################################################################################
macro(getenv_path VAR)
     set(ENV_${VAR} $ENV{${VAR}})
     # replace won't work if var is blank
     if (ENV_${VAR})
         string( REGEX REPLACE "\\\\" "/" ENV_${VAR} ${ENV_${VAR}} )
     endif ()
endmacro(getenv_path)

################################################################################
# Construct search paths for includes and libraries from a PREFIX_PATH
################################################################################
macro(create_search_paths PREFIX)
    foreach(dir ${${PREFIX}_PREFIX_PATH})
        set(${PREFIX}_INC_SEARCH_PATH ${${PREFIX}_INC_SEARCH_PATH}
            ${dir}/include ${dir}/Include ${dir}/include/${PREFIX} ${dir}/Headers)
        set(${PREFIX}_INC_SEARCH_PATH ${${PREFIX}_INC_SEARCH_PATH}
            ${dir}/${PREFIX}/include ${dir}/${PREFIX}/Include ${dir}/${PREFIX}/include/${PREFIX} ${dir}/${PREFIX}/Headers)
        set(${PREFIX}_LIB_SEARCH_PATH ${${PREFIX}_LIB_SEARCH_PATH}
            ${dir}/lib ${dir}/Lib ${dir}/lib/${PREFIX} ${dir}/Libs)
        set(${PREFIX}_BIN_SEARCH_PATH ${${PREFIX}_BIN_SEARCH_PATH}
            ${dir}/bin)
    endforeach(dir)
    if(ANDROID)
        set(${PREFIX}_LIB_SEARCH_PATH ${${PREFIX}_LIB_SEARCH_PATH} ${OGRE_DEPENDENCIES_DIR}/lib/${ANDROID_ABI})
    endif()
    set(${PREFIX}_FRAMEWORK_SEARCH_PATH ${${PREFIX}_PREFIX_PATH})
endmacro(create_search_paths)

################################################################################
# clear cache variables if a certain variable changed
################################################################################
macro(clear_if_changed TESTVAR)
    # test against internal check variable
    # HACK: Apparently, adding a variable to the cache cleans up the list
    # a bit. We need to also remove any empty strings from the list, but
    # at the same time ensure that we are actually dealing with a list.
    list(APPEND ${TESTVAR} "")
    list(REMOVE_ITEM ${TESTVAR} "")
    if (NOT "${${TESTVAR}}" STREQUAL "${${TESTVAR}_INT_CHECK}")
        message(STATUS "${TESTVAR} changed.")
        foreach(var ${ARGN})
            set(${var} "NOTFOUND" CACHE STRING "${var}-docstring" FORCE)
        endforeach(var)
    endif ()
    set(${TESTVAR}_INT_CHECK ${${TESTVAR}} CACHE INTERNAL "${TESTVAR}-docstring" FORCE)
endmacro(clear_if_changed)

################################################################################
# Try to get some hints from pkg-config, if available
################################################################################
macro(use_pkgconfig PREFIX PKGNAME)
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
        pkg_check_modules(${PREFIX} ${PKGNAME} QUIET)
    endif ()
endmacro (use_pkgconfig)

################################################################################
# Couple a set of release AND debug libraries (or frameworks)
################################################################################
macro(make_library_set PREFIX)

    if (${PREFIX}_FWK)
        set(${PREFIX} ${${PREFIX}_FWK})
    elseif (${PREFIX}_REL AND ${PREFIX}_DBG)
        set(${PREFIX} optimized ${${PREFIX}_REL} debug ${${PREFIX}_DBG})
    elseif (${PREFIX}_REL)
        set(${PREFIX} ${${PREFIX}_REL})
    elseif (${PREFIX}_DBG)
        set(${PREFIX} ${${PREFIX}_DBG})
    endif ()
endmacro(make_library_set)

################################################################################
# Generate debug names from given release names
################################################################################
macro(get_debug_names PREFIX)
    foreach(i ${${PREFIX}})
        set(${PREFIX}_DBG ${${PREFIX}_DBG} ${i}d ${i}D ${i}_d ${i}_D ${i}_debug ${i})
    endforeach(i)
endmacro(get_debug_names)

################################################################################
# Add the parent dir from DIR to VAR
################################################################################
macro(add_parent_dir VAR DIR)
    get_filename_component(${DIR}_TEMP "${${DIR}}/.." ABSOLUTE)
    set(${VAR} ${${VAR}} ${${DIR}_TEMP})
endmacro(add_parent_dir)

################################################################################
# Do the final processing for the package find.
################################################################################
macro(findpkg_finish PREFIX)
    # skip if already processed during this run
    if (NOT ${PREFIX}_FOUND)
        if (${PREFIX}_INCLUDE_DIR AND ${PREFIX}_LIBRARY)
            set(${PREFIX}_FOUND TRUE)
            set(${PREFIX}_INCLUDE_DIRS ${${PREFIX}_INCLUDE_DIR})
            set(${PREFIX}_LIBRARIES ${${PREFIX}_LIBRARY})
            if (NOT ${PREFIX}_FIND_QUIETLY)
                message(STATUS "Found ${PREFIX}: ${${PREFIX}_LIBRARIES}")
            endif ()
        else ()
            if (NOT ${PREFIX}_FIND_QUIETLY)
                message(STATUS "Could not locate ${PREFIX}")
            endif ()
            if (${PREFIX}_FIND_REQUIRED)
                message(FATAL_ERROR "Required library ${PREFIX} not found! Install the library (including dev packages) and try again. If the library is already installed, set the missing variables manually in cmake.")
            endif ()
        endif ()

        mark_as_advanced(${PREFIX}_FOUND ${PREFIX}_LIBRARIES ${PREFIX}_INCLUDE_DIRS ${PREFIX}_INCLUDE_DIR ${PREFIX}_LIBRARY ${PREFIX}_LIBRARY_REL ${PREFIX}_LIBRARY_DBG ${PREFIX}_LIBRARY_FWK)
    endif ()
endmacro(findpkg_finish)


#########################################################
# Find cegui modules
#########################################################
macro(cegui_find_modules PLUGIN_COMPONENT)

    foreach (comp ${CEGUI_${PLUGIN_COMPONENT}_MODULES})

        set(LIBNAME ${comp})
        set(HEADER ${CEGUI_${PLUGIN_COMPONENT}_MODULES_${comp}_HEADER})
        set(PKGCONFIG ${CEGUI_${PLUGIN_COMPONENT}_MODULES_${comp}_PKGCONFIG})
        set(CHECKNOPREFIX ${CEGUI_${PLUGIN_COMPONENT}_MODULES_${comp}_CHECKNOPREFIX})

        if (NOT "${HEADER}" STREQUAL "")

            findpkg_begin(CEGUI_${comp})

            if(PKGCONFIG)
                use_pkgconfig(CEGUI_${comp}_PKGC "CEGUI-${PKGCONFIG}")
                if(NOT CEGUI_${comp}_PKGC_INCLUDE_DIRS AND CEGUI_LIB_SUFFIX)
                    use_pkgconfig(CEGUI_${comp}_PKGC "CEGUI${CEGUI_LIB_SUFFIX}-${PKGCONFIG}")
                endif()
            endif()

            list(LENGTH HEADER len)
            math(EXPR lenmax "${len} - 1")

            foreach(val RANGE ${lenmax})
                list(GET HEADER ${val} pathHeader)

                if(CEGUI_${comp}_INCLUDE_DIR)
                    set(CEGUI_${comp}_INCLUDE_DIR_TMP ${CEGUI_${comp}_INCLUDE_DIR})
                    unset(CEGUI_${comp}_INCLUDE_DIR CACHE)
                endif()

                get_filename_component(head "${pathHeader}" NAME)
                get_filename_component(dir "${pathHeader}" DIRECTORY)
                if (NOT "${dir}" STREQUAL "")
                    set(dir "${dir}/")
                endif ()

                find_path(CEGUI_${comp}_INCLUDE_DIR NAMES ${head} HINTS ${CEGUI_INCLUDE_DIRS} ${CEGUI_${comp}_PKGC_INCLUDE_DIRS} PATH_SUFFIXES ${dir} CEGUI/${dir})

                if(CEGUI_${comp}_INCLUDE_DIR_TMP)
                    set(CEGUI_${comp}_INCLUDE_DIR ${CEGUI_${comp}_INCLUDE_DIR} ${CEGUI_${comp}_INCLUDE_DIR_TMP})
                    unset(CEGUI_${comp}_INCLUDE_DIR_TMP CACHE)
                endif()

            endforeach()
            list(REMOVE_DUPLICATES CEGUI_${comp}_INCLUDE_DIR)

            set(CEGUI_SUBLIB_DIR "cegui-${CEGUI_VERSION_MAJOR}.${CEGUI_VERSION_MINOR}")


            set(CEGUI_${comp}_LIBRARY_NAMES "CEGUI${comp}")
            get_debug_names(CEGUI_${comp}_LIBRARY_NAMES)

            set(CEGUI_${comp}_LIBRARY_NAMES_SUFFIX "CEGUI${comp}${CEGUI_LIB_SUFFIX}")
            get_debug_names(CEGUI_${comp}_LIBRARY_NAMES_SUFFIX)

            find_library(CEGUI_${comp}_LIBRARY_REL NAMES ${CEGUI_${comp}_LIBRARY_NAMES} ${CEGUI_${comp}_LIBRARY_NAMES_SUFFIX} HINTS ${CEGUI_LIBRARY_DIR_REL} ${CEGUI_LIBRARY_DIR_REL}/${CEGUI_SUBLIB_DIR} ${CEGUI_${comp}_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" "Release" "RelWithDebInfo" "MinSizeRel")
            find_library(CEGUI_${comp}_LIBRARY_DBG NAMES ${CEGUI_${comp}_LIBRARY_NAMES_DBG} ${CEGUI_${comp}_LIBRARY_NAMES_SUFFIX_DBG} HINTS ${CEGUI_LIBRARY_DIR_DBG} ${CEGUI_LIBRARY_DIR_DBG}/${CEGUI_SUBLIB_DIR} ${CEGUI_${comp}_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" "Debug")

            if(CHECKNOPREFIX AND (NOT CEGUI_${comp}_LIBRARY_REL OR NOT CEGUI_${comp}_LIBRARY_DBG))
                set(CEGUI_${comp}_LIBRARY_NAMES "${comp}")
                get_debug_names(CEGUI_${comp}_LIBRARY_NAMES)

                set(CEGUI_${comp}_LIBRARY_NAMES_SUFFIX "${comp}${CEGUI_LIB_SUFFIX}")
                get_debug_names(CEGUI_${comp}_LIBRARY_NAMES_SUFFIX)

                find_library(CEGUI_${comp}_LIBRARY_REL NAMES ${CEGUI_${comp}_LIBRARY_NAMES} ${CEGUI_${comp}_LIBRARY_NAMES_SUFFIX} HINTS ${CEGUI_LIBRARY_DIR_REL} ${CEGUI_LIBRARY_DIR_REL}/${CEGUI_SUBLIB_DIR} ${CEGUI_${comp}_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" "Release" "RelWithDebInfo" "MinSizeRel")
                find_library(CEGUI_${comp}_LIBRARY_DBG NAMES ${CEGUI_${comp}_LIBRARY_NAMES_DBG} ${CEGUI_${comp}_LIBRARY_NAMES_SUFFIX_DBG} HINTS ${CEGUI_LIBRARY_DIR_DBG} ${CEGUI_LIBRARY_DIR_DBG}/${CEGUI_SUBLIB_DIR} ${CEGUI_${comp}_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" "Debug")
            endif()

            make_library_set(CEGUI_${comp}_LIBRARY)

            if (NOT CEGUI_STATIC)
                if (WIN32)
                    find_file(CEGUI_${comp}_BINARY_REL NAMES "CEGUI${comp}.dll" "CEGUI${comp}${CEGUI_LIB_SUFFIX}.dll" HINTS ${CEGUI_BIN_SEARCH_PATH} ${CEGUI_${comp}_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" "Release" "RelWithDebInfo" "MinSizeRel")
                    find_file(CEGUI_${comp}_BINARY_DBG NAMES "CEGUI${comp}_d.dll" "CEGUI${comp}${CEGUI_LIB_SUFFIX}_d.dll" HINTS ${CEGUI_BIN_SEARCH_PATH} ${CEGUI_${comp}_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" "Debug")

                     if(CHECKNOPREFIX AND (NOT CEGUI_${comp}_LIBRARY_REL OR NOT CEGUI_${comp}_LIBRARY_DBG))
                         find_file(CEGUI_${comp}_BINARY_REL NAMES "${comp}${CEGUI_LIB_SUFFIX}.dll" HINTS ${CEGUI_BIN_SEARCH_PATH} PATH_SUFFIXES "" "Release" "RelWithDebInfo" "MinSizeRel")
                         find_file(CEGUI_${comp}_BINARY_DBG NAMES "${comp}${CEGUI_LIB_SUFFIX}_d.dll" HINTS ${CEGUI_BIN_SEARCH_PATH} PATH_SUFFIXES "" "Debug")
                    endif()
                endif()

                get_filename_component(CEGUI_${comp}_BINARY_DIR_REL "${CEGUI_${comp}_BINARY_REL}" PATH)
                get_filename_component(CEGUI_${comp}_BINARY_DIR_DBG "${CEGUI_${comp}_BINARY_DBG}" PATH)
                mark_as_advanced(CEGUI_${comp}_BINARY_REL CEGUI_${comp}_BINARY_DBG CEGUI_${comp}_BINARY_DIR_REL CEGUI_${comp}_BINARY_DIR_DBG)
            endif()

            findpkg_finish(CEGUI_${comp})

        endif ()

    endforeach (comp)

endmacro(cegui_find_modules)


#########################################################
# register a module
#########################################################
macro(cegui_register_module TYPE LIBNAME HEADER PKGCONFIG)
        set(CEGUI_MODULES ${CEGUI_MODULES} ${LIBNAME})
        set(CEGUI_${TYPE}_MODULES ${CEGUI_${TYPE}_MODULES} ${LIBNAME})
        set(CEGUI_${TYPE}_MODULES_${LIBNAME}_HEADER ${HEADER})
        if(NOT "${PKGCONFIG}" STREQUAL "")
            set(CEGUI_${TYPE}_MODULES_${LIBNAME}_PKGCONFIG ${PKGCONFIG})
        endif()
endmacro()


#########################################################
# Main
#########################################################


set(CEGUI_VERSION_MAJOR_DEFAULT "0")

# Register var to check change on base
set(CEGUI_RESET_VARS CEGUI_CONFIG_INCLUDE_DIR CEGUI_INCLUDE_DIR CEGUI_LIBRARY_REL CEGUI_LIBRARY_DBG)
set(CEGUI_PREFIX_WATCH ${CEGUI_PREFIX_PATH})
clear_if_changed(CEGUI_PREFIX_WATCH ${CEGUI_RESET_VARS})

findpkg_begin(CEGUI)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(CEGUI_HOME)
getenv_path(CEGUI_DIR)
getenv_path(CEGUI_ROOT)
getenv_path(PROGRAMFILES)

# Determine whether to search for a dynamic or static build
if (CEGUI_STATIC)
    set(CMAKE_FIND_LIBRARY_SUFFIXES_OLD "${CMAKE_FIND_LIBRARY_SUFFIXES}")
    if (WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib;.dll")
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")
    endif()
endif ()

# construct search paths from environmental hints and
# OS specific guesses
if (WIN32)
    set(CEGUI_PREFIX_GUESSES
        ${ENV_PROGRAMFILES}/cegui
        ${ENV_PROGRAMFILES}/CEGUI
        C:/CEGUI-SDK
        C:/lib/cegui
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;CEGUI_HOME]
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;CEGUI_DIR]
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;CEGUI_ROOT]
    )
elseif (UNIX)
    set(CEGUI_PREFIX_GUESSES
        /opt/cegui
        /opt/CEGUI
        /usr
        /usr/local
        $ENV{HOME}/cegui
        $ENV{HOME}/CEGUI
    )
    if (APPLE)
        set(CEGUI_PREFIX_GUESSES
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/macosx
            ${CEGUI_PREFIX_GUESSES}
        )
    endif ()
endif ()

set(CEGUI_PREFIX_PATH
        $ENV{CEGUI_HOME} $ENV{CEGUI_DIR} $ENV{CEGUI_ROOT}
        ${CEGUI_PREFIX_GUESSES}
)

# Construct search paths for includes and libraries
create_search_paths(CEGUI)



# try to locate CEGUI via pkg-config
use_pkgconfig(CEGUI_PKGC "CEGUI${CEGUI_LIB_SUFFIX};CEGUI${CEGUI_LIB_SUFFIX}-${CEGUI_VERSION_MAJOR_DEFAULT}")

# locate CEGUI include files
find_path(CEGUI_CONFIG_INCLUDE_DIR_PREFIX NAMES CEGUIConfig.h HINTS ${CEGUI_INC_SEARCH_PATH} ${CEGUI_FRAMEWORK_INCLUDES} ${CEGUI_PKGC_INCLUDE_DIRS} PATH_SUFFIXES CEGUI cegui cegui-${CEGUI_VERSION_MAJOR_DEFAULT} cegui-${CEGUI_VERSION_MAJOR_DEFAULT}/CEGUI)
find_path(CEGUI_CONFIG_INCLUDE_DIR NAMES CEGUIConfig.h Config.h HINTS ${CEGUI_INC_SEARCH_PATH} ${CEGUI_FRAMEWORK_INCLUDES} ${CEGUI_PKGC_INCLUDE_DIRS} PATH_SUFFIXES CEGUI cegui cegui-${CEGUI_VERSION_MAJOR_DEFAULT} cegui-${CEGUI_VERSION_MAJOR_DEFAULT}/CEGUI)
find_path(CEGUI_INCLUDE_DIR NAMES CEGUI.h HINTS ${CEGUI_INC_SEARCH_PATH} ${CEGUI_FRAMEWORK_INCLUDES} ${CEGUI_PKGC_INCLUDE_DIRS} PATH_SUFFIXES CEGUI cegui cegui-${CEGUI_VERSION_MAJOR_DEFAULT} cegui-${CEGUI_VERSION_MAJOR_DEFAULT}/CEGUI)

set(CEGUI_INCOMPATIBLE FALSE)
if (CEGUI_INCLUDE_DIR)
    # determine CEGUI version
    if(CEGUI_CONFIG_INCLUDE_DIR_PREFIX)
        file(READ ${CEGUI_INCLUDE_DIR}/CEGUIVersion.h CEGUI_TEMP_VERSION_CONTENT)
    else()
        file(READ ${CEGUI_INCLUDE_DIR}/Version.h CEGUI_TEMP_VERSION_CONTENT)
    endif()

    if (NOT "${CEGUI_TEMP_VERSION_CONTENT}" STREQUAL "")
        get_preprocessor_entry(CEGUI_TEMP_VERSION_CONTENT CEGUI_VERSION_MAJOR CEGUI_VERSION_MAJOR)
        get_preprocessor_entry(CEGUI_TEMP_VERSION_CONTENT CEGUI_VERSION_MINOR CEGUI_VERSION_MINOR)
        get_preprocessor_entry(CEGUI_TEMP_VERSION_CONTENT CEGUI_VERSION_PATCH CEGUI_VERSION_PATCH)
        get_preprocessor_entry(CEGUI_TEMP_VERSION_CONTENT CEGUI_VERSION_NAME CEGUI_VERSION_NAME)
        set(CEGUI_VERSION "${CEGUI_VERSION_MAJOR}.${CEGUI_VERSION_MINOR}.${CEGUI_VERSION_PATCH}")
        pkg_message(CEGUI "Found CEGUI ${CEGUI_VERSION_NAME} (${CEGUI_VERSION})")
    else()
        message(SEND_ERROR "Can't found CEGUI version !")
        set(CEGUI_INCOMPATIBLE FALSE)
    endif()

else ()
    set(CEGUI_INCOMPATIBLE FALSE)
endif ()


if ("${CEGUI_VERSION}" VERSION_GREATER "0.8.0")
    set(CEGUI_LIB_SUFFIX "-${CEGUI_VERSION_MAJOR}")
endif ()


set(CEGUI_LIBRARY_NAMES "CEGUIBase${CEGUI_LIB_SUFFIX}")
get_debug_names(CEGUI_LIBRARY_NAMES)
find_library(CEGUI_LIBRARY_REL NAMES ${CEGUI_LIBRARY_NAMES} HINTS ${CEGUI_LIB_SEARCH_PATH} ${CEGUI_PKGC_LIBRARY_DIRS} ${CEGUI_FRAMEWORK_SEARCH_PATH} PATH_SUFFIXES "" "release" "relwithdebinfo" "minsizerel")
find_library(CEGUI_LIBRARY_DBG NAMES ${CEGUI_LIBRARY_NAMES_DBG} HINTS ${CEGUI_LIB_SEARCH_PATH} ${CEGUI_PKGC_LIBRARY_DIRS} ${CEGUI_FRAMEWORK_SEARCH_PATH} PATH_SUFFIXES "" "debug")
make_library_set(CEGUI_LIBRARY)


if (CEGUI_INCOMPATIBLE)
    set(CEGUI_LIBRARY "NOTFOUND")
endif ()

set(CEGUI_INCLUDE_DIR ${CEGUI_CONFIG_INCLUDE_DIR} ${CEGUI_INCLUDE_DIR})
list(REMOVE_DUPLICATES CEGUI_INCLUDE_DIR)
findpkg_finish(CEGUI)

# Starting CEGUI 0.8, the headers are in the CEGUI subdirectory, and we must include
# its parent rather than directly the subdirectory. Otherwise we'll get compiler errors.
if ("${CEGUI_VERSION}" VERSION_GREATER "0.8.0")
    add_parent_dir(CEGUI_INCLUDE_DIRS_TEMP CEGUI_INCLUDE_DIR)
    # we basically overwrite CEGUI_INCLUDE_DIR so that it contains only the parent directory.
    set(CEGUI_INCLUDE_DIR ${CEGUI_INCLUDE_DIRS_TEMP})
endif()

set(CEGUI_INCLUDE_DIRS ${CEGUI_INCLUDE_DIR})

mark_as_advanced(CEGUI_CONFIG_INCLUDE_DIR)

if (NOT CEGUI_FOUND)
    return()
endif ()


get_filename_component(CEGUI_LIBRARY_DIR_REL "${CEGUI_LIBRARY_REL}" PATH)
get_filename_component(CEGUI_LIBRARY_DIR_DBG "${CEGUI_LIBRARY_DBG}" PATH)
set(CEGUI_LIBRARY_DIRS ${CEGUI_LIBRARY_DIR_REL} ${CEGUI_LIBRARY_DIR_DBG})


# find binaries
if (NOT CEGUI_STATIC)
        if (WIN32)
                find_file(CEGUI_BINARY_REL NAMES "CEGUIBase${CEGUI_LIB_SUFFIX}.dll" "CEGUIBase.dll" HINTS ${CEGUI_BIN_SEARCH_PATH} PATH_SUFFIXES "" "Release" "RelWithDebInfo" "MinSizeRel")
                find_file(CEGUI_BINARY_DBG NAMES "CEGUIBase${CEGUI_LIB_SUFFIX}_d.dll" "CEGUIBase_d.dll" HINTS ${CEGUI_BIN_SEARCH_PATH} PATH_SUFFIXES "" "Debug")
        endif()

        get_filename_component(CEGUI_BINARY_DIR_REL "${CEGUI_BINARY_REL}" PATH)
        get_filename_component(CEGUI_BINARY_DIR_DBG "${CEGUI_BINARY_DBG}" PATH)
        mark_as_advanced(CEGUI_BINARY_REL CEGUI_BINARY_DBG CEGUI_BINARY_DIR_REL CEGUI_BINARY_DIR_DBG)
endif()

if ("${CEGUI_VERSION}" VERSION_GREATER "0.8.0")
    # Base
    cegui_register_module(BASE CommonDialogs CommonDialogs/Module.h "")

    # WindowsRenderer
    cegui_register_module(WINDOWSRENDERER CoreWindowRendererSet WindowRendererSets/Core/Module.h "")

    # Renderer
    cegui_register_module(RENDERER Direct3D9Renderer RendererModules/Direct3D9/Renderer.h "")
    cegui_register_module(RENDERER Direct3D10Renderer RendererModules/Direct3D10/Renderer.h "")
    cegui_register_module(RENDERER Direct3D11Renderer RendererModules/Direct3D11/Renderer.h "")
    cegui_register_module(RENDERER DirectFBRenderer RendererModules/DirectFB/Renderer.h "")
    cegui_register_module(RENDERER IrrlichtRenderer RendererModules/Irrlicht/Renderer.h "IRRLICHT")
    cegui_register_module(RENDERER NullRenderer RendererModules/Null/Renderer.h "NULL")
    cegui_register_module(RENDERER OgreRenderer RendererModules/Ogre/Renderer.h "OGRE")
    cegui_register_module(RENDERER OpenGLRenderer RendererModules/OpenGL/GLRenderer.h "OPENGL")
    cegui_register_module(RENDERER OpenGL3Renderer RendererModules/OpenGL/GL3Renderer.h "OPENGL3")
    cegui_register_module(RENDERER OpenGLESRenderer RendererModules/OpenGLES/Renderer.h "")

    # ImageCodec
    cegui_register_module(IMAGECODEC CoronaImageCodec ImageCodecModules/Corona/ImageCodec.h "")
    cegui_register_module(IMAGECODEC DevILImageCodec ImageCodecModules/DevIL/ImageCodec.h "")
    cegui_register_module(IMAGECODEC FreeImageImageCodec ImageCodecModules/FreeImage/ImageCodec.h "")
    cegui_register_module(IMAGECODEC SILLYImageCodec ImageCodecModules/SILLY/ImageCodec.h "")
    cegui_register_module(IMAGECODEC STBImageCodec ImageCodecModules/STB/ImageCodec.h "")
    cegui_register_module(IMAGECODEC TGAImageCodec ImageCodecModules/TGA/ImageCodec.h "")
    cegui_register_module(IMAGECODEC PVRImageCodec ImageCodecModules/PVR/ImageCodec.h "")

    # Parser
    cegui_register_module(PARSER ExpatParser XMLParserModules/Expat/XMLParser.h "")
    cegui_register_module(PARSER LibXMLParser XMLParserModules/Libxml2/XMLParser.h "")
    cegui_register_module(PARSER RapidXMLParser XMLParserModules/RapidXML/XMLParser.h "")
    cegui_register_module(PARSER TinyXMLParser XMLParserModules/TinyXML/XMLParser.h "")
    cegui_register_module(PARSER XercesParser XMLParserModules/Xerces/XMLParser.h "")

    # Script
    cegui_register_module(SCRIPT LuaScriptModule ScriptModules/Lua/ScriptModule.h "LUA")

else ()

    # WindowsRenderer
    cegui_register_module(WINDOWSRENDERER FalagardWRBase "WindowRendererSets/Falagard/FalModule.h;falagard/CEGUIFalNamedArea.h" "")

    # Renderer
    cegui_register_module(RENDERER Direct3D9Renderer RendererModules/Direct3D9/CEGUIDirect3D9Renderer.h "")
    cegui_register_module(RENDERER Direct3D10Renderer RendererModules/Direct3D10/CEGUIDirect3D10Renderer.h "")
    cegui_register_module(RENDERER Direct3D11Renderer RendererModules/Direct3D11/CEGUIDirect3D11Renderer.h "")
    cegui_register_module(RENDERER DirectFBRenderer RendererModules/DirectFB/CEGUIDirectFBRenderer.h "")
    cegui_register_module(RENDERER IrrlichtRenderer RendererModules/Irrlicht/CEGUIIrrlichtRenderer.h "")
    cegui_register_module(RENDERER NullRenderer RendererModules/Null/CEGUINullRenderer.h "NULL")
    cegui_register_module(RENDERER OgreRenderer RendererModules/Ogre/CEGUIOgreRenderer.h "OGRE")
    cegui_register_module(RENDERER OpenGLRenderer RendererModules/OpenGL/CEGUIOpenGLRenderer.h "OPENGL")

    # ImageCodec
    cegui_register_module(IMAGECODEC CoronaImageCodec ImageCodecModules/CoronaImageCodec/CEGUICoronaImageCodec.h "")
    cegui_register_module(IMAGECODEC DevILImageCodec ImageCodecModules/DevILImageCodec/CEGUIDevILImageCodec.h "")
    cegui_register_module(IMAGECODEC FreeImageImageCodec ImageCodecModules/FreeImageImageCodec/CEGUIFreeImageImageCodec.h "")
    cegui_register_module(IMAGECODEC SILLYImageCodec ImageCodecModules/SILLYImageCodec/CEGUISILLYImageCodec.h "")
    cegui_register_module(IMAGECODEC STBImageCodec ImageCodecModules/STBImageCodec/CEGUISTBImageCodec.h "")
    cegui_register_module(IMAGECODEC TGAImageCodec ImageCodecModules/TGAImageCodec/CEGUITGAImageCodec.h "")

    # Parser
    cegui_register_module(PARSER ExpatParser XMLParserModules/ExpatParser/CEGUIExpatParser.h "")
    cegui_register_module(PARSER LibxmlParser XMLParserModules/LibxmlParser/CEGUILibxmlParser.h "")
    cegui_register_module(PARSER RapidXMLParser XMLParserModules/RapidXMLParser/CEGUIRapidXMLParser.h "")
    cegui_register_module(PARSER TinyXMLParser XMLParserModules/TinyXMLParser/CEGUITinyXMLParser.h "")
    cegui_register_module(PARSER XercesParser XMLParserModules/XercesParser/CEGUIXercesParser.h "")

    # Script
    cegui_register_module(SCRIPT LuaScriptModule ScriptingModules/LuaScriptModule/CEGUILua.h "")

    if(WIN32)
        cegui_register_module(SCRIPT tolua++ ScriptingModules/LuaScriptModule/support/tolua++/tolua++.h "")
        set(CEGUI_SCRIPT_MODULES_tolua++_CHECKNOPREFIX TRUE)
    else()
        cegui_register_module(SCRIPT toluapp ScriptingModules/LuaScriptModule/support/tolua++/tolua++.h "")
    endif()

endif ()


# Register var to check change on modules
set(CEGUI_RESET_MODULES_VARS "")
foreach (comp ${CEGUI_MODULES})
    set(CEGUI_RESET_MODULES_VARS ${CEGUI_RESET_MODULES_VARS}
        CEGUI_${comp}_INCLUDE_DIR CEGUI_${comp}_LIBRARY_FWK
        CEGUI_${comp}_LIBRARY_DBG CEGUI_${comp}_LIBRARY_REL
    )
endforeach (comp)
set(CEGUI_PREFIX_MODULES_WATCH ${CEGUI_PREFIX_MODULES_PATH})
clear_if_changed(CEGUI_PREFIX_WATCH ${CEGUI_RESET_MODULES_VARS})


# Find modules
cegui_find_modules(BASE)
cegui_find_modules(WINDOWSRENDERER)
cegui_find_modules(RENDERER)
cegui_find_modules(IMAGECODEC)
cegui_find_modules(PARSER)
cegui_find_modules(SCRIPT)


# Find modules
if (CEGUI_STATIC)
    set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES_OLD}")
endif()


#Check change on modules
clear_if_changed(CEGUI_PREFIX_WATCH)
clear_if_changed(CEGUI_PREFIX_MODULES_WATCH)
