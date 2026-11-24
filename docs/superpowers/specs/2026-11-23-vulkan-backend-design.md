# Tier C: Vulkan render backend (foundation document) — Design

> **Status: out of scope.** This document is a design *seed* for work that is
> not planned or approved. It exists so that a future implementer starts from
> FIFE's real architecture rather than rediscovering it. Nothing here is a
> commitment, a schedule, or a request to add dependencies today.

## Intent and why

OpenGL and OpenGL ES are in maintenance. Android documents Vulkan as its
primary low-level graphics API and says GLES is no longer under active feature
development; Apple deprecated GLES and caps desktop GL at 4.1; WebGPU is now
the default backend in the major browsers and is architecturally closer to
Vulkan than to GL. On the hardware FIFE cares about, Vulkan is already the
forward path: Raspberry Pi 4 and 5 expose Vulkan 1.2 through Mesa's `v3dv`
driver, and Mesa's `lavapipe` provides a CPU Vulkan implementation suitable for
headless CI.

Tiers A and B (`2026-11-23-modern-gl-es2-design.md`,
`2026-11-23-modern-gl-43-es31-design.md`) give FIFE a shader-based 2D renderer
and a compute-driven particle path. That is the right near-term answer for #793
and modern GL hardware, but not a long-term endpoint.

**Most important architectural principle:** keep Vulkan as an implementation
detail of the rendering backend. The rest of FIFE expresses *what to render*,
not *how Vulkan renders it*. Almost no GL code can be reused — what carries
over is the *design*: the batching contract, backend-independent particle
simulation, profile-independent shader semantics, and the generic Fifechan
adapter pattern.

## Goals (for the eventual implementation)

- Provide a first-class `RenderBackendVulkan` selectable by name, with parity
  for everything FIFE draws: textured quads with per-vertex colour, primitives,
  overlays, stencil-based lighting, render targets and screenshots.
- Preserve the `RenderBackend` / `Image` public interfaces. In particular,
  `addImageToArray()` and `renderVertexArrays()` keep their meaning, so view
  renderers and Track 1's particle renderer work unchanged.
- Keep SDL responsible for window and surface integration; own the Vulkan
  instance, device, swapchain, command buffers and synchronisation in FIFE.
- Establish explicit ownership/lifetime, synchronisation, shader-delivery,
  diagnostics and CI rules before optimising anything.
- Be testable headlessly on `lavapipe` with validation layers enabled.

## Non-goals

- Implementation, dependency changes or CI changes as part of *this* document.
- Ray tracing, mesh shaders, bindless resources, multi-threaded command
  recording, or explicit multi-GPU in the first version.
- A minimum version above Vulkan 1.0 for core functionality; 1.1/1.2 features
  are enabled opportunistically when present.
- Removing the GL or SDL backends. Vulkan is additive.
- Byte-identical output compared with GL or SDL.

## Anti-goals

- Do not retrofit Vulkan into `RenderBackendOpenGL` or the `glcore/` family.
  GL and Vulkan have incompatible object, state and synchronisation models.
- Do not leak Vulkan types into shared engine code. No `Vk*` in `RenderBackend`,
  `Image`, view renderers, model classes or Python bindings.
- Do not add **pure** virtuals to `RenderBackend`. There are already four
  concrete backends (SDL, GL, Null, Surface) plus Tier A/B variants. New hooks
  must be non-pure with safe defaults, as with Tier B's
  `IParticleRenderBackend`.
- Do not let Fifechan adapters cast to concrete backends. `DummyGuiGraphics`
  shows the correct pattern: generic `fcn::Graphics` forwarding to
  `RenderBackend` primitives and `Image::render()`.
- Do not require a build-time GLSL toolchain for end users; ship precompiled
  SPIR-V.
- Do not optimise before correctness, validation-layer cleanliness and tests
  are established.

## Constraints and findings from the codebase

Verified against the current tree; a future builder should not rediscover them.

1. `RenderBackend` is a `DynamicSingleton` with a large primitive API
   (`renderbackend.h:113` onward), pure virtuals including `init()`,
   `createMainScreen()`, `createImage()` overloads, `renderVertexArrays()`,
   `addImageToArray()` (`:219, :223`), `attachRenderTarget()` /
   `detachRenderTarget()` (`:628-632`) and `renderGuiGeometry()` (`:636-640`).
   `RenderBackendDummy` is the best template for no-op/safe defaults.
2. `Image` (`image.h:62-110`) owns an `SDL_Surface*` and defines `render()`,
   `renderZ()`, `useSharedImage()`, `load()` and `free()`. A `VulkanImage` keeps
   the surface behaviour and adds a Vulkan image, image view, sampler and
   descriptor reference. Atlas sub-images work through `useSharedImage()`
   (`image.h:188-233`, `atlasloader.cpp:253-294`).
3. Drawing is batched through `Image::render()` → `addImageToArray()` →
   `renderVertexArrays()`; camera flushes after each renderer
   (`camera.cpp:1087-1158`). Preserving this contract is what keeps every view
   renderer — including Track 1's — backend-agnostic.
4. `Window::toSDLFlags()` (`window.cpp:649-675`) adds `SDL_WINDOW_OPENGL` only
   when `m_settings.opengl` is set; `Engine::init()` sets it via a concrete cast
   (`engine.cpp:589`). A `vulkan` flag and a parallel cast branch are needed.
5. SDL3's Vulkan support is deliberately minimal: `SDL_Vulkan_GetInstanceExtensions`,
   `SDL_Vulkan_GetVkGetInstanceProcAddr`, `SDL_Vulkan_CreateSurface`,
   `SDL_Vulkan_DestroySurface`, `SDL_Vulkan_GetDrawableSize`,
   `SDL_Vulkan_GetPresentationSupport`, plus the `SDL_WINDOW_VULKAN` flag. SDL
   provides no swapchain, context or swapping help — that is entirely FIFE's.
6. Backend selection is the string chain in `Engine::init()`
   (`engine.cpp:393-423`) with names from `enginesettings.cpp:160-168`. Four
   downstream name-branch sites exist: `instancerenderer.cpp:137-162`,
   `layercache.cpp:196`, `camera.cpp:1023`, `imageloader.cpp:133`. Tier A chose
   to keep them as strings; Tier C revisits that, see "Engine integration".
7. Fifechan selection is string-based in `FifechanManager::init()`
   (`fifechanmanager.cpp:259-282`) with concrete casts in
   `resizeTopContainer()` (`:210-218`).
8. CMake has one GL option (`ENABLE_OPENGL`, `CMakeLists.txt:92-100, :162-164`)
   and no Vulkan discovery. A future change adds `ENABLE_VULKAN`, Vulkan
   headers/loader discovery, and SPIR-V resources. CI already installs Mesa
   packages; `lavapipe` needs no X server.
9. Tier B's compute-particle design keeps emission and recycling on the CPU and
   only integration on the GPU, behind `IParticleRenderBackend`. A Vulkan
   backend can implement that interface with a compute pipeline and a storage
   buffer; Track 1's emitter code and Python API stay untouched.
10. The headless dummy backends (`src/fife/video/dummy/`) must keep working. A
    Vulkan build that cannot create a device must fall back cleanly, following
    the existing SDL fallback (`engine.cpp:414-422`).

## Approaches considered

### Option A — Extend the GL backend family with a Vulkan path (rejected)

Share `RenderBackendGLCore` and switch internals at runtime. Rejected: every
method becomes a two-way branch and neither path is testable in isolation.

### Option B — Standalone `RenderBackendVulkan` family (chosen as direction)

A new backend under `src/fife/video/vulkan/` implementing the same public
interfaces as every other backend. Mirrors the decisions already made for the
dummy backends and for Tiers A/B; isolated, reversible, testable.

### Option C — Adopt WebGPU instead of Vulkan (noted, not chosen)

Lower boilerplate and a closer match for the web, but newer native desktop
support and a weaker Android/Pi story. Revisit if platform priorities change.

## Proposed architecture

```
                RenderBackend (abstract, DynamicSingleton)
       /          |            |              \              \
     SDL      OpenGL(legacy)  Dummy       GLCore (Tiers A/B)  Vulkan (Tier C)
                              /  \          /      \              |
                          Null Surface   GL3.3/ES2  GL4.3/ES3.1   vulkan/
                                                                  instance | device
                                                                  surface | swapchain
                                                                  allocator | resources
                                                                  pipelines | descriptors
                                                                  frame | sync

   Fifechan:  VulkanGuiGraphics -> generic RenderBackend primitives + Image::render()
   Particles: IParticleRenderBackend -> Vulkan compute path; CPU path unchanged
```

New code under `src/fife/video/vulkan/`:

- `vulkaninstance.h/.cpp` — `VkInstance`, validation layers (dev builds),
  `SDL_Vulkan_GetInstanceExtensions`, proc-address loading.
- `vulkandevice.h/.cpp` — physical device selection, logical device, queues.
- `vulkansurface.h/.cpp` — `SDL_Vulkan_CreateSurface`, swapchain, drawable size.
- `vulkanallocator.h/.cpp` — buffers, images, memory, suballocation.
- `vulkanresources.h/.cpp` — owned resources plus the retirement queue.
- `vulkanpipelines.h/.cpp`, `vulkandescriptors.h/.cpp` — fixed pipeline layouts
  and descriptor sets.
- `vulkanframe.h/.cpp` — per-frame command buffers, fences, semaphores, present.
- `renderbackendvulkan.h/.cpp` — the `RenderBackend` implementation.
- `vulkanimage.h/.cpp` — `Image` over `VkImage` / `VkImageView` / `VkSampler`.
- `vulkan_gui_graphics.h/.cpp` — generic Fifechan adapter.
- `shaders/` — GLSL sources and checked-in SPIR-V.

## Backend abstraction (checklist 1)

- View renderers and particle systems stay backend-agnostic. `addImageToArray()`
  appends to a CPU-side vertex staging buffer; `renderVertexArrays()` records a
  draw into the active command buffer. No Vulkan calls outside `vulkan/`.
- Shared interfaces express *intent* only: blend/stencil/scissor/depth through
  existing `RenderBackend` methods, textures through `ImagePtr`.
- GPU particles use the narrow `IParticleRenderBackend` interface from Tier B;
  the Vulkan implementation lives behind it and returns null on every other
  backend.

## Resource management (checklist 2)

- Every Vulkan object has exactly one owner: the backend for device-wide
  objects (instance, device, pipelines, descriptor layouts, sampler pool), a
  per-frame structure for command buffers/fences/semaphores and staging
  buffers, and `VulkanImage` for image/view/sampler bindings. None of them own
  the logical device.
- `VulkanImage` holds no independent device pointer; it references
  backend-owned pools, so teardown order cannot dangle. `copySubimage()` and
  atlas sub-images reuse the parent texture rather than allocating new device
  memory.
- **Destruction is deferred, never immediate.** A resource submitted to the GPU
  goes into a retirement queue tagged with the frame that last used it. It is
  destroyed only after that frame's fence signals. This covers texture
  deletion, buffer reuse and window/context recreation.
- Device shutdown drains the queue with `vkDeviceWaitIdle()` and destroys in
  reverse creation order. Window replacement follows the GL backend's
  recreate-from-scratch rule (`renderbackendopengl.cpp:270-332`).
- Staging buffers live in a per-frame ring; a frame's staging storage is not
  mapped or overwritten until its fence is signaled, which is the CPU/GPU
  synchronisation contract for uploads.

## Rendering architecture (checklist 3)

Four fixed pipelines, no dynamic pipeline generation:

1. **quad** — textured quad with per-vertex RGBA; sprites, text, GUI images and
   CPU-path particles.
2. **primitive** — coloured triangles for lines, circles and simple fills.
3. **overlay** — two textures reproducing the four `OverlayType` modes.
4. **particles** — `particle_compute` plus `particle_instanced` implementing
   Tier B's CPU-emit/GPU-integrate split.

Data organisation:

- **Descriptor set 0 (frame):** a uniform buffer with projection, view,
  viewport and time; updated once per frame, never per draw.
- **Descriptor set 1 (material):** combined image samplers for the bound
  textures (start with four slots, matching the existing texture-unit count).
  Changed only when the texture set changes.
- **Push constants:** small per-draw data — tint, alpha test, overlay mode,
  atlas offset — avoiding descriptor writes per particle batch.

Batches are sorted by `(pipeline, texture set, blend state)` before recording so
pipeline and descriptor changes are minimised. `changeBlending()` and
`changeRenderInfos()` map to fixed pipeline state, not shader recompilation.
Alpha test is `discard` in the fragment shader; lighting/stencil map to stencil
pipeline state and attachment operations.

## Synchronisation and frames in flight (checklist 4)

- `FRAME_IN_FLIGHT_COUNT = 2` initially; three only if measurements justify it.
- Each in-flight frame owns a command buffer, a fence (CPU/GPU completion), an
  `imageAvailable` semaphore (swapchain acquire → draw) and a `renderFinished`
  semaphore (draw → present).
- Loop: wait on the frame fence → reset and begin recording → acquire the next
  swapchain image → record batches → submit with both semaphores → present →
  advance the frame index.
- A resource is never overwritten while a frame that references it is pending.
  Uploads use the per-frame staging ring; deletions use the retirement queue
  above. Swapchain recreation waits for all fences before destroying old images.
- Validation-layer cleanliness is a hard exit criterion; no synchronisation
  warning is accepted as "harmless".

## SDL integration (checklist 5)

- SDL owns the window and surface only: `SDL_Vulkan_GetInstanceExtensions`,
  `SDL_Vulkan_GetVkGetInstanceProcAddr`, `SDL_Vulkan_CreateSurface`,
  `SDL_Vulkan_DestroySurface`, `SDL_Vulkan_GetDrawableSize`,
  `SDL_Vulkan_GetPresentationSupport`, `SDL_WINDOW_VULKAN`.
- FIFE owns instance, device, swapchain, render pass, framebuffers, command
  buffers and presentation.
- Resize and DPI changes are driven by `SDL_Vulkan_GetDrawableSize`, mapping
  onto the existing `getDrawableSizeOrFallback` / point-versus-pixel logic
  (`renderbackendopengl.cpp:104-120`, `window.cpp` sizing). A resize invalidates
  the swapchain and framebuffers and waits for in-flight frames before
  recreating them. The high-DPI path already present in `Window` must keep
  working.

## Shader management (checklist 6)

- GLSL sources live in the repo; `glslangValidator -V` produces SPIR-V that is
  checked in under `src/fife/video/vulkan/shaders/`. A CMake target regenerates
  them for developers; a normal build only consumes the checked-in binaries.
- A shader manifest records source version, target Vulkan version, entry points
  and a hash. At load time the backend validates the SPIR-V magic, entry points
  and descriptor bindings, and throws a descriptive `SDLException` on mismatch.
- Compatibility rules: SPIR-V targets Vulkan 1.0 baseline; 1.1/1.2-only features
  are gated behind device capability checks; one binary per pipeline variant;
  no runtime GLSL compilation.

## Device and platform compatibility (checklist 7)

- Minimum Vulkan 1.0; required features limited to what the four pipelines use.
  Optional 1.1/1.2 features are detected, not assumed.
- Device selection prefers discrete GPU, then integrated, then software
  (`lavapipe`), with a settings override to force software for CI. Selection
  checks graphics+present queue support and required extensions.
- **MoltenVK and Apple limitations are evaluated before implementation starts**,
  not after: confirm portability caveats and whether the SDL surface path works
  on macOS; GL remains the fallback there.
- Meaningful errors when Vulkan is unavailable: distinguish library load
  failure, no physical device, no present-capable queue, missing extensions and
  shader validation failure, each with a clear message; selectable backend then
  falls back to SDL with a warning as today (`engine.cpp:414-422`).

## Engine integration (checklist 8)

**Core principle:** Tier C improves the abstraction *without changing the
behavioural contract established by Tier A*. Introducing a capability
interface and preserving existing backend behaviour are separate goals; the
second always wins.

Rules for the capability migration:

1. **Preserve Tier A behaviour by default.** No change to any existing backend,
   and in particular no change to dummy-backend semantics.
2. **Use non-pure virtual capability methods.** Every default reproduces
   current behaviour exactly for a backend that does not override it.
3. **Override selectively.** Only a backend with genuinely different
   capabilities overrides a default; no blanket "all modern backends" switches.
4. **Migrate incrementally.** Replace one string branch at a time, only after
   equivalence for that site is demonstrated. Capability methods may coexist
   with string checks during the transition.
5. **Retain string checks where necessary.** Especially where the dummy
   backends have inconsistent historical behaviour across sites.
6. **Test behavioural equivalence.** Compare each existing backend's behaviour
   before and after every individual migration.

**Document the expected default of every capability method.** A default that
is technically valid but semantically wrong is a trap for future backends. Each
method's contract, default value and rationale must be written in
`renderbackend.h` alongside the declaration, not inferred from call sites.

### Capability methods and their exact defaults

These defaults are derived from the current string branches and reproduce them
for all four existing backends:

| Method (on `RenderBackend`, non-pure) | Default | Overridden by | Replaces |
|---------------------------------------|---------|---------------|----------|
| `bool usesGpuDepthBuffer() const` | `false` | `RenderBackendOpenGL` → `true` | `getName() == "OpenGL" && isDepthBufferEnabled()` in `instancerenderer.cpp:137,157` and `layercache.cpp:196` |
| `bool needsColorBindingWorkaround() const` | `false` | `RenderBackendSDL` → `true` | `getName() == "SDL"` in `instancerenderer.cpp:142,162` |
| `bool hasFlippedRenderTargetY() const` | `true` | `RenderBackendSDL` → `false` | `getName() == "SDL"` in `camera.cpp:1023` |
| `SDL_PixelFormat getPreferredImagePixelFormat() const` | `SDL_PIXELFORMAT_RGBA32` | `RenderBackendSDL` → `SDL_PIXELFORMAT_RGBA8888` | `getName() == "SDL"` in `imageloader.cpp:133` |

With these defaults: SDL gets false/false/false/RGBA8888, legacy OpenGL gets
true/false/true/RGBA32, and both dummy backends keep their current mixed
behaviour (false/false/true/RGBA32) without overrides. The user setting
`isDepthBufferEnabled()` remains a separate condition — the capability reports
*capability*, not current configuration.

`RenderBackendVulkan` overrides only the methods where its behaviour genuinely
differs; those values are decided during implementation and must be recorded in
the same table. No capability method mentions Vulkan, GL or SDL in its name.

### Migration procedure per site

For each of the four string-branch sites:

1. Record current output/state for SDL, legacy OpenGL, `DUMMY_NULL` and
   `DUMMY_SURFACE` in a regression test.
2. Add the capability method with the default above and change the call site to
   use it.
3. Run the test unchanged for all four backends; no expectations may change.
4. Only then add the Vulkan override if needed.

If a site cannot pass step 3 without changing an expectation, keep its string
check permanently and document the reason. `FifechanManager` follows the same
incremental rule.

### Other integration points

- `FifechanManager` gains a `"VULKAN"` branch and a `VulkanGuiGraphics` that
  implements all 14 `fcn::Graphics` pure virtuals through generic primitives
  and `Image::render()`, with no Vulkan dependency in the generic adapter.
- All GL-specific assumptions — Y-flip, pixel format, depth-sort reliance,
  colour binding in `instancerenderer.cpp` — are reviewed explicitly for the
  new backend rather than inherited by default.

## Testing, CI and validation (checklist 9)

- Headless CI on Mesa `lavapipe`, no Xvfb. One job runs with
  `VK_LAYER_KHRONOS_validation` enabled and treats any validation message as a
  failure.
- Tests cover: instance/device/surface/swapchain creation; repeated create and
  destroy of textures and buffers; resize and drawable-size changes; shutdown
  with frames still in flight; golden self-consistent pixel output for
  primitives and overlays; and the CPU/GPU particle tolerance tests from Tier B.
- Contract tests assert that a renderer drawing through `Image::render()`
  produces the same batch count on Vulkan as on the GL backends.
- **Capability-migration equivalence tests:** for each of the four string-branch
  sites, snapshot the observable behaviour of SDL, legacy OpenGL, `DUMMY_NULL`
  and `DUMMY_SURFACE` before migration and assert it is unchanged after. These
  tests are the gate that permits each individual migration.
- Keep `[dummy]` and all other backend jobs green.

## Performance and diagnostics (checklist 10)

- Validation layers are on by default in development builds and off in release,
  with a settings override for both.
- Instrument counters for draw calls, pipeline switches, descriptor writes,
  allocations and bytes uploaded, exposed through the existing logger. Optional
  GPU timestamps via `VK_QUERY_TYPE_TIMESTAMP` when supported.
- Establish correctness and validation cleanliness first; optimisation is a
  later phase with measured evidence, not a design-time goal.

## Implementation order

Adopted from the implementation checklist; each phase has an explicit exit
criterion. Later phases must not start before the earlier exit criterion is met.

| Phase | Focus | Exit criterion |
|-------|-------|----------------|
| 1 | Instance, device, surface | Vulkan initialises; a validation-clean headless context can be created |
| 2 | Swapchain and frame lifecycle | Frames present correctly, including resize and in-flight shutdown |
| 3 | Memory and resource management | Textures and buffers work; retirement/deferred destruction tested |
| 4 | Quad pipeline | Images render correctly through `addImageToArray()` / `renderVertexArrays()` |
| 5 | Primitive and overlay pipelines | Existing UI and GL-equivalent rendering works |
| 6 | Particle pipeline | Track 1 integration works via `IParticleRenderBackend` |
| 7 | Fifechan and engine integration | Backend is interchangeable; capability interfaces in place |
| 8 | CI, validation and optimisation | Stable cross-platform backend; validation-clean CI job |

## Risks and open issues

| Risk / issue | Why it matters |
|--------------|----------------|
| Vulkan boilerplate is far larger than GL | Honest sizing; likely several months to parity |
| Validation layers must be clean | Otherwise driver-specific bugs dominate later work |
| Y-flip / coordinate conventions differ from GL | Decide per capability hook, never by assumption |
| Descriptor and allocation strategy constrains later features | Keep behind an internal interface |
| Shader delivery as checked-in SPIR-V | Needs a reproducible toolchain and versioning policy |
| Platform coverage on Apple | Requires MoltenVK evaluation before implementation |
| SDL3 surface support is minimal | No help with swapchain, sync or device loss |
| Capability-interface migration changes shared code | Defaults preserve today's behaviour exactly; one site at a time; equivalence test per site; keep string checks where equivalence cannot be shown |

## What a future builder must decide

Deliberately open because they depend on conditions that do not yet exist:

- Whether to require Vulkan 1.1/1.2 for anything, or stay on the 1.0 baseline.
- Whether to depend on Vulkan-Headers/volk or load entry points solely through
  `SDL_Vulkan_GetVkGetInstanceProcAddr()`.
- The final descriptor layout, suballocation strategy and in-flight frame count.
- Which of the four string-branch sites pass the equivalence gate in the
  migration procedure, and which therefore stay as strings.
- How SPIR-V is generated, versioned and shipped.
- Whether this backend eventually replaces a GL backend or merely coexists.
- Whether multithreaded command recording is ever worth the complexity.

## Relationship to the other tiers

Tier C does not depend on Tiers A or B being completed in code, but it depends
on their *decisions*: the batching contract that keeps view renderers
backend-agnostic, CPU-owned particle simulation behind a narrow interface, and
the generic Fifechan adapter pattern. If those decisions change, this document
must be revised before implementation starts.
