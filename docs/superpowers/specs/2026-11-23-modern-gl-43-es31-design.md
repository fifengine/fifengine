# Tier B: OpenGL 4.3 core / OpenGL ES 3.1 profiles — Design

## Intent and why

Tier A (`2026-11-23-modern-gl-es2-design.md`) establishes a shader-based
`glcore/` backend family with two profiles: GL 3.3 core and ES 2.0. That tier
targets the widest possible hardware reach, including the original #793
Raspberry Pi motivation, and deliberately excludes compute shaders.

Tier B adds **GL 4.3 core and ES 3.1** as two more profiles on the same
foundation. These versions are near-equivalent in features (compute shaders,
SSBOs, instancing, UBOs, VAOs), so they form one coherent profile pair. The
payoff for FIFE is a GPU-side particle path: the O(N) integration step from
Track 1 moves into a compute shader, raising the particle budget substantially
without changing the scripting API.

This is Tier B of the three-tier roadmap; Tier C (Vulkan) is long-term and out
of scope here. Its foundation document is
`2026-11-23-vulkan-backend-design.md`.

## Goals

- Add `CORE_43` and `ES_31` profile configs to the existing `GLContextConfig`,
  using SDL3 attributes `SDL_GL_CONTEXT_PROFILE_CORE` / `SDL_GL_CONTEXT_PROFILE_ES`
  with versions 4.3 / 3.1.
- Add GLSL 430 and GLSL ES 310 shader variants from the same shared sources as
  Tier A, via a compatibility macro layer and a profile-specific
  version/precision preamble.
- Add capability detection for compute shaders and SSBOs; use them only on the
  two new profiles.
- Add an **optional GPU particle path** that reuses Track 1's emitter config,
  pool semantics and Python API, with the CPU path as the fallback.
- Keep Tier A profiles, the legacy OpenGL backend, SDL, and both dummy backends
  working and selectable without source changes.

## Non-goals

- A new backend family. Tier B extends the Tier A classes; it does not copy
  them.
- Dropping Tier A profiles or the CPU particle path. Both remain supported and
  the CPU path remains the default.
- Vulkan, SPIR-V, ray tracing, bindless textures, or any GL 4.4+ feature.
  Direct state access (4.5) and SPIR-V ingestion (4.6) are out.
- Geometry and tessellation shaders (ES 3.2 features, useless for a 2D engine).
- macOS / Windows-on-Arm coverage through this tier: macOS caps GL at 4.1 and
  Mesa's D3D12 layer on Windows-on-Arm provides 3.3. Those platforms use Tier A
  or the legacy backend. Documented limitation, not a bug to fix here.
- Bit-for-bit identical particle sequences between CPU and GPU paths.
- Indirect draw. It was considered and dropped; `hasIndirectDraw()` is not part
  of this design.

## Anti-goals

- Do not fork shader sources into four copies. One source per program with a
  preamble/macro layer is required now that four dialects exist.
- Do not let the GPU path become mandatory. It must work only where the profile
  supports it and the emitter opts in.
- Do not change Track 1's emitter/emission API, placement semantics,
  drop-on-full policy, or CPU fallback.
- Do not add GL types to `ParticleEmitter`, `ParticleManager` or
  `ParticleRenderer`. GL-specific code stays inside `glcore/`.
- Do not add Tier B names to settings unless compiled in; follow Tier A's
  warned-SDL-fallback rule for stale config.

## Constraints and findings from the codebase

1. Tier A delivers `GLContextConfig`, the vendored glad loader,
   `ShaderProgram`/`ShaderCache`, `VertexArray`/`Buffer`, `GLCoreState`,
   `GLCoreImage`, `GLCoreGuiGraphics`, and the `addImageToArray()` /
   `renderVertexArrays()` batching path. Tier B assumes these exist.
2. Tier A's `ShaderLoader` emits two dialect preambles (GLSL 330, ESSL 100).
   Tier B needs four: GLSL 330, GLSL 430, ESSL 100, ESSL 310. Relevant
   differences are version/precision lines and the pairs
   `attribute`/`varying`/`texture2D`/`gl_FragColor` versus
   `in`/`out`/`texture`/declared fragment output. Compute shaders exist only in
   the last two dialects.
3. Track 1's `ParticleEmitter` owns the pool, free-index stack, seeded
   `std::mt19937`, emission accumulator, lifetimes, `RendererNode` placement,
   drop-on-full overflow and the 2048 default cap. `ParticleManager` ticks once
   per frame at a fixed 1/120 s step and is deliberately backend-agnostic.
4. Backend selection remains the `Engine::init()` string chain
   (`engine.cpp:393-423`) with names from `enginesettings.cpp:160-168`. Tier A
   added two names; Tier B adds two more and must extend the same four
   name-branch sites (`instancerenderer.cpp`, `layercache.cpp`, `camera.cpp`,
   `imageloader.cpp`) with OpenGL-side behaviour.
5. Tier A's CMake adds `ENABLE_OPENGL_CORE` / `ENABLE_OPENGL_ES2`, each requiring
   `ENABLE_OPENGL=ON`. Tier B adds `ENABLE_OPENGL_43` / `ENABLE_OPENGLES_31`
   with the same requirement plus at least one Tier A option, so the shared
   foundation exists.
6. CI currently forces `MESA_GL_VERSION_OVERRIDE=3.3`; a Tier B job must not
   inherit that override.

## Approaches considered

### Option A — Separate backend classes (rejected)

Copy the Tier A backend for the higher profiles. Rejected: duplicates the
state cache, batching, image and GUI code whose only differences are context
attributes and shader preambles.

### Option B — New profiles on the Tier A backend family (chosen)

Extend `GLContextConfig`, add shader variants, and select behaviour from
capability queries. Chosen: one implementation of the batching and state code
for all four dialects.

### GPU particle sub-decision

Three splits were considered:

- **Full GPU ownership** (shader emits, integrates, recycles): largest speedup,
  but reimplements Track 1's seeded sequence, drop-on-full and placement in
  GLSL and risks semantic divergence. Rejected for v1.
- **Integration only, full per-frame upload**: keeps all semantics on CPU, but
  the CPU still does O(N) work, so the particle budget barely improves.
  Rejected.
- **CPU owns emission/recycling, GPU owns integration (chosen)**: the CPU keeps
  the pool, RNG, spawn scheduling, lifetimes and placement; it uploads only
  newly spawned particles into a persistent SSBO. The compute shader integrates
  all live particles each frame. This moves the O(N) work to the GPU while
  keeping the semantics and the CPU fallback in one place.

## Design

### Context and capabilities

- `GLProfile` gains `CORE_43` and `ES_31`; `GLContext` maps them to SDL3
  attributes (4/3 core, 3/1 ES).
- Capability queries gain `hasComputeShaders()` and
  `hasShaderStorageBuffers()`, true only for the new profiles. No
  `hasIndirectDraw()`.
- Profile choice is explicit via backend name. A context that cannot be created
  falls back to SDL with a warning, as in Tier A.

### Shader dialects

- One source per program. `ShaderLoader` prepends:
  - GLSL 430: `#version 430 core`, `in`/`out`, `texture()`, explicit fragment
    output.
  - ESSL 310: `#version 310 es`, required precision qualifiers, `in`/`out`,
    `texture()`.
  - Tier A keeps GLSL 330 and ESSL 100 preambles.
- A compatibility macro header lets the shared sprite/primitive/overlay/light
  sources use one spelling for texture sampling, inputs, varyings and fragment
  output.
- **Two new programs, both compiled only for the two Tier B dialects:**
  - `particle_compute` — integrates positions, velocities, colours, sizes and
    life in an SSBO.
  - `particle_instanced` — vertex/fragment pair drawing instanced quads that
    read per-instance data from the SSBO and select atlas frames.
  Compute shaders cannot target GLSL 330 or ESSL 100; those dialects compile
  the four existing programs only.

### GPU particle path

- `ParticleEmitter` keeps its config, pool, free-index stack, seeded RNG,
  emission accumulator, `RendererNode`, drop-on-full policy and Python API. It
  gains a `computeEnabled` flag (default false).
- **CPU responsibilities:** decide spawns, assign initial attributes and
  scheduled death times, recycle particles whose scheduled death has passed,
  and upload only newly spawned particle records to the GPU SSBO at a rolling
  cursor. It never reads particle state back.
- **GPU responsibilities:** `particle_compute` integrates all live SSBO entries
  each frame from the uploaded initial state plus elapsed time;
  `particle_instanced` draws them.
- The SSBO is allocated per emitter on creation/attach with capacity from
  `maxParticles`. Reallocation or recreation happens on emitter removal,
  `maxParticles` changes, `computeEnabled` changes, or context recreation.
  Changing rates, colours, lifetimes or the seed needs no reallocation: rates
  and colours go in as uniforms, a seed change resets only the CPU RNG.
- **Cap:** `maxParticles` is clamped to 2048 on the CPU path. With
  `computeEnabled` it may be set up to a hard bound of 262144, further clamped
  by `GL_MAX_SHADER_STORAGE_BLOCK_SIZE` and available memory. Drop-on-full still
  applies; the Python-visible alive count is an estimate on this path.
- **Layer and camera handling:** the CPU keeps selecting emitters by the
  drawn-layer tracking established in Track 1, so with multiple cameras or
  layers no emitter is dispatched twice per frame. Projection and view matrices
  for the active camera are uploaded as uniforms to `particle_instanced`.
- **Backend interface:** a small abstract `IParticleRenderBackend` is declared
  in `video/` with no GL types. `RenderBackend` gains one non-pure virtual,
  `virtual IParticleRenderBackend* getParticleRenderBackend() { return nullptr; }`.
  Only `RenderBackendGLCore` overrides it, returning an internal glcore bridge
  that owns the SSBOs, dispatches and instanced draws. SDL, legacy and dummy
  backends keep the default null. `ParticleRenderer` checks for null and falls
  back to `Image::render()`; it never mentions a concrete backend. The bridge
  is created/destroyed with the GL context and handle re-creation on window
  replacement as Tier A does for other GL objects.

### Integration and build

- `EngineSettings` gains `"OpenGL_43"` and `"GLES_31"`, listed only when their
  options are enabled; `Engine::init()` gains factory branches.
- Extend the four Tier A name-branch sites with OpenGL-side behaviour for both
  names.
- `CMakeLists.txt`: add `ENABLE_OPENGL_43` and `ENABLE_OPENGLES_31`, requiring
  `ENABLE_OPENGL=ON` and at least one Tier A backend option. Add the two new
  shader sources only in those builds.
- CI: a desktop 4.3 job without the 3.3 override, and a GLES 3.1 job where a
  context is obtainable. Validate shader compilation for all four dialects with
  the program/dialect matrix above. Keep Tier A and dummy jobs green.

### Relationship to Tier C (Vulkan)

Tier C is a genuinely separate backend family using SDL3's `SDL_Vulkan_*`
surface plumbing, with its own descriptor and pipeline management. Tier B is a
stepping stone in *capability and feature design* (compute particles), not in
GL code reuse. Nothing here should be read as making Vulkan cheaper to write;
see `2026-11-23-vulkan-backend-design.md` for the long-term backend sketch.

## Testing strategy

- Context/shader smoke tests extended to create 4.3 and 3.1 contexts and compile
  every applicable program: the four existing programs in all four dialects,
  and the two particle programs in GLSL 430 / ESSL 310 only.
- Capability assertions: Tier A profiles report no compute/SSBO support; Tier B
  profiles report both.
- CPU versus GPU particle tests for the same config and seed: assert spawn
  counts, death scheduling, approximate position ranges and centroid within a
  documented tolerance. Do **not** assert identical sequences; the GLSL RNG is
  not `std::mt19937`.
- Cap tests: 2048 clamp on CPU path, raised bound on GPU path, drop-on-full on
  both.
- Regression tests for both new backend names at all four name-branch sites and
  for the SDL fallback when a name was not compiled in.
- Keep `[dummy]` tests green; the GPU path never runs on a dummy backend.

## Effort / sizing

| Task | Estimate |
|------|----------|
| Profile configs, context attributes, capability queries | 3 days |
| Shader macro layer and four-dialect validation | 5 days |
| Engine/settings/CMake/CI integration | 4 days |
| GPU particle path (compute + instanced + bridge + cap handling) | 9 days |
| Tests and documentation | 3 days |
| **Total** | **~5 weeks** |

Smaller than Tier A's 11–12 weeks because the backend foundation already
exists. The GPU particle path is the largest item and is independently
shippable after the profiles work.

## Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| ES 3.1 context unavailable in CI | Medium | Medium | Compile-only shader validation plus `SKIP`; document |
| Shader macro layer breaks one dialect | Medium | High | CI compiles the full program/dialect matrix |
| CPU and GPU particle visuals diverge | Medium | Medium | CPU remains default; tolerance-based same-config tests |
| macOS / Win-on-Arm unsupported by this tier | High | Low | Tier A and legacy remain available; document |
| Broad GPU path destabilises CPU semantics | Medium | High | Keep emission/recycling on CPU; GPU only integrates |
| SSBO reallocation missed on runtime config change | Medium | Medium | Single bridge-owned invalidation path with tests |

## What's left to the builder

Decisions deliberately left open because a competent builder can settle them
without changing the design:

- Exact backend name strings and whether to use one option per tier instead of
  one per profile.
- Whether the shader macro layer lives in a generated header or in each
  program's preamble; one source per program is required either way.
- Whether the GPU particle path lands in the same release as the new profiles
  or immediately after.
- The exact SSBO element layout and compute dispatch dimensions.
- Whether to add a global CPU/GPU particle preference setting, or keep the
  choice strictly per emitter.
