#------------------------------------------------------------------------------
# InstallRuntimeDeps.cmake
# Copies runtime DLLs to CMAKE_INSTALL_LIBDIR during install phase
# Uses GET_RUNTIME_DEPENDENCIES_INFO for transitive dependency collection
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
# Internal: Add DLL to collection (skips debug DLLs)
#------------------------------------------------------------------------------
function(_collect_dll DLL_PATH)
    if(NOT EXISTS "${DLL_PATH}")
        return()
    endif()
    get_filename_component(DLL_NAME "${DLL_PATH}" NAME)
    if(DLL_NAME MATCHES "-d\\.dll$")
        return()
    endif()
    list(FIND _RUNTIME_DLLS "${DLL_PATH}" _INDEX)
    if(_INDEX EQUAL -1)
        list(APPEND _RUNTIME_DLLS "${DLL_PATH}")
        set(_RUNTIME_DLLS "${_RUNTIME_DLLS}" CACHE INTERNAL "")
    endif()
endfunction()

#------------------------------------------------------------------------------
# install_fifechan_dlls
# Collects FifeChan DLLs from the dependencies build directory
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

    foreach(_SUBDIR lib bin)
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
#------------------------------------------------------------------------------
function(install_vcpkg_dlls)
    if(NOT WIN32)
        return()
    endif()

    if(NOT DEFINED VCPKG_INSTALLED_DIR OR NOT DEFINED VCPKG_TARGET_TRIPLET)
        message(VERBOSE "VCPKG_INSTALLED_DIR or VCPKG_TARGET_TRIPLET not set, skipping vcpkg DLLs")
        return()
    endif()

    set(_VCPKG_BIN_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
    if(NOT EXISTS "${_VCPKG_BIN_DIR}")
        message(WARNING "vcpkg bin directory not found: ${_VCPKG_BIN_DIR}")
        return()
    endif()

    file(GLOB _ALL_DLLS "${_VCPKG_BIN_DIR}/*.dll")
    foreach(_DLL IN LISTS _ALL_DLLS)
        _collect_dll("${_DLL}")
    endforeach()
endfunction()

#------------------------------------------------------------------------------
# install_runtime_dlls
# Installs all collected DLLs and their transitive dependencies to CMAKE_INSTALL_LIBDIR
#------------------------------------------------------------------------------
function(install_runtime_dlls)
    if(NOT WIN32 OR _RUNTIME_DLLS STREQUAL "")
        return()
    endif()

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
        message(STATUS \"Installing runtime DLLs to \\\${CMAKE_INSTALL_LIBDIR}...\")
        foreach(_DLL ${_PROCESSED})
            get_filename_component(_NAME \"\${_DLL}\" NAME)
            if(_NAME MATCHES \"-d\\.dll\\\$\")
                message(STATUS \"  Skipping debug DLL: \${_NAME}\")
                continue()
            endif()
            file(COPY \"\${_DLL}\" DESTINATION \"\${CMAKE_INSTALL_LIBDIR}\"
                 FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                              GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
            message(STATUS \"  Installed: \${_NAME}\")
        endforeach()
        message(STATUS \"Runtime DLL installation complete.\")
    ")
endfunction()
