# Tracy Profiler Module

Optional CPU, OpenGL GPU and named-pool memory profiling for FIFE, coordinated
by `DiagnosticsManager` in `src/fife/debug/`. The module is independent of the
RenderDoc module: enabling Tracy never starts a RenderDoc capture, and RenderDoc
never starts Tracy.

## Build configuration

Tracy is supplied by vcpkg (`tracy` 0.14.1 at this repository's baseline, with
the `on-demand` feature). The dependency is installed from `vcpkg.json`, but the
FIFE module is compiled only when requested:

```bash
cmake --preset clang22-x64-linux-dbg-cov -DENABLE_TRACY=ON
cmake --build out/build/clang22-x64-linux-dbg-cov -j8
```

`ENABLE_TRACY` defaults to `OFF`. When ON, CMake finds the `Tracy` package,
links `Tracy::TracyClient`, and defines `FIFE_ENABLE_TRACY=1` and
`TRACY_ENABLE=1`. When OFF, no Tracy header is included, no Tracy symbol is
linked, and all `FIFE_PROFILE_*` macros expand to empty statements.

Tracy's `on-demand` feature is used: zones are collected only while a viewer is
connected. `IsProfilingEnabled()` reports the compiled-in/configured state;
`TracyProfiler::IsProfilerConnected()` reports an actual viewer connection.

## Enabling at runtime

```cpp
engine.getSettings().setTracyEnabled(true);
engine.getSettings().setTracyGPUProfiling(true);   // needs a glcore backend
engine.getSettings().setTracyMemoryProfiling(true);
engine.init();
```

`DiagnosticsManager` is owned by `Engine`, which calls `Initialize()` after the
OpenGL context is current and `Shutdown()` before the window and render backend
are destroyed. Hosts can reach the independent facades with
`Engine::getRenderDocDebugger()` and `DiagnosticsManager::GetProfiler()`.

## CPU profiling

- `FIFE_PROFILE_ZONE()` / `FIFE_PROFILE_ZONE_N(name)` wrap an entire scope and
  record source file, line and function through Tracy's `ZoneScoped` macros.
- Instrumented phases use the shared constants from `diagnosticsphases.h`:
  Frame, Input, Target Pass, Movie Pass, Offscreen Pass, World Pass, UI Pass
  and Cursor Pass, plus Resource Load in image loading.
- `FrameMark` is emitted once per `Engine::pump()` through
  `TracyProfiler::MarkFrame()` (named `MarkFrame` because `FrameMark` is a macro).

## GPU profiling (OpenGL)

`TracyGPUProfiler` uses the official `tracy/TracyOpenGL.hpp` backend:

- `TracyGpuContext` is created once, only when a current context exists and the
  driver advertises `GL_ARB_timer_query` (desktop) or
  `GL_EXT_disjoint_timer_query` (ES) with `GL_QUERY_COUNTER_BITS > 0`. This
  guard is required because Tracy itself asserts on unsupported drivers.
- `TracyGpuZone` is used through the RAII `TracyGPUPhase` object, so a zone
  spans the real phase in `Engine::pump()` rather than a function call.
- `TracyGpuCollect` is called once after presentation in
  `DiagnosticsManager::OnFrameEnd()`.

Only glad-based glcore backends (`OpenGL_CORE`, `OpenGL_43`, `GLES2`, `GLES_31`)
support GPU zones. The legacy GLEW `OpenGL` backend, SDL and dummy backends
report no GPU support and are never touched. Diagnostics are initialized after
the first context is current; a later context recreation disables GPU profiling
for that process (documented limitation).

### Calibration and synchronization

By default the OpenGL GPU context is uncalibrated: CPU and GPU clocks are
aligned once at context creation and can drift over long sessions. Defining
`TRACY_OPENGL_AUTO_CALIBRATION` enables periodic recalibration, but Tracy then
calls `glGetInteger64v(GL_TIMESTAMP)` about once per second, which forces a
CPU/GPU synchronization (a pipeline stall). FIFE never defines this macro; opt
in only when long-session alignment is worth the stall. `TracyGpuCollect` also
has a small cost (a few microseconds) and is called once per frame.

No `glFinish`, `glReadPixels` or other blocking synchronization is added by
FIFE. RenderDoc capture timings must not be used as performance benchmarks; use
Tracy GPU zones instead.

## Memory profiling

FIFE has no custom allocator or global `operator new` override. Instead:

- `TracyMemoryProfiler::TrackAllocation/TrackFree` map to `TracyAllocN` /
  `TracyFreeN` for named pools, and are a no-op when disabled.
- `TrackedAllocation` reports one known buffer with RAII semantics. It is used
  for the VFS image buffer in `ImageLoader`, and is available to hosts for
  their own pools.
- The existing memory management system is unchanged.

## Platform and driver limitations

- OpenGL GPU profiling is unsupported on macOS/iOS (Tracy disabled it upstream).
- Nvidia drivers give inconsistent GPU timings when two GL contexts are active.
- AMD drivers on Linux may reset timestamp registers on power-state changes;
  consult the Tracy manual for driver-specific workarounds.
- Timer queries with zero counter bits are rejected before `TracyGpuContext`
  is invoked.

## Testing

`tests/core_tests/test_diagnostics_manager.cpp` (`[diagnostics]` tag) covers
configuration defaults and round-trips, init/shutdown idempotence, unavailable
profiler, invalid GL context, memory hooks and phase-name consistency. Tests run
without a Tracy viewer, RenderDoc installation, display or GL context.

## Manual validation

1. Configure with `-DENABLE_TRACY=ON`, build, and launch the engine.
2. Start the Tracy profiler UI and connect to the running engine.
3. Confirm the timeline shows `Frame`, `Input`, phase zones and frame marks.
4. Enable a glcore backend; confirm a GPU context named "FIFE OpenGL" appears
   with GPU zones matching the CPU/RenderDoc phase names.
5. Load an image through the VFS fallback and confirm the "VFS image buffer"
   named allocation appears in the memory view.
6. Verify normal engine behaviour with `setTracyEnabled(false)` and with a
   build using `ENABLE_TRACY=OFF`.
