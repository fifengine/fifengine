#------------------------------------------------------------------------------
# InstallRuntimeDeps.cmake
# Copies runtime DLLs to CMAKE_INSTALL_LIBDIR during install phase
#
# For Debug builds, DLLs are installed to CMAKE_INSTALL_LIBDIR/debug/
# For Release/RelWithDebInfo builds, DLLs are installed to CMAKE_INSTALL_LIBDIR/
#
# Usage:
#   include(InstallRuntimeDeps)
#   install_fifechan_dlls()
#   install_vcpkg_dlls()
#   install_runtime_dlls()
#------------------------------------------------------------------------------

include_guard(GLOBAL)

set(_RUNTIME_DLLS "" CACHE INTERNAL "")

#------------------------------------------------------------------------------
# _get_dll_destination
# Returns the DLL destination directory based on build type
#------------------------------------------------------------------------------
function(_get_dll_destination OUTPUT_VAR)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(${OUTPUT_VAR} "${CMAKE_INSTALL_LIBDIR}/debug" PARENT_SCOPE)
    else()
        set(${OUTPUT_VAR} "${CMAKE_INSTALL_LIBDIR}" PARENT_SCOPE)
    endif()
endfunction()

#------------------------------------------------------------------------------
# _collect_dll
# Internal: Add DLL to collection (collects both release and debug DLLs)
#------------------------------------------------------------------------------
function(_collect_dll DLL_PATH)
    if(NOT EXISTS "${DLL_PATH}")
        return()
    endif()
    get_filename_component(DLL_NAME "${DLL_PATH}" NAME)
    list(FIND _RUNTIME_DLLS "${DLL_PATH}" _INDEX)
    if(_INDEX EQUAL -1)
        list(APPEND _RUNTIME_DLLS "${DLL_PATH}")
        set(_RUNTIME_DLLS "${_RUNTIME_DLLS}" CACHE INTERNAL "")
    endif()
endfunction()

#------------------------------------------------------------------------------
# install_fifechan_dlls
# Collects FifeChan DLLs from the dependencies build directory
# Only collects DLLs matching the current build type
#------------------------------------------------------------------------------
function(install_fifechan_dlls)
    if(NOT WIN32)
        return()
    endif()

    if(NOT DEFINED DEPENDENCY_INSTALL_DIR)
        message(VERBOSE "DEPENDENCY_INSTALL_DIR not set, skipping FifeChan DLLs")
        return()
    endif()

    set(_FIFECHAN_NAMES fifechan fifechan_sdl fifechan_opengl)

    # Only search the directory matching the current build type
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_SEARCH_SUBDIRS "lib/debug" "bin/debug")
    else()
        set(_SEARCH_SUBDIRS "lib" "bin")
    endif()

    foreach(_SUBDIR IN LISTS _SEARCH_SUBDIRS)
        set(_SEARCH_DIR "${DEPENDENCY_INSTALL_DIR}/${_SUBDIR}")
        if(EXISTS "${_SEARCH_DIR}")
            foreach(_NAME IN LISTS _FIFECHAN_NAMES)
                foreach(_EXT dll DLL)
                    set(_DLL "${_SEARCH_DIR}/${_NAME}.${_EXT}")
                    _collect_dll("${_DLL}")
                endforeach()
            endforeach()
        endif()
    endforeach()
endfunction()

#------------------------------------------------------------------------------
# install_vcpkg_dlls
# Collects vcpkg-managed DLLs from the vcpkg installed directory
# Only collects DLLs matching the current build type
#------------------------------------------------------------------------------
function(install_vcpkg_dlls)
    if(NOT WIN32)
        return()
    endif()

    if(NOT DEFINED VCPKG_INSTALLED_DIR OR NOT DEFINED VCPKG_TARGET_TRIPLET)
        message(VERBOSE "VCPKG_INSTALLED_DIR or VCPKG_TARGET_TRIPLET not set, skipping vcpkg DLLs")
        return()
    endif()

    # Only search the directory matching the current build type
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_VCPKG_SUBDIR "debug/bin")
    else()
        set(_VCPKG_SUBDIR "bin")
    endif()

    set(_VCPKG_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/${_VCPKG_SUBDIR}")
    if(EXISTS "${_VCPKG_DIR}")
        file(GLOB _DLL_FILES "${_VCPKG_DIR}/*.dll")
        foreach(_DLL IN LISTS _DLL_FILES)
            _collect_dll("${_DLL}")
        endforeach()
    endif()
endfunction()

#------------------------------------------------------------------------------
# install_runtime_dlls
# Installs collected DLLs.
#------------------------------------------------------------------------------
function(install_runtime_dlls)
    if(NOT WIN32 OR _RUNTIME_DLLS STREQUAL "")
        return()
    endif()

    _get_dll_destination(_DEST)

    set(_SCRIPT_FILE "${CMAKE_CURRENT_BINARY_DIR}/InstallRuntimeDepsImpl.cmake")

    file(CONFIGURE OUTPUT "${_SCRIPT_FILE}"
        CONTENT [[
        cmake_policy(SET CMP0207 NEW)
        message(STATUS "Installing runtime DLLs to @_DEST@...")

        # 1. Resolve ALL dependencies recursively in one call
        file(GET_RUNTIME_DEPENDENCIES
            LIBRARIES @_RUNTIME_DLLS@
            RESOLVED_DEPENDENCIES_VAR _all_deps
            UNRESOLVED_DEPENDENCIES_VAR _unres
            POST_EXCLUDE_REGEXES
                # Exclude Windows System Directories (Case Insensitiveish)
                ".*[/\\\\](System32|SysWOW64)[/\\\\].*"
                ".*[/\\\\]Windows[/\\\\].*"
                # Exclude VC Redist (usually in System32 or WinSxS, but good to be explicit)
                ".*[/\\\\]WinSxS[/\\\\].*"
        )

        # 2. Install the filtered list
        set(_INSTALLED_COUNT 0)
        foreach(_DLL IN LISTS _all_deps)
            if(EXISTS "${_DLL}")
                get_filename_component(_NAME "${_DLL}" NAME)
                file(COPY "${_DLL}" DESTINATION "@_DEST@"
                     FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                      GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
                message(STATUS "  Installed: ${_NAME}")
                math(EXPR _INSTALLED_COUNT "${_INSTALLED_COUNT} + 1")
            else()
                message(WARNING "  Skipped (not found): ${_DLL}")
            endif()
        endforeach()

        # 3. Report Unresolved
        list(LENGTH _unres _unres_len)
        if(_unres_len GREATER 0)
            message(WARNING "  Unresolved dependencies (@_unres_len@):")
            foreach(_u IN LISTS _unres)
                message(WARNING "    ${_u}")
            endforeach()
        endif()

        message(STATUS "Runtime DLL installation complete. (${_INSTALLED_COUNT} files)")
        ]]
        @ONLY
    )

    install(SCRIPT "${_SCRIPT_FILE}")
endfunction()
