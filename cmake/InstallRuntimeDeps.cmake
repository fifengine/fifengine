#------------------------------------------------------------------------------
# InstallRuntimeDeps.cmake
# Copies runtime DLLs to CMAKE_INSTALL_LIBDIR during install phase
# Uses GET_RUNTIME_DEPENDENCIES_INFO for transitive dependency collection
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
# Searches both release and debug directories
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

    # Search in: lib/, bin/, lib/debug/, bin/debug/
    foreach(_SUBDIR lib bin lib/debug bin/debug)
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
# Searches both release (bin/) and debug (debug/bin/) directories
#------------------------------------------------------------------------------
function(install_vcpkg_dlls)
    if(NOT WIN32)
        return()
    endif()

    if(NOT DEFINED VCPKG_INSTALLED_DIR OR NOT DEFINED VCPKG_TARGET_TRIPLET)
        message(VERBOSE "VCPKG_INSTALLED_DIR or VCPKG_TARGET_TRIPLET not set, skipping vcpkg DLLs")
        return()
    endif()

    # Search both release and debug directories
    foreach(_SUBDIR "bin" "debug/bin")
        set(_VCPKG_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/${_SUBDIR}")
        if(EXISTS "${_VCPKG_DIR}")
            file(GLOB _DLL_FILES "${_VCPKG_DIR}/*.dll")
            foreach(_DLL IN LISTS _DLL_FILES)
                _collect_dll("${_DLL}")
            endforeach()
        endif()
    endforeach()
endfunction()

#------------------------------------------------------------------------------
# install_runtime_dlls
# Installs all collected DLLs and their transitive dependencies
#------------------------------------------------------------------------------
function(install_runtime_dlls)
    if(NOT WIN32 OR _RUNTIME_DLLS STREQUAL "")
        return()
    endif()

    _get_dll_destination(_DEST)

    set(_PROCESSED "")
    set(_TO_PROCESS "${_RUNTIME_DLLS}")

    while(_TO_PROCESS)
        list(POP_FRONT _TO_PROCESS _DLL)
        list(FIND _PROCESSED "${_DLL}" _IDX)
        if(NOT _IDX EQUAL -1)
            continue()
        endif()
        list(APPEND _PROCESSED "${_DLL}")

        file(GET_RUNTIME_DEPENDENCIES_INFO
            OUTPUT_VARIABLE _OUT
            FILES "${_DLL}"
            CONTEXT_TYPE RESOLVED_DLL
            CONTEXT_FILE "${_DLL}"
        )

        string(REGEX MATCHALL "[A-Za-z]:[^ \"]+\\.dll" _TRANS "${_OUT}")
        foreach(_T IN LISTS _TRANS)
            list(FIND _PROCESSED "${_T}" _I)
            if(_I EQUAL -1)
                list(FIND _TO_PROCESS "${_T}" _I2)
                if(_I2 EQUAL -1)
                    list(APPEND _TO_PROCESS "${_T}")
                endif()
            endif()
        endforeach()
    endwhile()

    install(CODE "
        message(STATUS \"Installing runtime DLLs to \\\${_DEST}...\")
        foreach(_DLL ${_PROCESSED})
            get_filename_component(_NAME \"\${_DLL}\" NAME)
            file(COPY \"\${_DLL}\" DESTINATION \"\${_DEST}\"
                 FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                              GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
            message(STATUS \"  Installed: \${_NAME}\")
        endforeach()
        message(STATUS \"Runtime DLL installation complete.\")
    ")
endfunction()
