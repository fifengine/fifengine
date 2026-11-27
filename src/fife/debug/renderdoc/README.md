# RenderDoc Debugger Module

An optional, self-contained debugging module for FIFE. It dynamically loads the
RenderDoc in-application API at runtime, controls frame captures, emits OpenGL
debug markers and labels GL resources. It does **not** link against RenderDoc,
create an OpenGL context or change the renderer. With RenderDoc absent (or the
module disabled) FIFE behaves exactly as before.

## Architecture

| File | Responsibility |
| --- | --- |
| `renderdoc_app.h` | Official RenderDoc v1.x header, vendored verbatim (MIT license, RenderDoc contributors). Newest API struct: `RENDERDOC_API_1_7_0`. |
| `renderdocapi.h/.cpp` | Locates the resident RenderDoc module, resolves `RENDERDOC_GetAPI`, requests the highest supported API version and stores the table. Injectable `IRenderDocLibraryLoader` seam for tests. |
| `renderdocconfig.h` | Plain settings struct (`enableRenderDoc`, `autoCapture`, `captureOnHotkey`, `captureHotkey`, `captureDirectory`, `enableDebugMarkers`, `enableResourceNaming`, `logCaptureEvents`). Defaults are all safe. |
| `renderdoccapture.h/.cpp` | Capture state machine: `TriggerCapture`, `StartFrameCapture`, `EndFrameCapture`, `IsFrameCapturing`, path template and last capture path. |
| `renderdocmarkers.h/.cpp` | KHR_debug event regions, one-shot markers and `glObjectLabel` resource names. Injectable GL function table for tests. |
| `renderdocdebugger.h/.cpp` | Public facade combining the above; owned by `DiagnosticsManager`, reachable through `Engine`. |

The only engine changes are: one member in `Engine`, a few lines in
`Engine::init()` (create/configure), `Engine::pump()` (hotkey, frame and 2D phase
markers) and `Engine::destroy()` (release while the GL context is still alive).
No renderer, shader or GL state code was modified.

## Requirements and installation

- RenderDoc 1.x (any version exposing the in-app API; API 1.0.0 through 1.7.0
  are accepted with a fallback chain). Download it from <https://renderdoc.org/>.
- A backend with an OpenGL context (`OpenGL`, `OpenGL_CORE`, `GLES2`,
  `OpenGL_43`, `GLES_31`). SDL and dummy backends are supported but produce no
  GL markers or labels; capture still works through RenderDoc's own hooks.
- No build-time dependency: the module compiles into `fife_lib` unconditionally
  and requires no additional libraries or include paths.

## Enabling RenderDoc

The module is disabled by default. Set the following on `Engine::getSettings()`
in C++, or through the matching Python properties:

```cpp
FIFE::Engine engine;
engine.getSettings().setRenderBackend("OpenGL_CORE");
engine.getSettings().setRenderDocEnabled(true);
engine.getSettings().setRenderDocHotkeyCapture(true);
engine.getSettings().setRenderDocCaptureHotkey("F12");   // default
engine.getSettings().setRenderDocCaptureDirectory("captures"); // optional
engine.getSettings().setRenderDocAutoCapture(false);     // first frame only
engine.getSettings().setRenderDocDebugMarkers(true);
engine.getSettings().setRenderDocResourceNaming(true);
engine.getSettings().setRenderDocLogCaptureEvents(true);
engine.init();
```

When the config is disabled, `initialize()` performs no API lookup at all —
there is no measurable cost beyond one boolean check per frame.

## Launching with RenderDoc

RenderDoc must be present in the process **before** the engine is initialized;
the documented mechanism detects an already-loaded module and does not inject
one. Use RenderDoc UI → *Launch Application* with the engine executable, or
*File → Attach to Running Instance* and enable graphics API hooks before the
GL context is created. The engine logs one line at startup:

```
RenderDoc unavailable: no resident RenderDoc module detected
```

or, on success:

```
RenderDoc detected; in-app API initialized (reported version 1.7.0)
```

## Triggering captures

- Press the configured hotkey (`F12` by default) — implemented through FIFE's
  `EventManager`, with rising-edge detection identical to the F11/Ctrl-V
  shortcuts. RenderDoc's own hotkey (usually F12) may also work, but is
  controlled by RenderDoc, not by FIFE.
- Enable `AutoCapture` to request a capture of the first pumped frame.
- From C++:

```cpp
#include "debug/renderdoc/renderdocdebugger.h"

auto* debugger = engine.getRenderDocDebugger();
debugger->triggerCapture();          // next presented frame
debugger->startCapture();            // explicit region
// ... render ...
if (debugger->endCapture()) {        // only true when the API reports success
    auto path = debugger->getLastCapturePath();
}
```

`Engine::getRenderDocDebugger()` returns a non-null facade even when RenderDoc
is unavailable; every call is then a safe no-op. Resource naming helpers
(`nameTexture`, `nameFramebuffer`, `nameBuffer`, `nameShader`, `nameProgram`,
`nameRenderbuffer`, `nameVertexArray`) are available on the same facade and must
be called with a current GL context.

`SetCaptureFilePathTemplate` takes a **template**, not a final filename:
`captures/fife_capture` produces `captures/fife_capture_frame123.rdc`. Nested
explicit captures are refused. An `EndFrameCapture` failure is logged as an
error and never recorded as a successful capture.

## Inspecting captures

1. In RenderDoc UI, open the captured `.rdc` file from the configured directory
   (or the UI's default capture folder when no directory is configured).
2. The Event Browser shows nested regions such as `Frame`, `Target Pass`,
   `World Pass`, `UI Pass`, `Cursor Pass` and `Movie Pass`.
3. Bound textures, framebuffers, buffers, shaders, programs, renderbuffers and
   VAOs that were labelled through the facade appear with their names in the
   pipeline view and Resource Inspector. Names must be set before capture;
   RenderDoc does not support a resource changing name within a capture.
4. Optional rich object metadata can be added by hosts using the 1.7.0
   `SetObjectAnnotation` API directly through `renderdocapi::api()`.

## OpenGL marker mechanism

The RenderDoc in-application API deliberately has **no** marker functions.
RenderDoc's own annotation documentation specifies API-native debug markers;
for OpenGL this is `GL_KHR_debug`:

| Facade call | OpenGL call |
| --- | --- |
| `BeginEvent(name)` | `glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name)` |
| `EndEvent()` | `glPopDebugGroup()` |
| `SetMarker(name)` | `glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, name)` |
| `nameTexture/nameFramebuffer/...` | `glObjectLabel(<object enum>, name, -1, label)` |

Entry points are resolved once through FIFE's vendored glad loader. If
`GL_KHR_debug` is unavailable, the calls become no-ops and one warning is
logged. No GL function is ever called without a valid context; the engine sets
context validity after `createMainScreen()` and clears it at shutdown.

FIFE is a 2D engine: it has no shadow, geometry, lighting, bloom or tonemapping
pass. Constants for those names are provided by `RenderDocPass` for games that
own such passes, but the engine only emits markers around real phases (frame,
offscreen render, target render, world update, GUI, cursor, movie).

## API loading and platform differences

| Platform | Detection | Symbol | Notes |
| --- | --- | --- | --- |
| Windows | `GetModuleHandleA("renderdoc.dll")` | `GetProcAddress` | Module is never freed; `RemoveHooks()` is intentionally not called (undefined after API work). |
| Linux | `dlopen("librenderdoc.so", RTLD_NOW \| RTLD_NOLOAD)` | `dlsym` | `RTLD_NOLOAD` avoids loading RenderDoc; the handle is intentionally not `dlclose`d. |

Versions are requested newest-first (1.7.0, 1.6.0, 1.4.0, 1.1.2, 1.0.0) and the
actual version is read with `GetAPIVersion()`. `RENDERDOC_GetAPI` is called at
most once per requested version, under a mutex, as required by the header.

For OpenGL, RenderDoc expects the device pointer to be the native context
(HGLRC/GLXContext/EGLContext). SDL's `SDL_GLContext` is opaque, so the module
uses the documented `NULL` wildcard for the device and pins only the native
window handle from SDL properties (HWND, X11 Window or Wayland surface). With
one GL context this is unambiguous; with multiple contexts RenderDoc may pick
an arbitrary match. This is a documented limitation, not a crash path.

## Known limitations

- Windows and Linux only; macOS and Android are not supported in this iteration.
- The legacy `OpenGL` backend (GLEW) and non-GL backends do not emit GL debug
  markers/labels because they do not load the glad KHR_debug entry points.
- Multi-context/multi-window capture relies on RenderDoc's wildcard semantics.
- The Python API exposes the settings, not capture control methods.
- `RenderDocPass` names for 3D passes are constants only — no synthetic passes
  are inserted into the 2D renderer.

## Troubleshooting

| Symptom | Cause / action |
| --- | --- |
| "no resident RenderDoc module detected" | Engine was launched normally, not through RenderDoc UI or with hooks injected. Relaunch from RenderDoc. |
| "RENDERDOC_GetAPI was not found" | Incompatible/very old RenderDoc build; upgrade RenderDoc. |
| "in-app API version unsupported" | RenderDoc older than 1.0.0; upgrade RenderDoc. |
| No markers in Event Browser | Backend is SDL/dummy, `GL_KHR_debug` unsupported, or `EnableDebugMarkers` is false. |
| No resource names | `EnableResourceNaming` is false, or names were applied after capture started. |
| Capture failed / empty path | Disk error or `EndFrameCapture` returned 0; check the log and capture directory. |
| Capture contains wrong window | Multiple GL contexts; use one window/context or annotate targets manually in RenderDoc UI. |

## Manual validation with a real RenderDoc UI

1. Build the engine with an OpenGL backend and set the settings above.
2. Launch the executable from RenderDoc UI (*Launch Application*).
3. Confirm the startup log reports API 1.x and no warnings.
4. Press `F12` (or the configured key); the UI should show a new capture.
5. Open it, verify the `Frame` and phase markers exist in the Event Browser.
6. Inspect a bound texture/framebuffer and confirm its label is visible.
7. Verify captures land in `CaptureDirectory` with `_frameNNN.rdc` names.
8. Quit and restart the engine without RenderDoc; verify normal operation and
   the single "unavailable" log line.

Automated tests (`[renderdoc]` tag in `tests/core_tests/test_renderdoc_debugger.cpp`)
use injected fake loaders, API tables and GL function tables, so they run in CI
without RenderDoc, a display or a GL context.
