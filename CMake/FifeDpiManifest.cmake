#
# FifeDpiManifest.cmake
#
# Windows DPI-awareness manifest support.
#
# DPI awareness is a property of the process, declared by the executable's
# manifest. A DLL (including the SWIG .pyd extension and libfifengine.dll)
# cannot declare process DPI awareness; only the host .exe can. This module
# therefore only attaches the manifest to executable targets.
#
# Usage:
#   fife_embed_windows_dpi_manifest(my_exe)
#   fife_install_windows_dpi_manifest()
#
# The generated manifest is also installed to
# <prefix>/share/fife/platform/windows_dpi_manifest.manifest so applications
# that build their own launcher can embed it.
#

include_guard(GLOBAL)

set(FIFE_WINDOWS_DPI_MANIFEST
    "${PROJECT_BINARY_DIR}/platform/windows_dpi_manifest.manifest"
    CACHE INTERNAL "Generated Windows DPI-awareness manifest")

#------------------------------------------------------------------------------
# _fife_configure_windows_dpi_manifest
# Generates the manifest once. Safe to call from any target function.
#------------------------------------------------------------------------------
function(_fife_configure_windows_dpi_manifest)
  if(NOT WIN32)
    return()
  endif()

  # configure_file() only rewrites the output when its content changes, so
  # calling this from several targets is cheap.
  configure_file(
    "${PROJECT_SOURCE_DIR}/CMake/platform/windows_dpi_manifest.manifest.in"
    "${FIFE_WINDOWS_DPI_MANIFEST}"
    @ONLY
  )
endfunction()

#------------------------------------------------------------------------------
# fife_embed_windows_dpi_manifest(<target>)
#
# Makes a Windows executable PerMonitorV2 DPI-aware.
#
# - MSVC: merges the manifest into the linker-generated manifest, so it is
#   embedded in the .exe.
# - MinGW/Clang: copies the manifest next to the executable as
#   <target>.exe.manifest, which Windows loads at process start.
#
# The target must be an executable. DLLs and Python extension modules cannot
# set process DPI awareness; the host .exe must carry the manifest.
#------------------------------------------------------------------------------
function(fife_embed_windows_dpi_manifest target)
  if(NOT WIN32)
    return()
  endif()

  if(NOT TARGET ${target})
    message(FATAL_ERROR "fife_embed_windows_dpi_manifest(): unknown target '${target}'")
  endif()

  get_target_property(target_type ${target} TYPE)
  if(NOT target_type STREQUAL "EXECUTABLE")
    message(FATAL_ERROR
      "fife_embed_windows_dpi_manifest() requires an executable target. "
      "'${target}' is of type '${target_type}'. DPI awareness is process-wide; "
      "a DLL or Python extension cannot declare it. Embed the manifest in the "
      "host .exe instead.")
  endif()

  _fife_configure_windows_dpi_manifest()

  if(MSVC)
    # link.exe already embeds a generated manifest by default. Merge the FIFE
    # manifest into it so the DPI settings end up in the executable.
    file(TO_NATIVE_PATH "${FIFE_WINDOWS_DPI_MANIFEST}" manifest_native)
    target_link_options(${target} PRIVATE
      "/MANIFESTINPUT:${manifest_native}")
  else()
    # MinGW/Clang do not embed a manifest. Windows reads the external
    # <exe>.manifest file next to the executable at process start.
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FIFE_WINDOWS_DPI_MANIFEST}"
        "$<TARGET_FILE:${target}>.manifest"
      COMMENT "Copying DPI manifest next to ${target}"
      VERBATIM
    )
  endif()
endfunction()

#------------------------------------------------------------------------------
# fife_install_windows_dpi_manifest()
#
# Installs the manifest for applications that build their own launcher.
#------------------------------------------------------------------------------
function(fife_install_windows_dpi_manifest)
  if(NOT WIN32)
    return()
  endif()

  _fife_configure_windows_dpi_manifest()

  install(
    FILES "${FIFE_WINDOWS_DPI_MANIFEST}"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/fife/platform"
  )
endfunction()
