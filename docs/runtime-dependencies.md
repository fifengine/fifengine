# FIFEngine Runtime Dependencies (Windows)

This document describes the runtime DLL dependencies for FIFEngine on Windows.

## Install Location

All DLLs are installed to `${CMAKE_INSTALL_PREFIX}/lib/` (i.e., `CMAKE_INSTALL_LIBDIR`)
to ensure co-location with the main library `fife.dll`.

This is required for Windows to find the DLLs at runtime without modifying the system PATH.

## Dependency Tree

```
fife.dll
|
+-- fifechan.dll
|   +-- fifechan_sdl.dll
|   |   +-- SDL2.dll
|   |       +-- (system DLLs: ddraw.dll, dsound.dll, winmm.dll, etc.)
|   |
|   +-- fifechan_opengl.dll
|       +-- glew32.dll
|           +-- opengl32.dll
|
+-- SDL2.dll (shared)
|
+-- SDL2_image.dll
|   +-- SDL2.dll
|
+-- SDL2_ttf.dll
|   +-- SDL2.dll
|   +-- freetype.dll (if dynamic)
|       +-- zlib.dll
|
+-- glew32.dll
|   +-- opengl32.dll
|
+-- libogg.dll
|
+-- libvorbis.dll
|   +-- libogg.dll
|
+-- libvorbisfile.dll
|   +-- libogg.dll
|   +-- libvorbis.dll
|
+-- libpng16.dll
|   +-- zlib.dll
|
+-- OpenAL32.dll
|   +-- (system DLLs)
|
+-- zlib.dll
|
+-- tinyxml2.dll
```

## Primary DLLs (vcpkg)

| DLL | Package | Description |
|-----|---------|-------------|
| SDL2.dll | sdl2 | Simple DirectMedia Layer 2 |
| SDL2_image.dll | sdl2-image | SDL image loading extension |
| SDL2_ttf.dll | sdl2-ttf | SDL TrueType font support |
| glew32.dll | glew | OpenGL Extension Wrangler Library |
| libogg.dll | libogg | Ogg Vorbis container format |
| libvorbis.dll | libvorbis | Ogg Vorbis audio codec |
| libvorbisfile.dll | libvorbis | Ogg Vorbis file streaming |
| libpng16.dll | libpng | PNG image format library |
| OpenAL32.dll | openal-soft | Open Audio Library |
| zlib.dll | zlib | Compression library |
| tinyxml2.dll | tinyxml2 | XML parsing library |

## Primary DLLs (FifeGUI/FifeChan)

| DLL | Description |
|-----|-------------|
| fifechan.dll | Core FifeChan GUI framework |
| fifechan_sdl.dll | SDL2 backend for FifeChan |
| fifechan_opengl.dll | OpenGL backend for FifeChan |

## Transitive Dependencies (System DLLs)

The build system uses CMake's `file(GET_RUNTIME_DEPENDENCIES_INFO)` to
automatically collect all transitive DLL dependencies.

The following Windows system DLLs are typically included:

| DLL | Description |
|-----|-------------|
| kernel32.dll | Windows NT BASE API Client DLL |
| user32.dll | Windows USER API Client DLL |
| gdi32.dll | Windows GDI Client DLL |
| msvcrt.dll | Microsoft Visual C Runtime |
| vcruntime.dll | Visual C++ Runtime Library |
| opengl32.dll | OpenGL Client DLL |
| ddraw.dll | DirectDraw |
| dsound.dll | DirectSound |
| winmm.dll | Windows Multimedia API |
| advapi32.dll | Advanced Windows API |
| shell32.dll | Windows Shell |
| ole32.dll | OLE 2.0 |
| oleaut32.dll | OLE Automation |
| comdlg32.dll | Common Dialog Box |
| comctl32.dll | Common Controls |
| ws2_32.dll | Windows Sockets 2 |

## Build System

The DLL installation is handled by `cmake/InstallRuntimeDeps.cmake`:

1. `install_fifechan_dlls()` - Collects FifeChan DLLs from `${DEPENDENCY_INSTALL_DIR}/lib/`
2. `install_vcpkg_dlls()` - Collects vcpkg DLLs from `${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/`
3. `install_runtime_dlls()` - Installs all collected DLLs and their transitive dependencies to `lib/`

## Troubleshooting

### "DLL not found" errors

Ensure all DLLs from the `lib/` directory are in your PATH or co-located with
the application executable.

### Missing transitive dependencies

The build system automatically collects transitive dependencies. If a DLL is
still missing, it may be a new transitive dependency added by a library update.

### Debug vs Release DLLs

Only Release DLLs are installed. Debug DLLs (matching `*-d.dll`) are skipped.
Ensure you are linking against Release builds of dependencies.

## See Also

- [cmake/InstallRuntimeDeps.cmake](../cmake/InstallRuntimeDeps.cmake) - DLL installation implementation
- [CMakeLists.txt](../CMakeLists.txt) - Main build configuration
