# Windows: DPI awareness and the host process

FIFE renders in points and pixels and exposes the display scale, but **DPI
awareness itself is a property of the process**, not of the engine library.
Windows decides at process start whether the process is DPI-unaware,
system-aware, per-monitor-aware, or per-monitor-v2-aware.

## The manifest belongs to the `.exe`

A manifest embedded in a DLL is ignored for process DPI awareness. That means:

- `libfifengine.dll` cannot make a process DPI-aware.
- `_fife_swig.pyd` (the SWIG extension) cannot make a process DPI-aware.
- Only the **host executable** can declare it, through its manifest.

On Windows builds the FIFE CMake project generates
`platform/windows_dpi_manifest.manifest` and installs it (via
`fife_install_windows_dpi_manifest()`) to:

```text
<prefix>/share/fife/platform/windows_dpi_manifest.manifest
```

The manifest requests `PerMonitorV2` (with the legacy `dpiAware=true`
fallback) and the Common Controls v6 dependency. The file is installed even if
no executable in the tree uses it, so downstream projects can embed it.

## Python games

A normal Python game is started as:

```bat
python run_uh.py
```

The process is `python.exe`. The official CPython `python.exe` manifest does
**not** declare DPI awareness. SDL3 handles this itself: when the Windows video
subsystem initializes, SDL calls `SetProcessDpiAwarenessContext` and defaults
to `permonitorv2`. FIFE initializes SDL video during `Engine.init()`, before it
creates the window, so a stock `python.exe` game is PerMonitorV2 without any
extra manifest.

FIFE also sets the SDL hint `SDL_WINDOWS_DPI_AWARENESS=permonitorv2` before
video init, unless the application already set it. Set the hint before
`Engine.init()` to override it (for example to `system` or `unaware`). A
manifest on the host `.exe` still wins over the hint.

The hint is not exposed as a macro in the SDL3 headers, but the SDL 3.4 Windows
backend still reads the literal name in `WIN_InitDPIAwareness()` and defaults
to `permonitorv2` when it is unset. FIFE's call therefore only states the
intent explicitly; SDL would pick the same mode anyway.

Awareness can only be set before the process owns any window (HWND). That
works for the normal case because SDL video init, inside `Engine.init()`, is
the first thing that creates one. A game that opens a Tk or Qt window before
`Engine.init()` can end up unaware.

Consequences:

- Do **not** ship the manifest next to `_fife_swig.pyd` and expect it to work.
- Do **not** copy it over `python.exe.manifest` in an installed Python; that
  modifies the interpreter installation and is not portable.
- Use `Window.getDisplayScale()` / `EngineSettings` and the GUI scaling hooks
  as described in [window-sizing.md](window-sizing.md).

### Frozen or packaged games

If the game is packaged with PyInstaller, Nuitka, cx_Freeze, or a custom C++
launcher, the packaged `.exe` is the host process. Its manifest controls DPI
awareness. Embed the FIFE manifest in that launcher:

- **PyInstaller**: pass the manifest through `--manifest <file>` (or the
  `EXE(..., manifest=...)` argument in the `.spec` file).
- **Other packagers**: use the equivalent "embed a manifest" option. If the
  packager cannot embed one, place the manifest next to the launcher as
  `<launcher>.exe.manifest`.
- **Custom C++ launcher**: use the CMake helper described below.

### Script fallback

If the host executable cannot be changed and SDL video init has not run yet, a
Python entry point can set awareness itself. It must run before
`Engine.init()` (more precisely: before any top-level window is created):

```python
import ctypes
import sys

if sys.platform == "win32":
    try:
        # DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        ctypes.windll.user32.SetProcessDpiAwarenessContext(ctypes.c_void_p(-4))
    except AttributeError:
        # Older Windows without SetProcessDpiAwarenessContext.
        pass
```

This is a fallback, not the preferred mechanism. `SetProcessDpiAwarenessContext`
returns 0 when the process already has DPI awareness (a manifest or an earlier
call), so the call is harmless if it is too late — the manifest still wins.

## CMake helper for executables

`CMake/FifeDpiManifest.cmake` provides:

```cmake
fife_embed_windows_dpi_manifest(my_game_exe)
```

- **MSVC**: merges the manifest into the linker-generated manifest, so it is
  embedded in the `.exe`.
- **MinGW/Clang**: copies it next to the executable as
  `my_game_exe.exe.manifest`, which Windows loads at process start.

Both functions are no-ops on non-Windows builds. The helper rejects
DLL/library targets with a clear error, because they cannot declare process
DPI awareness. In-tree targets get the function from FIFE's top-level
`CMakeLists.txt` (`include(FifeDpiManifest)`); other projects include the
module themselves or use the installed manifest file.

The FIFE core test executable uses it:

```cmake
add_executable(all_tests ...)
fife_embed_windows_dpi_manifest(all_tests)
```

## Verifying

After `Engine.init()`, FIFE logs one line with the values needed in bug
reports:

```text
Window geometry: 1280x720 points (1280x720 pixels), display scale 1.50, pixel/point ratio 1.00, GUI scale 1.50
```

- `display scale` is the OS content scale (`1.50` = 150 %).
- `pixel/point ratio` is what the renderer and font rasterizer already apply.
- On Windows the ratio is usually `1.00`; the display scale carries the DPI
  percentage.

The same values are available from Python:

```python
window = engine.getWindow()
print(window.getDisplayScale(), window.getDPIScaleFactor())
```

## Related

- [window-sizing.md](window-sizing.md) — points vs pixels, requested vs
  effective size, GUI scaling.
