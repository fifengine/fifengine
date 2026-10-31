# Window sizing and DPI

FIFE distinguishes **points** from **pixels**:

* A point is a logical, DPI-independent unit. `Window::getWidthInPoints()` and
  `Window::getHeightInPoints()` return the window size in points.
* A pixel is a physical framebuffer unit. `Window::getWidthInPixels()` and
  `Window::getHeightInPixels()` return the drawable size (points ×
  `Window::getDPIScaleFactor()`).
* Window positions (`WindowSettings::x/y`) and display bounds use SDL screen
  coordinates: pixels on Windows, points on macOS.

Game UI and camera viewports normally use points. Rendering code that needs
the drawable size uses pixels.

On Windows, whether the process is DPI-aware at all is decided by the host
executable's manifest. Without one, SDL3 opts the process into PerMonitorV2
when the video subsystem initializes, so a plain `python.exe` game is covered;
frozen games and custom launchers need the manifest embedded in their `.exe`.
See [windows-hidpi.md](windows-hidpi.md).

## Requested vs effective size

`EngineSettings::setScreenWidth()` / `setScreenHeight()` set the **requested**
window size. `Engine::init()` clamps it to the target display; the window
manager may also shrink it further.

After `Engine::init()`:

| Getter | Meaning |
| --- | --- |
| `EngineSettings::getScreenWidth()` / `getScreenHeight()` | Effective size of the live window, in points. Kept in sync with resizes, fullscreen switches and mode changes. |
| `EngineSettings::getRequestedScreenWidth()` / `getRequestedScreenHeight()` | The size the application asked for, before clamping. Never overwritten by the engine. |
| `Window::getWidthInPoints()` / `getHeightInPoints()` | The live window size; the preferred API for viewport sizing. |
| `Window::getWidthInPixels()` / `getHeightInPixels()` | The drawable size for rendering. |

Settings dialogs should display the *requested* values so the user's choice is
preserved. Runtime viewport code should use the *effective* values (or,
preferably, the `Window` getters) so it matches what is actually on screen.

## Display scale and GUI scaling

`Window` exposes two related but distinct values:

| Getter | Meaning |
| --- | --- |
| `Window::getDPIScaleFactor()` | Pixel/point ratio for the drawable. The renderer and font rasterization already apply it. 1.0 on Windows; 2.0 on a macOS Retina display. |
| `Window::getDisplayScale()` | The operating system's UI/content scale for the window's display. 1.0 = 100 %, 1.25 = 125 %, 1.5 = 150 %, 2.0 = 200 %. |

On Windows both `SDL_GetWindowSize()` and `SDL_GetWindowSizeInPixels()` report physical
pixels, so the ratio stays 1.0 while `getDisplayScale()` carries the DPI percentage. On
macOS the window size is already in points and the ratio is 2.0, so applying the content
scale directly would scale fonts twice.

The GUI scale is therefore:

```text
guiScale = clamp(displayScale / dpiScaleFactor, 1.0, 8.0)   (or 1.0 when disabled)
```

`Engine::applyGUIScale()` computes this and calls `IGUIManager::setDPIScale()`. It runs when
a GUI manager is installed, at startup after the window is shown, and whenever the
pixel/point ratio or the display scale changes. `Window` polls the display scale in
`updateDPIScaleIfNeeded()` (called from `Engine::pump()`) and, when the drawable size
changed, in `recalculateDPIScale()`; both emit `signalDisplayScaleChanged`, so moving a
window between monitors at different DPI percentages updates the GUI without a restart.

A change of the pixel/point ratio also calls `FontManager::setDPIScaleForAll()` so
renderer fonts are rasterized for the drawable size.

`FifechanManager::setDPIScale()` / `getDPIScale()` implement the hook. TrueType fonts are
re-rasterized in place at the scaled size, including fonts that already exist. Bitmap/image
fonts keep their native pixel size and log a warning. The top container and mouse
coordinates stay in window point coordinates; the GUI is not globally scaled.

Applications using `IEngineChangeListener` receive `onDisplayScaleChanged(scale)` for the
content scale and `onDPIScaleChanged(scale)` for the pixel/point ratio.
`BasicApplication` polls both and exposes `_onDisplayScaleChanged()` /
`_onDPIScaleChanged()` for subclasses.

Scope limit: this FifeChan version has no global widget scale. Fonts created through
`FifechanManager` scale, but widget widths, paddings and layout must be scaled by the
application (pychan/Unknown Horizons). Use `getDisplayScale()` there when laying out the
HUD.

Applications that implement their own HUD scaling can disable the engine behaviour:

```python
engine.getSettings().setGUIScalingEnabled(False)
```

Set this before `Engine.init()`. Changing it later takes effect the next time the
engine applies a GUI scale (GUI-manager install, DPI or display-scale change). This
does not change the window pixel/point ratio used by the renderer.

## Display bounds

`DeviceCaps::getDisplayBounds()` and `Display::getDisplayBounds()` return the
full monitor rectangle. `Display::getUsableBounds()` returns the desktop work
area, excluding taskbars and docks.

Windowed startup clamps the requested size to the **usable** bounds, so a
window never starts underneath a taskbar. Fullscreen uses the full display
bounds.

## Mode changes and the render backend

`Engine::toggleFullscreen()` and `Engine::changeWindowMode()` apply the mode to
the live `SDL_Window`; they do not recreate it. Changing the monitor or the
OpenGL renderer still requires a restart and is reported through
`Engine::windowSettingsRequireRestart()`.

An application that calls `Window::create()` directly replaces the underlying
`SDL_Window`. Before the old window is destroyed, `Window` invokes its
`signalWindowDestroyed` hook; the engine uses it to invalidate cached images
while the renderer and textures are still valid. On the next
`createMainScreen()` call the SDL backend detects the new handle and rebuilds
its renderer and surface. The OpenGL backend recreates its context, re-runs
GLEW initialization and resets the cached GL state.
