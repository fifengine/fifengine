# Backend-agnostic particle engine (Issue #414) — Design

## Intent and why

Issue #414 (2009) asks how complicated a *simple* particle engine for FIFE
would be, and observes that the SDL renderer is a fallback while a hardware
accelerated, shader-based particle system would be the ideal end state. The
previous analysis turned that observation into a prerequisite: a full OpenGL
pipeline rewrite. That rewrite is a separate, multi-month project (see
`2026-11-23-modern-gl-es2-design.md`, Track 2) and #793 was removed from the
0.5.0 milestone in 2026 while #414 remains in it.

This is **Track 1**: deliver a genuinely simple, scriptable particle engine in
0.5.0 that runs unchanged on SDL, legacy OpenGL, and the headless dummy
backends. The key discovery that makes this cheap is that a C++ view renderer
can draw textured quads through the public `Image::render()` API — no
`RenderBackend` interface change, no shaders, and no backend-specific code.
CPU simulation is backend-independent, so when the graphics roadmap's Tier A
backend lands (`2026-11-23-modern-gl-es2-design.md`), only the drawing path can
be upgraded; emitters, simulation, and the Python API stay as they are. Tier B
(`2026-11-23-modern-gl-43-es31-design.md`) can later move integration into a
compute shader underneath the same API.

## Goals

- Give Python game scripts a particle facility: emitters with shapes, rates,
  lifetimes, velocity/gravity/drag, colour and size interpolation, and atlas
  frames — enough for smoke, rain, sparks, leaves and similar effects.
- Keep simulation strictly backend-independent and tick it **once per frame**
  from the engine's existing time system.
- Draw particles through `Image::render()` so all four existing backends work
  without modification. On headless backends, drawing must be a safe no-op.
- Reuse existing FIFE conventions: `RendererBase` pipeline, `RendererNode`
  attachment, `AtlasLoader`/`useSharedImage()` sub-images, seeded RNG for
  deterministic tests, and a per-renderer `.i` binding file.
- Ship a runnable example so the answer to "how complicated is it?" is a demo,
  not a document.

## Non-goals

- GPU simulation, transform feedback, compute shaders, or instanced quads.
  Those belong to graphics Tier B and are explicitly a follow-on.
- Arbitrary per-particle geometry rotation and additive blending. Both need
  backend-specific state that would break backend agnosticism. Pre-rotated
  atlas frames are the v1 workaround for rotation; additive blending may be
  added later via a non-pure virtual with a no-op default.
- Force fields, particle collisions with map geometry, trails, render-to-texture
  effects, or editor integration.
- Byte-identical appearance across backends — alpha blending and filtering
  differ between SDL and OpenGL.
- Changing `RenderBackend` (not even by adding a non-pure virtual in v1), and
- Making this depend on the modern GL backend.

## Anti-goals

- Do not call `RenderBackendOpenGL`-specific methods (`addImageToArray` with
  texture ids, `changeRenderInfos`, Z-buffer paths) from the particle renderer.
  That would reintroduce the backend coupling this split was chosen to avoid.
- Do not attach the simulation tick to `ParticleRenderer::render()`. The camera
  calls every renderer once per rendered layer (`camera.cpp:1087-1158`), so a
  multi-layer/camera scene would advance the simulation several times per
  frame.
- Do not create a second ownership system for emitters. `RendererNode` already
  solves attachment to instances, locations, layers and screen points, and is
  already used by `LightRenderer` and `GenericRenderer`.
- Do not add a new `EngineSettings` default for this renderer. View renderer
  defaults live in `Engine::init()` today.

## Constraints and findings from the codebase

Verified while designing; the builder should not rediscover them.

1. **Renderer lifecycle.** `Camera` is both `IRendererListener` and
   `IRendererContainer` (`camera.h:44`, `:352-362`). `Camera::addRenderer()`
   inserts enabled renderers into a sorted pipeline keyed by `getName()`
   (`camera.cpp:785-792`), and `Camera::render()` calls each renderer then
   flushes `renderVertexArrays()` (`camera.cpp:1087-1158`). `RendererBase`
   derives with default `m_enabled=false` and pipeline position 1000
   (`rendererbase.cpp:29-39`). There is no per-renderer tick hook.
2. **Creation chain.** `Engine::init()` creates every renderer prototype and
   passes them to `Model` (`engine.cpp:699-715`, includes `:64-76`); `Model`
   stores raw `RendererBase*` (`model.h:45,194`) and forwards them into each
   `Map` (`model.cpp:90-103`); `Map::addCamera()` clones each prototype into the
   new `Camera` (`map.cpp:242-258`). A new renderer therefore needs an
   `Engine::init()` prototype; cloning into cameras is automatic. Source lists
   to edit: `CMakeLists.txt:407-423` (model), `:502-514` (renderers),
   `:736-750` (headers), `:825-898` (SWIG).
3. **`Image::render()` is the backend-agnostic draw call.** `image.h:68`
   declares `render(Rect const&, uint8_t alpha, uint8_t const* rgba)`; `Rect` is
   `x, y, w, h` (`rect.h:62-77`). `GLImage::render()` batches via
   `addImageToArray()` (`glimage.cpp:272-338`), `SDLImage::render()` draws
   immediately with `SDL_RenderTexture` (`sdlimage.cpp:92-152`), and
   `DummyImage::render()` forwards to a no-op in `RenderBackend`
   (`dummyimage.cpp:54-60`, `renderbackend.cpp:110-117`). The `RenderBackend`
   singleton is created by `Engine::init()` (`engine.cpp:393-415`). `Image::render()`
   is deliberately **not** exposed to Python (`video.i:43-70`); the particle
   renderer calls it from C++.
4. **Atlas frames already exist.** `Image::useSharedImage(ImagePtr, Rect)`
   (`image.h:188-208,217-233`) is implemented for GL (`glimage.cpp:776-848`),
   SDL (`sdlimage.cpp:164-197`) and dummy (no-op, `dummyimage.cpp:62-65`).
   `AtlasLoader::loadAtlas()` creates sub-images via `useSharedImage()`
   (`atlasloader.cpp:253-294`), and `AtlasBook` packs regions
   (`atlasbook.h:25-155`). A particle emitter can own a single atlas `ImagePtr`
   plus a small `std::vector<ImagePtr>` of sub-images, one per animation frame.
5. **Simulation tick.** `Engine::pump()` drives `TimeManager::update()` once
   per frame (`engine.cpp:850-856`), which fires registered `TimeEvent`s; a
   `TimeEvent` with period 0 fires every update (`timeevent.h:19-48`,
   `timemanager.cpp:41-71`). `InstanceRenderer` already uses a `FIFE::Timer`
   with a callback (`instancerenderer.h:145-148,235`; `.cpp:144-148`), so an
   engine-driven tick is an established pattern. `Model::update()` →
   `Map::update()` → `Layer::update()` → `Instance::update()`
   (`model.cpp:344-354`, `map.cpp:169-223`, `layer.cpp:630-659`,
   `instance.cpp:898-957`) is an alternative chain, but emitters are not
   instances.
6. **Attachment pattern.** `RendererNode` attaches to `Instance`, `Location`,
   `Layer` or a screen `Point` and tracks instance deletion internally
   (`renderernode.h:28-100`, bound in `renderernode.i:13-57`). `LightRenderer`
   and `GenericRenderer` already build element groups on top of it
   (`lightrenderer.h:263-336`, `genericrenderer.i:12-79`). This is the natural
   home for emitter placement.
7. **SWIG conventions.** Each renderer has its own `.i` file with a
   `static X* getInstance(IRendererContainer*)` that does a `dynamic_cast`
   via `cnt->getRenderer("X")` — e.g. `floatingtextrenderer.i:28`,
   `instancerenderer.i:29`. Config structs and enums need no separate `.i`;
   SWIG parses the header directly (`floatingtextrenderer.i:10`). The master
   template pulls `.i` files in through `@CMAKE_SWIG_INCLUSIONS@`
   (`fife.i.templ.cm:35-36,235-236`, configured at `CMakeLists.txt:1222-1226`).
   `src/python/fife/fife.pyi` is a small hand-maintained dynamic stub
   (`fife.pyi:1-39`) with no renderer classes today.
8. **Tests.** Catch2 `all_tests` (`tests/core_tests/CMakeLists.txt:9,15-59`),
   with a `[dummy]` tag split into `all_tests_dummy` (`:146-155`). Headless
   tests construct engines with `DUMMY_NULL` / `DUMMY_SURFACE` and assert
   `getWindow() == nullptr` (`test_renderbackend_dummy.cpp:47-83`). SWIG tests
   use pytest with engine fixtures (`tests/swig_tests/conftest.py:39-110`) and
   already exercise `InstanceRenderer.getInstance(cam)`
   (`test_view.py:42-43`).

## Approaches considered

### Option A — Python-only particle manager (rejected)

A `ParticleManager` in `src/python/fife/` driving existing `Instance` and
`Animation` objects from a Python timer.

- Trades off: zero C++ or backend changes; fastest to write.
- Rejected: per-particle Python objects are slow at the target counts, it is a
  workaround rather than an engine feature, and C++ games could not use it.
  Retained as an optional example/demo layer, not as the implementation.

### Option B — Particle API on `RenderBackend` (rejected)

Add `addParticleBatch()` to the backend interface with per-backend
implementations using `SDL_RenderGeometry` and OpenGL instancing.

- Trades off: best raw performance.
- Rejected: there are four concrete backends today (`SDL`, `OpenGL`, `Null`,
  `Surface`), so a pure virtual touches all of them and the headless test
  path; ES2 instancing needs extensions or a CPU fallback; and it makes #414
  wait for the GL rewrite. This is the coupling the split was meant to remove.

### Option C — Simulation in the model, drawing in the view, using `Image::render()` (chosen)

A model-side `ParticleManager` owns emitters and the once-per-frame tick; a
view-side `ParticleRenderer` reads particle state and calls `Image::render()`.
No backend code changes at all.

- Trades off: per-particle `Image::render()` calls are more expensive than a
  dedicated batch API (roughly 1k–3k particles on SDL, 5k+ on OpenGL, at 60
  FPS), and rotation/additive blending are unavailable in v1.
- Chosen: it is the only variant that ships in 0.5.0, works on every backend,
  and leaves simulation untouched by the graphics roadmap.

### Sub-decisions

- **Tick source:** `TimeEvent` with period 0 owned by `ParticleManager`
  (chosen) over ticking from `render()` (called once per layer, would advance
  multiple times per frame) or from `Instance::update()` (emitters are not
  instances).
- **Emitter placement:** existing `RendererNode` (chosen) over a bespoke
  layer-name + position pair, which loses instance attachment and deletion
  tracking.
- **Rotation and additive blending:** deferred (chosen) over per-backend
  special cases that would break the no-backend-change guarantee.

## Design

### Architecture

```
   Engine::pump() ── TimeManager::update() ── ParticleManager::tick(dt)
                                                    │  (once per frame)
                                        ┌───────────┴───────────┐
                                   Emitter A              Emitter B
                                   (pool, RNG,            (pool, RNG,
                                    RendererNode)          RendererNode)
                                        │                      │
   Camera::render(layer, ...) ── ParticleRenderer::render() ───┘
                                        │
                          Image::render(Rect, alpha, rgba) per particle
                                        │
              GLImage::addImageToArray  /  SDLImage::SDL_RenderTexture  /  no-op
```

### Model: `ParticleManager` and `ParticleEmitter`

New files under `src/fife/model/structures/` (or `src/fife/model/`, builder's
call): `particlemanager.h/.cpp`, `particleemitter.h/.cpp`.

- `ParticleManager` is a singleton-style owner of all emitters
  (`std::vector<std::unique_ptr<ParticleEmitter>>`). It registers a
  `TimeEvent` with period 0 on creation and unregisters it on shutdown, so
  `Engine::pump()` advances every emitter exactly once per frame. On
  registration it calls `TimeEvent::setLastUpdateTime64(TimeManager::now64())`:
  the constructor already stamps that value (`timeevent.cpp:21`), so without
  this reset the first `updateEvent()` would receive the whole
  init-to-first-frame gap as its delta (`timeevent.cpp:32-42`).
  Emitter creation/removal is safe while iterating.
- `ParticleEmitter` holds:
  - a `Config` POD (emission rate, burst count/interval, shape and size,
    lifetime min/max, speed min/max, gravity, drag, start/end size, start/end
    RGBA, atlas frame count and duration, a `localSpace` flag, `maxParticles`,
    loop/one-shot flag). **There is no angular-velocity field in v1** — see
    "Simulation semantics" below;
  - a **preallocated particle pool** (`std::vector<Particle>` plus a free-index
    stack) so no heap allocation happens per particle per frame;
  - a seeded `std::mt19937` so simulation is reproducible and Python can set a
    seed;
  - a `RendererNode` for placement, with `attachToInstance`,
    `attachToLayer`, `attachToPoint` and `detach` forwarding methods;
  - an `ImagePtr` atlas image and a `std::vector<ImagePtr>` of sub-images
    created with `useSharedImage()`.
- `update(dt)` emits due particles, integrates velocity/gravity/drag, advances
  age and atlas frame, and recycles dead particles. It has no drawing code and
  no GL/SDL types.

### Simulation semantics (decided)

- **Particles are world-space by default.** `Particle` stores absolute world
  coordinates; at emission the position is the node's current world position
  plus the shape offset, and after emission the particle is independent of
  later emitter movement. Without this, a smoke plume would translate rigidly
  when its attached instance walks. `Config::localSpace = true` opts into
  emitter-relative offsets, added to the node position at render time, for
  effects that should follow a character. The renderer transforms each
  particle through `Camera::toScreenCoordinates()` (the path already used in
  `renderernode.cpp:318-322`).
- **Rotation is out, not implicit.** The non-goal on geometry rotation also
  removes `angularVelocity` from v1; a pre-rotated-frame extension would need a
  second atlas dimension (rotation step × animation frame) and is not part of
  this design.
- **Pool capacity and overflow.** `maxParticles` is Python-configurable per
  emitter, defaulting to `ceil(rate * maxLifetime) + burstCount` and clamped to
  2048. When the pool is full, **new emissions are dropped**; live particles
  are never recycled early, so visual continuity is preserved. The
  emitted-count reported to Python counts actual allocations.
- **Fixed timestep.** The manager accumulates the frame delta and advances in
  fixed steps of 1/120 s, clamping any single frame delta to 50 ms before
  accumulation. Seeded RNG plus fixed steps make a given seed reproduce the
  same particle sequence; wall-clock jitter only changes how many steps run in
  a frame. Unit tests call `update(dt)` directly with a fixed `dt` and never
  depend on `TimeEvent`.
- **Attached instance deleted.** The emitter captures the node's last resolved
  world position, disables further emission, and clears the attachment. Because
  particles are world-space, existing ones simply finish where they are. The
  emitter stays in the manager so Python can re-attach it. The emitter tracks
  its own drawn layer at attach time for filtering rather than relying on
  `RendererNode::isDrawnOn()` after detachment: `removeInstance()` only clears
  `m_instance` (`renderernode.cpp:292-300`), after which `isDrawnOn()` can fall
  through to "every layer" and `getCalculatedPoint()` to the raw screen point
  (`:342-360`), which would draw the plume repeatedly at (0,0).

### View: `ParticleRenderer`

New files `src/fife/view/renderers/particlerenderer.h/.cpp` and
`particlerenderer.i`.

- Extends `RendererBase`; follows the standard static
  `ParticleRenderer::getInstance(IRendererContainer*)` pattern.
- `render(Camera*, Layer*, RenderList&)` iterates the manager's emitters,
  selects those drawn on the current layer (or unattached, meaning
  screen-space), and for each alive particle:
  - takes the particle's stored world position (or adds the node position in
    `localSpace` mode), converts it with `Camera::toScreenCoordinates()`, and
    builds a `Rect` from particle size;
  - picks the current atlas sub-image by frame index;
  - calls `subImage->render(rect, alpha, rgba)`, where `rgba` carries the
    interpolated colour modulation and the particle's alpha.
- Pipeline position sits near the instance renderers so particles draw with
  the scene; the exact value is a builder call.
- `reset()` clears only per-camera caches. Particle state is model-owned and
  survives renderer resets and camera reattachment.
- With a dummy backend, `DummyImage::render()` is a no-op, so the whole path is
  safe headlessly with no backend checks in the renderer.

### Engine, CMake and Python wiring

- `Engine::init()`: add the renderer include near `engine.cpp:64-76` and a
  prototype near `:699-712`. `Model`/`Map` cloning handles the rest.
- `ParticleManager` lifecycle is tied to `Engine` so the `TimeEvent` is
  registered after `TimeManager` exists and unregistered before shutdown.
- `CMakeLists.txt`: add sources to the model/renderer lists
  (`:407-423`, `:502-514`), headers to `:736-750`, and `particlerenderer.i`
  plus any model `.i` file to `:825-898`.
- `particlerenderer.i` exposes `ParticleRenderer`, `getInstance()`, and the
  manager/emitter/config types and enums by including their headers.
- Optional `.pyi` typing and a runnable example script under `tests/fife_test/`
  or the demos directory. `Image::render()` itself stays out of Python, as it
  is today.

### Testing strategy

- C++ Catch2 test with a seeded emitter: emit a known count, step `update(dt)`
  a fixed number of times, assert particle counts, positions and recycling;
  assert identical results for identical seeds.
- Test pool overflow: configure a small `maxParticles`, exceed it with a burst,
  assert the cap is respected and live particles are not recycled.
- Test world-space versus `localSpace` behaviour by moving the emitter's
  attached instance between steps and asserting which particles move.
- Test that a renderer drawing particles under `DUMMY_NULL` and
  `DUMMY_SURFACE` does not throw and leaves the engine healthy, tagged `[dummy]`.
- Test emitter attachment via `RendererNode`, including that deleting the
  attached instance stops emission, leaves existing particles at their world
  positions, and does not draw them on extra layers.
- SWIG test: create a manager and emitter from Python, configure it, pump the
  engine, and assert the reported alive-particle count; verify
  `ParticleRenderer.getInstance(cam)` returns the renderer.
- A visual example for manual inspection; no pixel snapshots, since output
  legitimately differs per backend.

## Effort / sizing

| Task | Estimate |
|------|----------|
| `ParticleEmitter`: config, pool, seeded RNG, simulation | 3 days |
| `ParticleManager`: ownership, `TimeEvent`, `RendererNode` placement | 2 days |
| `ParticleRenderer`: camera transform, atlas frames, `Image::render()` | 3 days |
| Engine / CMake / SWIG wiring | 2 days |
| Tests, example, documentation | 2 days |
| **Total** | **~12 days** |

This is a small-to-medium change. It is written at full length because it
introduces a new model subsystem, a new view renderer and a new Python API,
but it stays isolated from all four backends.

## Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Per-particle `Image::render()` too slow on SDL | Medium | Medium | Document ~1k–3k budget for SDL; graphics Tier B adds a compute path without touching simulation |
| Backend differences in alpha blending | High | Low | Document; no cross-backend pixel equality promised |
| `TimeEvent` double registration or huge first delta across engine re-init | Medium | Medium | Tie registration to `Engine` lifecycle, reset last-update time on registration, clamp deltas |
| Emitters outliving their attached instance | Medium | Medium | Explicit detach policy: stop emission, keep world-space particles, track drawn layer locally |
| Per-particle `Image::render()` cost exceeds the SDL budget | Medium | Medium | Configurable `maxParticles` cap with drop-on-full; document the budget |
| Atlas sub-image creation complexity | Medium | Low | Reuse `AtlasLoader` conventions; start with a single image per emitter |

## What's left to the builder

Decisions deliberately left open because a competent builder can settle them
without changing the design:

- Whether the model classes live under `src/fife/model/` or
  `src/fife/model/structures/`, and the exact source-list placement in CMake.
- Exact field names and defaults in `ParticleConfig`, and the preset names
  shipped with the example (if any).
- The final pipeline position value and whether the renderer is enabled by
  default in `Engine::init()`.
- How the manager is exposed to Python (singleton accessor versus engine
  getter), as long as `ParticleRenderer.getInstance(cam)` follows the existing
  renderer convention.
- Where the example script lives and which effects it demonstrates.
- Whether additive blending arrives as a non-pure `RenderBackend` virtual
  after the graphics roadmap, or stays out of scope permanently.
