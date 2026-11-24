# Modern OpenGL Core / OpenGL ES 2.0 backend (Issues #793, #414 follow-up) — Design

## Intent and why

The existing OpenGL backend is a fixed-function (compatibility profile 3.0)
renderer: it drives the matrix stack, client-side vertex arrays, `GL_COMBINE`
texture environments and fixed-function lighting. That pipeline cannot run on
OpenGL ES 2.0 at all, and it cannot run in a core profile. Issue #793 has asked
for ES2 since 2013 (original motivation: ~2.5 FPS for Unknown Horizons on a
Raspberry Pi under the SDL renderer). The maintainers' own 2017 analysis says
the fix is a programmable-pipeline rewrite, and names #856 and #1014 as
prerequisites.

Both prerequisites are now complete: #856 landed as static index buffers and
`GL_TRIANGLES` in 2017 (visible today as `m_tIndices` / `m_tcIndices` /
`glDrawElements` in `renderbackendopengl.cpp`), and #1014 was closed in June
2026 via #1099 (the `Window` class and `getAttachedWindow()` machinery). The
rewrite is genuinely unblocked, and it is the foundation any future GPU
feature — including instanced particles — needs.

This is **Track 2** of a two-track split. Track 1 delivers a backend-agnostic,
CPU-simulated particle system for 0.5.0 without touching the render backends.
That track is deliberately *not* a prerequisite for this one, and this one is
deliberately *not* a gate on that one. Issue #793 was removed from the 0.5.0
milestone in April 2026; this work is a separate, later release track.

This document is also **Tier A of a three-tier graphics roadmap**:

- **Tier A (this doc)** — GL 3.3 core + ES 2.0. Widest reach: Raspberry Pi 1–5,
  older desktop hardware, WebGL 1, CI llvmpipe. Baseline shader architecture.
- **Tier B** (`2026-11-23-modern-gl-43-es31-design.md`) — GL 4.3 core + ES 3.1.
  Same `glcore/` foundation and backend classes, added as new profile configs
  and shader variants; unlocks compute shaders and GPU-side particles. Covers
  Pi 4/5 and modern desktop. Specified separately.
- **Tier C (long-term, out of scope)** — a Vulkan backend, sketched as a
  foundation document in `2026-11-23-vulkan-backend-design.md`. Vulkan is the
  actual forward path (Android's primary API, available on Pi 4/5 via `v3dv`,
  and on Apple via MoltenVK), but it is a genuinely separate backend family —
  pipelines, descriptor sets, command buffers and explicit synchronisation —
  and must not block Tier A or B.

Tier A therefore must keep its shader sources and context abstraction
structured so that new profile/preamble variants can be added in Tier B
without rewriting the batching or state code.

## Goals

- Add a shader-based render backend that runs in **OpenGL 3.3 core profile** on
  desktop and in **OpenGL ES 2.0** contexts (SDL3 `SDL_GL_CONTEXT_PROFILE_ES`),
  so ES2-capable platforms no longer fall back to the slow SDL renderer.
- Keep the existing SDL, legacy OpenGL, and dummy (Null/Surface) backends fully
  functional and selectable. No backend is removed or disabled by this change.
- Preserve the `RenderBackend` and `Image` public interfaces so existing view
  renderers keep working. The new backend implements the existing batching API
  (`addImageToArray()` / `renderVertexArrays()`) with VBOs and shaders instead
  of client-side arrays.
- Isolate all new GL code in a new backend family; do not scatter `if
  (modern)` branches through the legacy backend.
- Make Fifechan/GUI drawing work on the new backend without concrete-backend
  casts in production GUI code.
- Add build options and CI coverage for both desktop core and ES2 profiles.

## Non-goals

- Removing or rewriting the legacy `RenderBackendOpenGL` in this change.
- Compute shaders, transform feedback, or any GL 4.x-only feature.
- **Particle-specific code.** Track 1's particle renderer draws through the
  generic batching API and therefore works on this backend unchanged. An
  optional instanced-particle fast path is a later follow-on, not part of this
  design.
- Platform packaging for Android/iOS/Emscripten (APKs, Xcode projects, Emscripten
  ports). The goal is that an ES2 context can be created through SDL3 on those
  platforms; shipping those targets is separate.
- Byte-for-byte identical output compared with the legacy or SDL backends.
  Blending and filtering differences are acceptable; self-consistency is not.
- Auto-detection that silently replaces the default backend. Selection stays
  explicit through `EngineSettings`.

## Anti-goals

- Do not add a **pure** virtual to `RenderBackend`. There are now four concrete
  backend implementations (`RenderBackendSDL`, `RenderBackendOpenGL`,
  `RenderBackendNull`, `RenderBackendSurface` via `RenderBackendDummy`); a new
  pure virtual forces edits to all of them and to the headless test path.
  **Non-pure virtuals with safe defaults are explicitly allowed** for the
  batching hooks described below; that is the chosen mechanism for Z and
  overlay drawing.
- Do not make Track 1's particle system depend on this backend. It must keep
  working on SDL, legacy OpenGL, and the dummy backends.
- Do not touch `SdlGuiGraphics`, `OpenGLGuiGraphics`, or the legacy GLImage
  upload paths. New GUI and image implementations live beside them.
- Do not gate this work on #414. The two tracks ship independently.

## Constraints and findings from the codebase

These were verified while designing; the builder should not rediscover them.

1. **Context creation** is split between `RenderBackendOpenGL::init()`
   (`renderbackendopengl.cpp:226-261`) and `createMainScreen()`
   (`:270-395`). The only `SDL_GL_SetAttribute` calls are major/minor 3.0 and
   buffer sizes (`:249-259`); `SDL_GL_ACCELERATED_VISUAL` is set later at
   `:314`. There is **no** profile mask, context flags, or EGL setup anywhere
   in `src/`. Context creation/make-current/destruction is at `:277-332` and
   `:202-218`. `Window::toSDLFlags()` adds `SDL_WINDOW_OPENGL` only when
   `m_settings.opengl` is set (`window.cpp:649-655`), and `Engine::init()` sets
   that flag via `dynamic_cast<RenderBackendOpenGL*>`
   (`engine.cpp:589`).
2. **GLEW is the only GL loader** (`fife_opengl.h:13-15`), initialised once per
   fresh context at `renderbackendopengl.cpp:320-328` with `glewExperimental`.
   Other GL include sites are `fifechanmanager.cpp:18-21` and
   `opengl_gui_graphics.h/.cpp:13-19`. There is no `SDL_opengl.h`, `glad`, or
   `SDL_GL_GetProcAddress` anywhere in `src/`. GLEW feature gates appear at
   `renderbackendopengl.cpp:207, :3101` (FBO), `glimage.cpp:444, :620, :781`
   (NPOT), `:557` (compression), `:543-546` (anisotropy).
3. **Fixed-function hotspots** that need shader/VAO equivalents: matrix stack
   (`renderbackendopengl.cpp:361-365, :854-857, :3113-3117, :3163-3167,
   :3177-3203`), `glEnableClientState` and pointer setup (`:390-392, :530-564,
   :709-724, :748-779, :3180-3198`), alpha test (`:668-691`), lighting
   (`:509-523, :590-619`), stencil (`:621-661`), and three `GL_COMBINE` blocks
   in `prepareForOverlays()` (`:2754-2869`). `renderWithZTest()` still uses
   `GL_QUADS` at `:1534`, which is illegal in core profile and ES2.
   `lightrenderer.cpp` issues no GL calls; it drives stencil state indirectly
   through `changeRenderInfos()` with `GLConstants` values
   (`renderbackend.h:42-59`), so keeping that API intact protects it.
4. **GLImage is coupled to the concrete legacy backend**: `dynamic_cast` at
   `glimage.cpp:519, :855, :878` and in `render()`/`renderZ()` at `:361-365,
   :389-390, :420-421`. Mipmapping uses `GL_GENERATE_MIPMAP` (`:523-526`), and
   NPOT/compression/anisotropy use desktop GLEW extensions. Sized internal
   formats `GL_RGBA8`/`GL_RGB8` (`:556, :2769, :3096, :3148`) and the EXT-suffixed
   FBO entry points (`:3102-3103, :3142`) are not available in ES2.
5. **Fifechan is coupled by string and by cast**: `FifechanManager::init()`
   dispatches on `"SDL"` / `"OpenGL"` / `"DUMMY_*"` and `assert(0)`s otherwise
   (`fifechanmanager.cpp:259-282`); `resizeTopContainer()` casts to concrete
   graphics classes (`:210-218`). `OpenGLGuiGraphics` holds a
   `RenderBackendOpenGL*` obtained by `dynamic_cast` (`opengl_gui_graphics.cpp:41-46`).
   `SdlGuiGraphics` holds a base `RenderBackend*` and only casts for image
   drawing. The existing `DummyGuiGraphics` already demonstrates the pattern
   we want: implement generic `fcn::Graphics` by forwarding to
   `RenderBackend` primitives and `Image::render()`, with no concrete cast.
6. **Backend selection is a string branch**: possible names are `"SDL"`,
   `"OpenGL"`, `"DUMMY_NULL"`, `"DUMMY_SURFACE"` (`enginesettings.cpp:160-168`,
   default `"SDL"` at `:47`), and the factory is the if/else chain in
   `engine.cpp:393-423`. There are **four** downstream name-branch sites:
   - `instancerenderer.cpp:137, :142, :157, :162` — depth-buffer path and
     colour binding;
   - `layercache.cpp:196` — CPU sort versus reliance on GPU depth;
   - `camera.cpp:1023` — render-target clip rect is Y-flipped for everything
     that is not `"SDL"`;
   - `imageloader.cpp:133` — `"SDL"` gets `SDL_PIXELFORMAT_RGBA8888`, all
     others get `SDL_PIXELFORMAT_RGBA32` for `GL_RGBA`/`GL_UNSIGNED_BYTE`
     uploads.
   The dummy backends are *not* uniform across these sites (they take the
   OpenGL side of the camera and imageloader checks, but the non-OpenGL side
   of the layercache check), so a single capability flag cannot replace all
   four without changing headless behaviour.
7. **Build system**: `ENABLE_OPENGL` is the only GL option
   (`CMakeLists.txt:92-100, :162-164`). GL/GLEW sources are in the unconditional
   source lists (`:490-491, :522, :721-723, :759`), so GLEW is effectively
   always required today. GLEW discovery/link is at `:964-968, :985,
   :1018-1025, :1076-1090, :1493, :1576`; tests link it at
   `tests/core_tests/CMakeLists.txt:105-107`; `vcpkg.json:19` pins `glew`.
   CI images **already install** `libgles2-mesa-dev` and `libegl1-mesa-dev`
   (`.github/workflows/build-on-linux.yml:117-120`), and already have a GL job
   using Xvfb + llvmpipe + `MESA_GL_VERSION_OVERRIDE=3.3` (`:566-587`).
8. **Testing** uses one Catch2 `all_tests` binary (`tests/core_tests/`). OpenGL
   tests that cannot get a context wrap creation in `try/catch(SDLException)`
   and `SKIP` (`test_gui.cpp:99-103, :145-165`). Dummy tests are filtered by
   the `[dummy]` tag into `all_tests_dummy` (`tests/core_tests/CMakeLists.txt:146-155`).

## Approaches considered

### Option A — Rewrite `RenderBackendOpenGL` in place (rejected)

Convert the existing backend's internals to shaders/VAOs and add profile
selection, keeping one class.

- Trades off: no duplicate class, all renderers automatically use the new path.
- Rejected: no rollback path for users with old drivers; ES2 cannot be reached
  without dropping the fixed-function fallbacks that make the current backend
  work everywhere; every fix risks breaking the only working GL renderer; and
  it forces Track 1's particle renderer to be debugged against a moving target.

### Option B — New shader-based backend family beside the legacy one (chosen)

Add `RenderBackendGLCore` (desktop core) and `RenderBackendGLES2` (ES2),
sharing a new `src/fife/video/glcore/` foundation. Legacy GL, SDL and dummy
backends stay untouched and selectable.

- Trades off: some duplicated context/batching code and a second backend to
  maintain, but zero risk to existing users, a clean rollback (change one
  settings string), a natural home for ES2 constraints, and the same isolation
  pattern the project already accepted for the dummy backends.
- Chosen. This is the same architectural bet that made #632 cheap to add.

### Option C — One backend with runtime fixed-function/shader switching (rejected)

Detect capabilities at runtime and pick a shader or legacy path inside one
class.

- Trades off: single backend name, no duplication.
- Rejected: doubles the state machines inside one class, makes driver-bug
  failures hard to reproduce, and requires every renderer to be tested in both
  modes. The project's own 2017 note ("a lot has to change ... lights, overlays
  need review") argues for a separate, testable backend instead.

### Loader sub-decision

Keep GLEW for the legacy backend; use **vendored, generated glad** for the new
backend, initialised through `SDL_GL_GetProcAddress` (chosen). GLEW's
experimental mode and `GL_EXT`-suffixed entry points are a poor fit for core
and ES2, and GLEW cannot load ES2 entry points. A generated loader checked
into the tree keeps builds offline and reproducible. Hand-written function
pointers were rejected as error-prone; migrating GLEW across the whole project
was rejected as out of scope.

### GUI sub-decision

Add a `GLCoreGuiGraphics` modelled on `DummyGuiGraphics` — a generic
`fcn::Graphics` implementation forwarding to `RenderBackend` primitives and
`Image::render()` (chosen). Reuse/refactor `OpenGLGuiGraphics` was rejected
because it holds a concrete `RenderBackendOpenGL*` and would require touching
production GUI code used by the legacy backend.

## Design

### Architecture

```
                RenderBackend (abstract, DynamicSingleton)
       /          |            |              \
     SDL      OpenGL(legacy)  Dummy        GLCore family (new)
                              /  \          /            \
                          Null Surface   GLCore(3.3)   GLES2(2.0)
                                            \            /
                                   src/fife/video/glcore/ foundation
                                   context | loader | shaders | buffers | batching

   Fifechan:
     SdlGuiGraphics          -> RenderBackendSDL       (unchanged)
     OpenGLGuiGraphics       -> RenderBackendOpenGL    (unchanged)
     DummyGuiGraphics        -> generic primitives     (unchanged)
     GLCoreGuiGraphics       -> generic primitives     (new)
```

New files under `src/fife/video/glcore/`:

- `glcontext.h/.cpp` — context configuration, profile/flag attributes,
  make-current, swap, drawable-size query, capability queries.
- `glad/` — vendored generated loader (desktop core 3.3 and ES2 variants),
  loaded via `SDL_GL_GetProcAddress`.
- `shader.h/.cpp`, `shaderloader.h/.cpp` — program compilation, cached uniform
  locations, built-in shader sources.
- `buffer.h/.cpp`, `vertexarray.h/.cpp` — VBO/IBO/VAO wrappers.
- `renderbackendgl.h/.cpp` — shared implementation; the two concrete classes
  are thin profile/config specialisations.
- `glcoreimage.h/.cpp` — `Image` implementation owning a GL texture and four
  UVs.
- `glcore_gui_graphics.h/.cpp` — generic Fifechan adapter.

New GUI adapter under `src/fife/gui/fifechan/base/glcore/`. Modified existing
files: `EngineSettings`, `Engine::init()`, `FifechanManager`, `CMakeLists.txt`,
`renderbackend.h` (non-pure batching hooks only), and all four backend-name
branch sites: `instancerenderer.cpp`, `layercache.cpp`, `camera.cpp`,
`imageloader.cpp`.

### Context and profile selection

- `GLContextConfig` holds profile (core / ES2), major/minor version, debug flag,
  stencil/depth sizes, vsync, and MSAA.
- Desktop core: `SDL_GL_CONTEXT_MAJOR_VERSION=3`, `MINOR_VERSION=3`,
  `SDL_GL_CONTEXT_PROFILE_MASK = SDL_GL_CONTEXT_PROFILE_CORE`,
  `SDL_GL_CONTEXT_FLAGS = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG` on platforms
  where that is safe.
- ES2: major 2, minor 0, `SDL_GL_CONTEXT_PROFILE_ES`. EGL setup is left to
  SDL3; no direct EGL calls in FIFE.
- All attributes are set in the new backend only. The legacy attribute block
  stays untouched. `Window::toSDLFlags()` needs no change: the new backend
  also sets `m_settings.opengl` through `Engine::init()`, which is the existing
  way `SDL_WINDOW_OPENGL` gets requested.
- `createMainScreen()` and window-replacement handling follow the legacy
  backend's lifecycle (destroy + recreate GL objects on a new window), because
  `Window` can replace the underlying SDL window underneath a backend.

### Loader

- glad is generated for the two profiles and vendored under
  `src/fife/video/glcore/glad/`. The loader is initialised once per context
  with `SDL_GL_GetProcAddress`; a fresh context re-initialises it.
- The new backend must not include `fife_opengl.h` or `GL/glew.h`. Existing
  GLEW include sites remain as they are for the legacy backend.
- CMake gains a `glad` source group / include path but no new external package.

### Shaders and state

- Built-in programs: **sprite** (textured quad + per-vertex RGBA, alpha test via
  `discard`), **primitive** (coloured lines/triangles/circles), **overlay** (two
  textures plus the four existing overlay modes), and **light** (modulate
  colour, stencil supplied by fixed GL state). Programs are embedded as string
  literals with a version/precision preamble: GLSL 330 core for desktop,
  ESSL 100 for ES2. No runtime shader files and no SPIR-V in v1.
- A `GLCoreState` cache replaces `m_state`: current program, texture bindings
  per unit, blend function, depth/scissor/stencil state, viewport, projection
  and view matrices. The existing `RenderBackend` API (`changeBlending`,
  `changeRenderInfos`, `pushClipArea`, lighting setters) maps onto this cache,
  so `LightRenderer` and other callers are unchanged.
- `glOrtho` is replaced by an explicit projection matrix uniform computed from
  the same point/pixel sizing logic currently in `createMainScreen()` and
  `setScalingMode()`.
- `GL_QUADS` becomes indexed triangles through the shared quad index buffer —
  the #856 work already established this pattern in the legacy backend.

### Images, batching and render targets

- `GLCoreImage` derives from `Image`, reusing base SDL surface handling, and
  owns a GL texture plus four UVs (same corner layout as `GLImage`). Atlas
  sub-images use the existing `useSharedImage()` path; the builder must keep
  the half-pixel texel correction that `GLImage::generateGLSharedTexture()`
  performs (`glimage.cpp:776-799`).
- The new backend implements the inherited `addImageToArray()` and
  `renderVertexArrays()` virtuals: particles and every renderer that draws via
  `Image::render()` feed a dynamic VBO/IBO and flush in one draw per texture
  and blend mode. This is what keeps Track 1 backend-agnostic.
- The three extra batching hooks that legacy `GLImage` reaches by
  `dynamic_cast<RenderBackendOpenGL*>` (`glimage.cpp:364, :389, :420`) —
  two-texture, Z, and multitexture-plus-Z — are promoted to **non-pure
  virtuals on `RenderBackend` with safe defaults** that fall back to the
  single-texture path. `RenderBackendOpenGL` already declares them as
  `virtual` beyond the base (`renderbackendopengl.h:61-80`), so it overrides
  them with its existing implementations; SDL and dummy backends inherit the
  defaults. `GLCoreImage` calls them through the base pointer, so no
  concrete-backend cast is needed anywhere in the image layer.
- ES2 texture adaptations: clamp NPOT textures to `GL_CLAMP_TO_EDGE` with no
  mipmaps; use `GL_RGBA`/`GL_UNSIGNED_BYTE` instead of sized internal formats;
  replace `GL_GENERATE_MIPMAP` with `glGenerateMipmap` when available; treat
  anisotropy and compression as desktop-only and ignore the corresponding
  user settings on ES2 rather than failing.
- Render targets use core FBO entry points on desktop (`glGenFramebuffers`
  etc.); ES2 uses the core ES2 FBO API (available in ES2 proper, not an
  extension). The compressed-texture workaround in
  `attachRenderTarget()` is desktop-only.
- `captureScreen()` uses the same `SDL_Surface`/`Image::saveAsPng` path as the
  legacy backend, reading pixels through `glReadPixels`.

### Fifechan and engine wiring

- `GLCoreGuiGraphics` implements the 14 `fcn::Graphics` pure virtuals by
  forwarding to generic `RenderBackend` primitives; image drawing calls
  `Image::render()`. It holds a base `RenderBackend*`, never a concrete cast.
- `FifechanManager::init()` and `resizeTopContainer()` gain branches for the
  two new backend names, mirroring the dummy branches.
- `EngineSettings::getPossibleRenderBackends()` gains `"OpenGL_CORE"` and
  `"GLES2"` **only when the corresponding CMake option is ON**. `Engine::init()`
  gains factory branches under the same guards and sets
  `winSettings.opengl = true` for both. A stale config selecting a backend
  that was not compiled in logs a warning and falls back to SDL, mirroring
  the existing legacy-without-`HAVE_OPENGL` fallback (`engine.cpp:414-422`).
- **All four name-branch sites treat both new backends exactly like legacy
  OpenGL.** Concretely: `instancerenderer.cpp:137-162` takes the depth-buffer
  and non-SDL colour paths; `layercache.cpp:196` lets GPU depth substitute for
  CPU sorting when depth buffering is enabled; `camera.cpp:1023` uses the
  Y-flipped render-target rect; `imageloader.cpp:133` uses
  `SDL_PIXELFORMAT_RGBA32` so `GL_RGBA`/`GL_UNSIGNED_BYTE` uploads match.
  Add the two new names to the OpenGL side of each condition. Do **not**
  replace these with a single capability flag: the dummy backends currently
  take the OpenGL side at `camera.cpp` and `imageloader.cpp` but the other
  side at `layercache.cpp`, and a flag would silently change headless
  behaviour. A capability-based cleanup is a separate follow-on.
- The legacy backend keeps returning `"OpenGL"`; the new ones return their
  own names, so the change is invisible to existing configurations.

### Build system and CI

- New mutually exclusive CMake options `ENABLE_OPENGL_CORE` and
  `ENABLE_OPENGL_ES2` (both default OFF); `ENABLE_OPENGL` keeps controlling the
  legacy backend and `HAVE_OPENGL`.
- **v1 requires `ENABLE_OPENGL=ON` whenever either new option is ON.** CMake
  must enforce this. This keeps `HAVE_OPENGL` defined, so `GLConstants`
  (`renderbackend.h:42-59`) continues to carry real `GLenum` values that the
  new state cache can pass through, and keeps the legacy sources compiled so
  no configuration is left with no working GL backend. Core/ES2-only builds
  with `ENABLE_OPENGL=OFF` are explicitly out of scope; supporting them later
  requires translating `GLConstants` explicitly rather than casting.
- Legacy GL/GLEW sources stay in the unconditional lists for now, so the
  change does not disturb existing builds. New sources are added only when one
  of the new options is ON. A later cleanup can make the legacy sources
  conditional once the new backend is proven.
- Desktop core links `OpenGL::GL`; ES2 links the found OpenGLES/EGL libraries.
  CI already installs the needed GLES2/EGL packages.
- Extend the GL CI job with a core-profile configuration. Add an ES2 job that
  creates a GLES2 context through SDL with Mesa software rendering (EGL
  surfaceless or Xvfb + GLES2 visual); if a reliable CI context cannot be
  obtained, the ES2 job compiles the backend and runs shader-validation tests
  only, and the limitation is documented.

### Phasing (one branch, independently reviewable commits)

1. **Foundation** — context abstraction, loader, shader/buffer/VAO wrappers,
   one sprite program, and a textured quad drawn in core profile.
2. **Feature parity** — all `RenderBackend` primitives, `GLCoreImage`, atlas,
   overlays, lighting, stencil, depth, scissor, blending, render targets,
   screenshots.
3. **Integration** — Fifechan adapter, engine/settings selection, CMake
   options, tests, docs.
4. **ES2** — second shader preamble and precision qualifiers, ES2 texture
   fallbacks, extension handling, ES2 CI job.
5. **Follow-on (not this doc)** — instanced particle rendering on top of the
   batching foundation.

## Testing strategy

- Context/shader smoke tests in `tests/core_tests/` using the existing Catch2
  `SKIP` pattern when no suitable context can be created.
- A test that creates each supported context type (legacy compat, core 3.3,
  ES2 where available) and asserts version/profile strings and that all
  built-in shaders compile and link.
- Pixel tests for representative primitives (textured quad, coloured triangle,
  circle, overlay modes, stencil-based light) asserting self-consistency, not
  equality with the legacy backend.
- Verify that a renderer drawing through `Image::render()` produces the same
  quad count on the new backend as on the legacy one — this is the contract
  Track 1 depends on.
- Regression tests for engine selection by the two new backend names, for
  `FifechanManager` dispatch with the new GUI adapter, and for the OpenGL-side
  behaviour at all four backend-name branch sites (including the
  selected-but-not-compiled SDL fallback).
- Keep the dummy backend tests in `all_tests_dummy` green throughout; the new
  backend must not affect headless CI.

## Effort / sizing

| Phase | Estimate |
|-------|----------|
| Foundation (context, loader, shaders, buffers, first quad) | 2 weeks |
| Feature parity for `RenderBackend` + `GLCoreImage` | 4 weeks |
| Fifechan, engine/settings, build options, tests | 2 weeks |
| ES2 profile, shader variants, fallbacks, CI | 2 weeks |
| Validation, bug fixes, documentation | 1–2 weeks |
| **Total** | **~11–12 weeks** |

This is a project-sized change. The estimate assumes one developer familiar
with the codebase and no new platform targets beyond what SDL3 already
supports.

## Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Core/ES2 context unavailable on some CI runners | Medium | Medium | Keep the `SKIP` pattern; document, don't fake coverage |
| ES2 shader precision/NPOT differences cause visual bugs | Medium | Medium | Separate ESSL sources, explicit clamp/no-mipmap path, pixel tests |
| Dual backend maintenance burden | High | Medium | Isolate in `glcore/`, no branching in legacy code, retire legacy only in a later release |
| One of the four backend-name branch sites is missed | Medium | High | All four are listed explicitly in the constraints and engine-wiring sections; add a regression test for each new backend name |
| Fifechan adapter misses a drawing path | Medium | Medium | Implement all 14 virtuals, reuse the dummy adapter as a template |
| Build breakage from vendored glad on unusual toolchains | Low | Medium | Header-only generated sources, no new external package |

## What's left to the builder

Decisions deliberately left open because a competent builder can settle them
without changing the design:

- Exact glad generation commands and whether to keep one combined loader or
  two profile-specific ones.
- Whether shader sources live as raw string literals in C++ or as embedded
  resources generated at configure time; both are acceptable, no runtime file
  I/O.
- The order in which individual `RenderBackend` primitives are ported, as long
  as feature parity is reached before integration.
- Whether `ENABLE_OPENGL_CORE` / `ENABLE_OPENGL_ES2` are enough or a single
  `FIFE_GL_BACKEND` selector is preferable in this CMake style.
- How ES2 CI obtains a context, and what is skipped if it cannot.
- Whether the instanced particle fast path lands immediately after this work
  or in a separate follow-up issue.
