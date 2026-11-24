# Modern OpenGL Core / OpenGL ES 2.0 Backend Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add shader-based `"OpenGL_CORE"` (GL 3.3) and `"GLES2"` (ES 2.0) render backends beside the legacy ones, with GUI support, build options, and CI coverage.

**Architecture:** A new `src/fife/video/glcore/` foundation owns context creation, vendored glad loading, shaders, buffers/VAOs, state caching and batching; `RenderBackendGL` implements the existing `RenderBackend` API and two thin subclasses select the profile. A generic `GLCoreGuiGraphics` forwards Fifechan drawing through base `RenderBackend` primitives, so no production GUI or legacy GL code needs concrete-backend casts.

**Tech Stack:** C++20, SDL3 (`SDL_GL_*`), vendored generated glad loaded through `SDL_GL_GetProcAddress`, GLSL 330 / ESSL 100 string literals, Catch2 `all_tests`, CMake options, GitHub Actions Xvfb/llvmpipe.

**Spec:** `docs/superpowers/specs/2026-11-23-modern-gl-es2-design.md`

## Global Constraints

- Backend names are exactly `"OpenGL_CORE"` and `"GLES2"`; the legacy backend keeps returning `"OpenGL"`.
- CMake options `ENABLE_OPENGL_CORE` and `ENABLE_OPENGL_ES2` are mutually exclusive, both default OFF, and **v1 requires `ENABLE_OPENGL=ON` whenever either new option is ON** (keeps `HAVE_OPENGL` defined so `GLConstants` (`renderbackend.h:42-59`) carries real `GLenum` values and a working GL backend always exists).
- Desktop core: `SDL_GL_CONTEXT_MAJOR_VERSION=3`, `SDL_GL_CONTEXT_MINOR_VERSION=3`, `SDL_GL_CONTEXT_PROFILE_MASK = SDL_GL_CONTEXT_PROFILE_CORE`, `SDL_GL_CONTEXT_FLAGS = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG` on platforms where safe.
- ES2: major 2, minor 0, `SDL_GL_CONTEXT_PROFILE_ES`; EGL setup is left to SDL3; no direct EGL calls in FIFE.
- Loader: vendored, generated glad under `src/fife/video/glcore/glad/`, initialised through `SDL_GL_GetProcAddress`; a fresh context re-initialises it. The new backend must not include `fife_opengl.h` or `GL/glew.h`; no new external package and no `vcpkg.json` change.
- Shaders: four built-in programs (sprite, primitive, overlay, light) with a GLSL 330 core preamble and an ESSL 100 preamble; embedded as string literals; no runtime shader files and no SPIR-V in v1.
- Do not add a **pure** virtual to `RenderBackend`. The three extra batching hooks (two-texture, Z, multitexture-plus-Z) are **non-pure virtuals with safe defaults** falling back to the single-texture path.
- All four name-branch sites — `instancerenderer.cpp:137,142,157,162`, `layercache.cpp:196`, `camera.cpp:1023`, `imageloader.cpp:133` — take the OpenGL side for both new names. Do **not** replace them with a single capability flag; dummy backends stay on the OpenGL side at camera/imageloader and the other side at layercache.
- A stale config selecting a backend that was not compiled in logs a warning and falls back to SDL, mirroring `engine.cpp:414-422`.
- No backend is removed or disabled: SDL, legacy OpenGL, `DUMMY_NULL`, `DUMMY_SURFACE` stay selectable. No `if (modern)` branches in legacy code; `SdlGuiGraphics`, `OpenGLGuiGraphics`, legacy `GLImage` upload paths and `Window::toSDLFlags()` stay untouched.
- C++20; one Catch2 `all_tests` binary; context-dependent tests use the existing `try/catch(SDLException)` + `SKIP` pattern (`test_gui.cpp:99-103,145-165`); `all_tests_dummy` (`[dummy]` tag) stays green throughout.
- CI: extend the existing GL job with a core-profile configuration and add an ES2 job; if no reliable ES2 context can be obtained, compile and run shader-validation tests only and document the limitation.
- No particle-specific code, no GL 4.x-only feature, no compute; Track 1 stays independent; keep shader sources and context config structured so Tier B can add profile variants without rewriting batching/state.
- Verification commands below use `BUILD=out/build/glcore-core` (configured with `-DENABLE_OPENGL=ON -DENABLE_OPENGL_CORE=ON -DENABLE_OPENGL_ES2=OFF -DBUILD_TESTS=ON -DBUILD_PYTHON_BINDINGS=OFF`) and `BUILD_ES2=out/build/glcore-es2` (ES2 ON instead).

## Review Focus

- One of the four backend-name branch sites is missed, so the new names get CPU sorting / unflipped render-target clip / wrong upload pixel format and content silently breaks — pinned by the predicate truth-table test in Task 9.
- A non-pure batching default is missing or wrong, so two-texture, Z and multitexture-Z `Image::render()` calls silently vanish on SDL/dummy while legacy overrides are broken — pinned by `CountingSurfaceBackend` and the legacy-override pixel test in Task 4.
- A stale config naming a compiled-out backend asserts, throws, or selects the wrong backend instead of warning and falling back to SDL — pinned by `resolveCompiledBackendName()` tests in Task 9.
- `GLCoreGuiGraphics` misses an `fcn::Graphics` virtual or `FifechanManager::resizeTopContainer()` casts to the wrong concrete graphics class, crashing or dropping GUI drawing — pinned by the all-virtuals smoke test and manager dispatch/resize test in Task 8.
- A core/ES2 path difference (`GL_QUADS`, ESSL precision, NPOT wrap/mipmaps) yields a black screen or link failure — pinned by the dual-preamble compile test (Tasks 2 and 10), the indexed-quad pixel test (Task 3) and the ES2 NPOT test (Task 10).

---

## File Structure

New under `src/fife/video/glcore/` (all new code lives here; no `fife_opengl.h`/GLEW includes; each task adds its new `.cpp`/`.h` to `FIFE_GLCORE_SOURCES`/`FIFE_GLCORE_HEADERS` in `CMakeLists.txt`):

- `glcontext.h/.cpp` — `GLProfile`, `GLContextConfig`, profile factories, SDL attribute application, context create/make-current/swap/destroy, drawable-size and capability queries.
- `glad/glcore_loader.h/.cpp` — single facade `glcoreLoadProcs(GLProfile)` over the vendored generated loaders.
- `glad/glad_core.h/.c`, `glad/glad_es2.h/.c` — checked-in generated GL 3.3 core and GLES2 loaders; no external package.
- `shader.h/.cpp` — `GLCoreShader` compilation/linking and cached uniform locations.
- `shaderloader.h/.cpp` — `GLCoreProgram`, `ShaderLoader`, the four built-in program sources and `glcorePreamble()`.
- `buffer.h/.cpp` — `GLCoreBuffer` VBO/IBO wrapper.
- `vertexarray.h/.cpp` — `GLCoreVertexArray` VAO wrapper and the shared indexed-quad IBO.
- `glcorestate.h/.cpp` — `GLCoreState` deduplicating state cache and explicit projection matrix.
- `renderbackendgl.h/.cpp` — `RenderBackendGL` shared implementation plus thin `RenderBackendGLCore` and `RenderBackendGLES2` specialisations.
- `glcoreimage.h/.cpp` — `GLCoreImage`, atlas UV helper, texture upload/fallbacks, FBO targets, `captureScreen` support.
- `glcore_names.h/.cpp` — the four backend-name predicates, `resolveCompiledBackendName()`, compiled-name lists.

New under `src/fife/gui/fifechan/base/glcore/`:

- `glcore_gui_graphics.h/.cpp` — `GLCoreGuiGraphics`, generic `fcn::Graphics` adapter holding only `RenderBackend*`.

New tests under `tests/core_tests/`: `test_glcore_context.cpp`, `test_glcore_shaders.cpp`, `test_glcore_buffers.cpp`, `test_glcore_hooks.cpp`, `test_glcore_state.cpp`, `test_glcore_primitives.cpp`, `test_glcore_image.cpp`, `test_glcore_gui.cpp`, `test_glcore_selection.cpp`, `test_glcore_es2.cpp`, `test_glcore_validation.cpp`.

Modified existing files:

- `CMakeLists.txt` — options, mutual-exclusion/`ENABLE_OPENGL` enforcement, `HAVE_OPENGL_CORE`/`HAVE_OPENGL_ES2` definitions, conditional source/header groups, glad includes, OpenGL::GL vs GLES/EGL links.
- `tests/core_tests/CMakeLists.txt` — add each new test source and link the new GL/GLES libraries.
- `src/fife/video/renderbackend.h/.cpp` — three non-pure batching hooks with safe defaults only.
- `src/fife/controller/engine.cpp` — factory branches, stale-name resolution, `winSettings.opengl = true` for both new backends.
- `src/fife/controller/enginesettings.cpp` — conditional `getPossibleRenderBackends()` entries.
- `src/fife/gui/fifechan/fifechanmanager.cpp` — `init()` and `resizeTopContainer()` branches for both new names.
- `src/fife/view/renderers/instancerenderer.cpp`, `src/fife/view/layercache.cpp`, `src/fife/view/camera.cpp`, `src/fife/loaders/native/video/imageloader.cpp` — replace inline name tests with `glcore_names.h` predicates.
- `.github/workflows/build-on-linux.yml` — core-profile configuration and ES2 job.
- `docs/glcore-backends.md` — build flags, selection strings, ES2 CI limitation notes.

---

### Task 1: Foundation — build options, context configuration and glad loader

**Files:**
- Create: `src/fife/video/glcore/glcontext.h`, `src/fife/video/glcore/glcontext.cpp`, `src/fife/video/glcore/glad/glcore_loader.h`, `src/fife/video/glcore/glad/glcore_loader.cpp`, `src/fife/video/glcore/glad/glad_core.h`, `src/fife/video/glcore/glad/glad_core.c`, `src/fife/video/glcore/glad/glad_es2.h`, `src/fife/video/glcore/glad/glad_es2.c`
- Modify: `CMakeLists.txt:88-100,162-164,1474-1478,1487-1507,1570-1590`, `tests/core_tests/CMakeLists.txt:15-51,105-107`
- Test: `tests/core_tests/test_glcore_context.cpp`

**Interfaces:**
- Consumes: `WindowSettings` / `Window::create()` and the `SKIP` pattern in `tests/core_tests/test_gui.cpp:95-110`; `HAVE_OPENGL` from `CMakeLists.txt:162-164`.
- Produces:

```cpp
namespace FIFE {
enum class GLProfile : uint8_t { Core33, ES2 };
struct GLContextConfig {
    GLProfile profile{GLProfile::Core33};
    int32_t major{3};
    int32_t minor{3};
    bool debug{false};
    int32_t stencilBits{8};
    int32_t depthBits{24};
    bool vsync{false};
    int32_t msaaSamples{0};
};
GLContextConfig makeCore33Config();
GLContextConfig makeES2Config();
class GLContext {
public:
    explicit GLContext(GLContextConfig config);
    ~GLContext();
    GLContext(GLContext const&) = delete;
    GLContext& operator=(GLContext const&) = delete;
    void applyAttributes() const;
    bool create(SDL_Window* window);
    void makeCurrent(SDL_Window* window);
    void swap(SDL_Window* window) const;
    void destroy();
    bool isLoaded() const;
    SDL_GLContext native() const;
    GLContextConfig const& config() const;
};
void glcoreGetDrawableSize(SDL_Window* window, uint32_t& w, uint32_t& h);
bool glcoreLoadProcs(GLProfile profile);
}
```

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_context.cpp` and add it to `FIFE_CORE_TEST_SOURCES` in `tests/core_tests/CMakeLists.txt`:

```cpp
TEST_CASE("GLContextConfig pins core 3.3 and ES 2.0 values", "[glcore][context]") {
    auto core = FIFE::makeCore33Config();
    CHECK(core.profile == FIFE::GLProfile::Core33);
    CHECK(core.major == 3); CHECK(core.minor == 3); CHECK(core.stencilBits == 8);
    auto es = FIFE::makeES2Config();
    CHECK(es.profile == FIFE::GLProfile::ES2);
    CHECK(es.major == 2); CHECK(es.minor == 0);
}
TEST_CASE("GLContext creates a core 3.3 context through SDL", "[glcore][context]") {
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) { SKIP("SDL video unavailable"); }
    FIFE::Window window;
    try { window.create({.width = 64, .height = 64, .opengl = true, .windowMode = FIFE::WindowMode::Windowed}); }
    catch (FIFE::SDLException const&) { SKIP("OpenGL not available in this environment"); }
    FIFE::GLContext ctx(FIFE::makeCore33Config());
    ctx.applyAttributes();
    int value = 0;
    REQUIRE(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value)); CHECK(value == 3);
    REQUIRE(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value)); CHECK(value == 3);
    REQUIRE(SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &value));
    CHECK(value == SDL_GL_CONTEXT_PROFILE_CORE);
    REQUIRE(ctx.create(window.getSDLWindow()));
    CHECK(ctx.isLoaded());
    ctx.destroy();
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake -S . -B out/build/glcore-core -DENABLE_OPENGL=ON -DENABLE_OPENGL_CORE=ON -DENABLE_OPENGL_ES2=OFF -DBUILD_TESTS=ON -DBUILD_PYTHON_BINDINGS=OFF && cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][context]"`
Expected: FAIL to compile with `fatal error: video/glcore/glcontext.h: No such file or directory`.

- [ ] **Step 3: Implement the option block and context/loader in the listed files**

In `CMakeLists.txt`: add `option(ENABLE_OPENGL_CORE ... OFF)`, `option(ENABLE_OPENGL_ES2 ... OFF)` next to `ENABLE_OPENGL`; `message(FATAL_ERROR ...)` when both are ON or when either is ON with `ENABLE_OPENGL=OFF`; define `HAVE_OPENGL_CORE` / `HAVE_OPENGL_ES2` via `fife_options`; append `FIFE_GLCORE_SOURCES`/`FIFE_GLCORE_HEADERS` to `fife_lib` sources and the public header set under the guards; add the glad include dir and link `OpenGL::GL` for core and the found GLES/EGL libraries for ES2. Leave legacy GL/GLEW sources in the unconditional lists. Generate glad checked-in with the project's SDL3 GL headers; `glcoreLoadProcs()` calls the core or ES2 generated loader with `SDL_GL_GetProcAddress` and returns its boolean result. `applyAttributes()` sets exactly the attributes in Global Constraints; `create()` calls `SDL_GL_LoadLibrary`/`SDL_GL_CreateContext` then `glcoreLoadProcs`, and returns false on failure.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][context]"`
Expected: PASS (or SKIP with "OpenGL not available in this environment" when no context exists).

- [ ] **Step 5: Commit**

```bash
git add CMakeLists.txt tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_context.cpp src/fife/video/glcore/
git commit -m "feat(glcore): add build options, context abstraction and glad loader"
```

### Task 2: Foundation — shader wrappers and four built-in programs

**Files:**
- Create: `src/fife/video/glcore/shader.h`, `src/fife/video/glcore/shader.cpp`, `src/fife/video/glcore/shaderloader.h`, `src/fife/video/glcore/shaderloader.cpp`
- Modify: `tests/core_tests/CMakeLists.txt`
- Test: `tests/core_tests/test_glcore_shaders.cpp`

**Interfaces:**
- Consumes: `GLProfile`, `GLContext`, `glcoreLoadProcs()` from Task 1.
- Produces:

```cpp
namespace FIFE {
enum class GLCoreProgram : uint8_t { Sprite, Primitive, Overlay, Light };
char const* glcorePreamble(GLProfile profile);
class GLCoreShader {
public:
    GLCoreShader(GLProfile profile, char const* vsSource, char const* fsSource);
    ~GLCoreShader();
    GLCoreShader(GLCoreShader const&) = delete;
    GLCoreShader& operator=(GLCoreShader const&) = delete;
    uint32_t program() const;
    bool isLinked() const;
    int32_t uniform(char const* name) const;
    void bind() const;
};
class ShaderLoader {
public:
    explicit ShaderLoader(GLProfile profile);
    ~ShaderLoader();
    GLCoreShader const& get(GLCoreProgram program) const;
    int32_t uniform(GLCoreProgram program, char const* name) const;
};
}
```

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_shaders.cpp`, add it to `FIFE_CORE_TEST_SOURCES`:

```cpp
TEST_CASE("Built-in shader preambles pin GLSL 330 and ESSL 100", "[glcore][shader]") {
    CHECK_THAT(std::string(FIFE::glcorePreamble(FIFE::GLProfile::Core33)),
               Catch::StartsWith("#version 330 core"));
    CHECK_THAT(std::string(FIFE::glcorePreamble(FIFE::GLProfile::ES2)),
               Catch::StartsWith("#version 100"));
    CHECK(std::string(FIFE::glcorePreamble(FIFE::GLProfile::ES2))
              .find("precision mediump float;") != std::string::npos);
}
TEST_CASE("All four built-in programs compile and link", "[glcore][shader]") {
    // create a core 3.3 window/context with try/catch(SDLException) + SKIP as in Task 1
    FIFE::ShaderLoader loader(FIFE::GLProfile::Core33);
    for (auto p : {FIFE::GLCoreProgram::Sprite, FIFE::GLCoreProgram::Primitive,
                   FIFE::GLCoreProgram::Overlay, FIFE::GLCoreProgram::Light}) {
        CHECK(loader.get(p).isLinked());
        CHECK(loader.uniform(p, "uProjection") >= 0);
    }
    CHECK(loader.uniform(FIFE::GLCoreProgram::Sprite, "uTexture0") >= 0);
    CHECK(loader.uniform(FIFE::GLCoreProgram::Sprite, "uAlphaTest") >= 0);
    CHECK(loader.uniform(FIFE::GLCoreProgram::Overlay, "uTexture1") >= 0);
    CHECK(loader.uniform(FIFE::GLCoreProgram::Overlay, "uOverlayMode") >= 0);
    CHECK(loader.uniform(FIFE::GLCoreProgram::Light, "uIntensity") >= 0);
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][shader]"`
Expected: FAIL to compile (`video/glcore/shaderloader.h: No such file or directory`).

- [ ] **Step 3: Implement `GLCoreShader`, `ShaderLoader`, `glcorePreamble(GLProfile)` in the listed files**

Embed the four program pairs in `shaderloader.cpp` as string literals prefixed with `glcorePreamble(profile)`. Bind the vertex-position attribute `aPosition` to location 0 before linking (`glBindAttribLocation`; ESSL 100 has no layout qualifiers, and Task 3 binds location 0). Uniform contracts: every program has `uProjection`; Sprite adds `uTexture0`, `uColor`, `uAlphaTest` (alpha test via `discard`); Primitive adds `uColor`; Overlay adds `uTexture0`, `uTexture1`, `uFactor`, `uOverlayMode` for the four `OverlayType` values; Light adds `uColor`, `uIntensity`. Cache locations in `uniform()` at construction. No runtime file I/O.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][shader]"`
Expected: PASS (context-dependent case SKIPs where unavailable).

- [ ] **Step 5: Commit**

```bash
git add src/fife/video/glcore/shader.h src/fife/video/glcore/shader.cpp src/fife/video/glcore/shaderloader.h src/fife/video/glcore/shaderloader.cpp tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_shaders.cpp
git commit -m "feat(glcore): add shader wrappers and built-in programs"
```

### Task 3: Foundation — VBO/IBO/VAO wrappers and the first indexed textured quad

**Files:**
- Create: `src/fife/video/glcore/buffer.h`, `src/fife/video/glcore/buffer.cpp`, `src/fife/video/glcore/vertexarray.h`, `src/fife/video/glcore/vertexarray.cpp`
- Modify: `tests/core_tests/CMakeLists.txt`
- Test: `tests/core_tests/test_glcore_buffers.cpp`

**Interfaces:**
- Consumes: `GLContext` from Task 1; `ShaderLoader`/`GLCoreProgram::Sprite` from Task 2.
- Produces:

```cpp
namespace FIFE {
class GLCoreBuffer {
public:
    enum class Kind : uint8_t { Array, Element };
    GLCoreBuffer(Kind kind, uint32_t usage);
    ~GLCoreBuffer();
    GLCoreBuffer(GLCoreBuffer const&) = delete;
    GLCoreBuffer& operator=(GLCoreBuffer const&) = delete;
    void upload(void const* data, size_t byteCount);
    void bind() const;
    uint32_t name() const;
};
class GLCoreVertexArray {
public:
    GLCoreVertexArray();
    ~GLCoreVertexArray();
    GLCoreVertexArray(GLCoreVertexArray const&) = delete;
    GLCoreVertexArray& operator=(GLCoreVertexArray const&) = delete;
    void bindAttribute(uint32_t location, int32_t componentCount, size_t stride, size_t offset);
    void bind() const;
    static void unbind();
    uint32_t name() const;
};
uint32_t glcoreQuadIndexBuffer(); // lazily created shared IBO {0,1,2,0,2,3}
}
```

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_buffers.cpp`, add it to `FIFE_CORE_TEST_SOURCES`:

```cpp
TEST_CASE("Indexed quad renders with the shared index buffer in core profile", "[glcore][buffer]") {
    // core 3.3 window/context with SKIP as in Task 1; ShaderLoader Sprite bound
    CHECK(FIFE::glcoreQuadIndexBuffer() != 0u);
    float const vertices[] = {-0.5f,-0.5f, 0.5f,-0.5f, 0.5f,0.5f, -0.5f,0.5f};
    FIFE::GLCoreBuffer vbo(FIFE::GLCoreBuffer::Kind::Array, 0x88E4u /* GL_STATIC_DRAW */);
    vbo.upload(vertices, sizeof(vertices));
    FIFE::GLCoreVertexArray vao;
    vao.bindAttribute(0, 2, 0, 0);
    // draw indexed triangles; glReadPixels centre pixel
    CHECK(centrePixel.r == 255); CHECK(centrePixel.a == 255);
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][buffer]"`
Expected: FAIL to compile (`video/glcore/buffer.h: No such file or directory`).

- [ ] **Step 3: Implement the wrappers and shared IBO in the listed files**

`GLCoreBuffer` owns one GL buffer name, created on construction and destroyed in the destructor; `upload()` uses `glBufferData`. `GLCoreVertexArray` owns one VAO name and configures enabled attributes with `glVertexAttribPointer`/`glEnableVertexAttribArray`. `glcoreQuadIndexBuffer()` returns a lazily created static IBO with six `uint32_t` indices (0,1,2,0,2,3) — indexed triangles only; `GL_QUADS` must not appear in any new source.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][buffer]"`
Expected: PASS (or SKIP without a context).

- [ ] **Step 5: Commit**

```bash
git add src/fife/video/glcore/buffer.h src/fife/video/glcore/buffer.cpp src/fife/video/glcore/vertexarray.h src/fife/video/glcore/vertexarray.cpp tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_buffers.cpp
git commit -m "feat(glcore): add buffer and VAO wrappers with indexed quad"
```

### Task 4: Feature parity — non-pure batching hooks on `RenderBackend`

**Files:**
- Modify: `src/fife/video/renderbackend.h:215-238`, `src/fife/video/renderbackend.cpp`
- Test: `tests/core_tests/test_glcore_hooks.cpp`

**Interfaces:**
- Consumes: existing `virtual void addImageToArray(uint32_t id, Rect const& rec, float const* st, uint8_t alpha, uint8_t const* rgba)` at `renderbackend.h:223-224`; legacy overrides at `renderbackendopengl.h:61-80` (already `virtual`, so they will override automatically).
- Produces:

```cpp
virtual void addImageToArray(Rect const& rect, uint32_t id1, float const* st1,
                             uint32_t id2, float const* st2, uint8_t alpha, uint8_t const* rgba);
virtual void addImageToArrayZ(uint32_t id, Rect const& rect, float vertexZ,
                              float const* st, uint8_t alpha, uint8_t const* rgba);
virtual void addImageToArrayZ(Rect const& rect, float vertexZ, uint32_t id1, float const* st1,
                              uint32_t id2, float const* st2, uint8_t alpha, uint8_t const* rgba);
```

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_hooks.cpp`, add it to `FIFE_CORE_TEST_SOURCES` (tagged `[dummy]` for the headless part):

```cpp
class CountingSurfaceBackend final : public FIFE::RenderBackendSurface {
public:
    using FIFE::RenderBackendSurface::RenderBackendSurface;
    int calls = 0; uint32_t lastId = 0;
    void addImageToArray(uint32_t id, FIFE::Rect const&, float const*, uint8_t, uint8_t const*) override {
        ++calls; lastId = id;
    }
};
TEST_CASE("Batching hook defaults fall back to the first texture", "[dummy][glcore][hooks]") {
    CountingSurfaceBackend backend({0,0,0,255}, 64, 64);
    FIFE::Rect const rect(0,0,8,8); float st[4]{0,0,1,1}; uint8_t rgba[4]{255,255,255,255};
    backend.addImageToArray(rect, 11u, st, 22u, st, 255, rgba);
    CHECK(backend.calls == 1); CHECK(backend.lastId == 11u);
    backend.addImageToArrayZ(33u, rect, 0.5f, st, 255, rgba);
    CHECK(backend.calls == 2); CHECK(backend.lastId == 33u);
    backend.addImageToArrayZ(rect, 0.5f, 44u, st, 55u, st, 255, rgba);
    CHECK(backend.calls == 3); CHECK(backend.lastId == 44u);
}
TEST_CASE("Legacy OpenGL overrides of the batching hooks still draw", "[glcore][hooks]") {
    // core/compat context with SKIP; call addImageToArrayZ(id, rect, 0.5f, st, 255, rgba),
    // renderVertexArrays(), then glReadPixels at the quad centre
    CHECK(centrePixel.a > 0);
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][hooks]"`
Expected: FAIL to compile — `no member named 'addImageToArrayZ' in 'FIFE::RenderBackendSurface'`.

- [ ] **Step 3: Implement the three declarations and their safe defaults**

Add the declarations to `RenderBackend` as **non-pure** virtuals; define them in `renderbackend.cpp` so each forwards to `addImageToArray(id, rect, st, alpha, rgba)` using the first texture id (`id1`/`id`) and ignoring the second texture and `vertexZ`. Do not add any pure virtual, and do not edit `renderbackendopengl.h/.cpp` or `glimage.cpp`.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][hooks]" && ./out/build/glcore-core/tests/core_tests/all_tests "[dummy]"`
Expected: PASS; the `[dummy]` subset (including `all_tests_dummy`) stays green.

- [ ] **Step 5: Commit**

```bash
git add src/fife/video/renderbackend.h src/fife/video/renderbackend.cpp tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_hooks.cpp
git commit -m "feat(glcore): add non-pure batching hooks with safe defaults"
```

### Task 5: Feature parity — `GLCoreState` cache and backend lifecycle/state mapping

**Files:**
- Create: `src/fife/video/glcore/glcorestate.h`, `src/fife/video/glcore/glcorestate.cpp`, `src/fife/video/glcore/renderbackendgl.h`, `src/fife/video/glcore/renderbackendgl.cpp`
- Modify: `CMakeLists.txt`, `tests/core_tests/CMakeLists.txt`
- Test: `tests/core_tests/test_glcore_state.cpp`

**Interfaces:**
- Consumes: all Task 1–3 types; `GLConstants`, `OverlayType`, `RenderDataType` (`renderbackend.h:42-102`); legacy lifecycle shape (`renderbackendopengl.cpp:270-395`).
- Produces:

```cpp
namespace FIFE {
class GLCoreState {
public:
    GLCoreState();
    void reset();
    void useProgram(uint32_t program);
    void bindTexture(uint32_t unit, uint32_t texture);
    void setBlend(uint32_t src, uint32_t dst);
    void setDepthTest(bool enabled, uint32_t func);
    void setScissor(FIFE::Rect const& area, bool enabled);
    void setStencil(bool enabled, uint8_t ref, uint8_t op, uint8_t func);
    void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    void setProjection(float left, float right, float top, float bottom, float nearZ, float farZ);
    float const* projection() const;
};
class RenderBackendGL : public RenderBackend {
public:
    RenderBackendGL(SDL_Color const& colorkey, GLContextConfig config);
    ~RenderBackendGL() override;
    // overrides every pure virtual in renderbackend.h with its exact signature
};
class RenderBackendGLCore final : public RenderBackendGL {
public:
    explicit RenderBackendGLCore(SDL_Color const& colorkey); // makeCore33Config(), name "OpenGL_CORE"
};
class RenderBackendGLES2 final : public RenderBackendGL {
public:
    explicit RenderBackendGLES2(SDL_Color const& colorkey); // makeES2Config(), name "GLES2"
};
}
```

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_state.cpp`, add it to `FIFE_CORE_TEST_SOURCES`:

```cpp
TEST_CASE("GLCoreState builds the orthographic projection used by the legacy backend", "[glcore][state]") {
    FIFE::GLCoreState state;
    state.setProjection(0.0f, 100.0f, 75.0f, 0.0f, -100.0f, 100.0f);
    CHECK(state.projection()[0] == Approx(0.02f));   // 2/(right-left)
    CHECK(state.projection()[5] == Approx(-0.0266667f)); // -2/(top-bottom)
    CHECK(state.projection()[10] == Approx(-0.01f));
}
TEST_CASE("Backend names and profile configs are stable", "[glcore][state]") {
    FIFE::RenderBackendGLCore core({0,0,0,255});
    FIFE::RenderBackendGLES2 es({0,0,0,255});
    CHECK(core.getName() == "OpenGL_CORE");
    CHECK(es.getName() == "GLES2");
    CHECK(core.getWindow() == nullptr);
    CHECK(core.getBaseWidth() == 1280); // RenderBackend ctor default, renderbackend.cpp:44
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][state]"`
Expected: FAIL to compile (`video/glcore/glcorestate.h: No such file or directory`).

- [ ] **Step 3: Implement the state cache and shared backend shell in the listed files**

`GLCoreState` stores the current program, four texture units, blend func, depth/scissor/stencil state, viewport and a 4×4 projection, and skips redundant GL calls. `RenderBackendGL` holds `GLContext`, `ShaderLoader`, `GLCoreState`, an image/VBO batch buffer and a render-target image; it implements `init`, `createMainScreen` (window replacement destroys and recreates all GL objects; `glcoreGetDrawableSize()` gives the viewport; explicit projection replaces `glOrtho`), `endFrame`, `clearBackBuffer`, `setScalingMode`, `setClipArea`, `changeBlending`, `changeRenderInfos`, lighting setters, `resetStencilBuffer`, scissor toggles and `captureScreen` (`glReadPixels` → `SDL_Surface` → `Image::saveAsPng`). `changeRenderInfos()` maps `RenderDataType`/`OverlayType`/`GLConstants` onto the cache with `LightRenderer` unchanged. Use `OpenGL::GL`/GLES link targets already added in Task 1.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][state]"`
Expected: PASS.

- [ ] **Step 5: Commit**

```bash
git add src/fife/video/glcore/glcorestate.* src/fife/video/glcore/renderbackendgl.* CMakeLists.txt tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_state.cpp
git commit -m "feat(glcore): add state cache and shared backend shell"
```

### Task 6: Feature parity — primitives, overlays and stencil lighting

**Files:**
- Modify: `src/fife/video/glcore/renderbackendgl.h`, `src/fife/video/glcore/renderbackendgl.cpp`
- Modify: `tests/core_tests/CMakeLists.txt`
- Test: `tests/core_tests/test_glcore_primitives.cpp`

**Interfaces:**
- Consumes: `GLCoreState`, `RenderBackendGL`, `ShaderLoader` (Tasks 2 and 5); the exact primitive signatures in `renderbackend.h:327-457`, `renderGuiGeometry()` at `:649-653`, and `GuiVertex` at `:104-110`.
- Produces: `RenderBackendGL` overrides of `putPixel`, `drawLine`, `drawThickLine`, `drawPolyLine`, `drawBezier`, `drawTriangle`, `fillTriangle`, `drawRectangle`, `fillRectangle`, `drawQuad`, `drawVertex`, `drawCircle`, `drawFillCircle`, `drawCircleSegment`, `drawFillCircleSegment`, `drawLightPrimitive`, `renderGuiGeometry`, plus `void prepareForOverlays();` — all with the base-header signatures.

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_primitives.cpp`, add it to `FIFE_CORE_TEST_SOURCES`:

```cpp
TEST_CASE("GLCore primitives draw self-consistent pixels", "[glcore][primitive]") {
    // core 3.3 backend with SKIP; clear, draw, glReadPixels helper readPixel(x,y)
    backend.drawLine({10,10}, {50,10}, 255, 0, 0);
    backend.renderVertexArrays();
    CHECK(readPixel(30,10).r == 255);
    backend.fillTriangle({5,20}, {40,20}, {20,50}, 0, 255, 0);
    backend.renderVertexArrays();
    CHECK(readPixel(20,30).g == 255);
    backend.drawFillCircle({60,60}, 12, 0, 0, 255);
    backend.renderVertexArrays();
    CHECK(readPixel(60,60).b == 255);
}
TEST_CASE("Overlay modes and stencil light change output deterministically", "[glcore][primitive]") {
    // for each OverlayType in {COLOR, COLOR_AND_TEXTURE, TEXTURES_AND_FACTOR}:
    //   backend.changeRenderInfos(..., otype); draw two images; read centre pixel;
    //   CHECK(pixel != singleTextureBaseline); CHECK(pixel == repeatRunPixel);
    // light: changeRenderInfos with stenref/stenop/stenfunc GLConstants values and
    //        drawLightPrimitive(); CHECK(centrePixel != noLightBaseline);
}
TEST_CASE("renderGuiGeometry forwards indexed GUI vertices", "[glcore][primitive]") {
    // two triangles worth of GuiVertex + indices + translation + GLCoreImage texture
    // CHECK(readPixel(...) != background);
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][primitive]"`
Expected: FAIL to link — undefined references to `RenderBackendGL::drawLine` and the other primitive overrides (test constructs a concrete backend and calls them).

- [ ] **Step 3: Implement the primitive overrides in `renderbackendgl.cpp`**

Build vertex arrays in colour and primitive programs, submit through the dynamic VBO/IBO created in Task 5 and flush on `renderVertexArrays()`. Replace `GL_QUADS` with the shared quad IBO from Task 3. Implement `prepareForOverlays()` with shader uniforms `uTexture1`, `uFactor`, `uOverlayMode` for the four `OverlayType` values instead of the three `GL_COMBINE` blocks. Map lighting/stencil through `GLCoreState` using `GLConstants` values so `LightRenderer` needs no change.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][primitive]"`
Expected: PASS (or SKIP without a context).

- [ ] **Step 5: Commit**

```bash
git add src/fife/video/glcore/renderbackendgl.h src/fife/video/glcore/renderbackendgl.cpp tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_primitives.cpp
git commit -m "feat(glcore): implement primitives, overlays and stencil lighting"
```

### Task 7: Feature parity — `GLCoreImage`, atlas UVs, render targets and VBO batching contract

**Files:**
- Create: `src/fife/video/glcore/glcoreimage.h`, `src/fife/video/glcore/glcoreimage.cpp`
- Modify: `src/fife/video/glcore/renderbackendgl.h`, `src/fife/video/glcore/renderbackendgl.cpp`, `tests/core_tests/CMakeLists.txt`
- Test: `tests/core_tests/test_glcore_image.cpp`

**Interfaces:**
- Consumes: `Image` (`image.h:31-237`), `GLCoreBuffer`/`GLCoreVertexArray`, `RenderBackendGL`, the three base hooks from Task 4; legacy half-pixel formula (`glimage.cpp:776-799`).
- Produces:

```cpp
namespace FIFE {
std::array<float,4> glcoreAtlasTexCoords(Rect const& region, uint32_t width, uint32_t height, bool halfPixel);
class GLCoreImage final : public Image {
public:
    explicit GLCoreImage(IResourceLoader* loader = nullptr);
    explicit GLCoreImage(std::string const& name, IResourceLoader* loader = nullptr);
    explicit GLCoreImage(SDL_Surface* surface);
    GLCoreImage(std::string const& name, SDL_Surface* surface);
    explicit GLCoreImage(uint8_t const* data, uint32_t width, uint32_t height);
    GLCoreImage(std::string const& name, uint8_t const* data, uint32_t width, uint32_t height);
    ~GLCoreImage() override;
    void invalidate() override;
    void setSurface(SDL_Surface* surface) override;
    void render(Rect const& rect, uint8_t alpha = 255, uint8_t const* rgb = nullptr) override;
    void render(Rect const& rect, ImagePtr const& overlay, uint8_t alpha = 255, uint8_t const* rgb = nullptr) override;
    void renderZ(Rect const& rect, float vertexZ, uint8_t alpha = 255, uint8_t const* rgb = nullptr) override;
    void renderZ(Rect const& rect, float vertexZ, ImagePtr const& overlay,
                 uint8_t alpha = 255, uint8_t const* rgb = nullptr) override;
    void renderZ(Rect const& rect, float vertexZ, uint8_t alpha = 255,
                 bool forceNewBatch = false, uint8_t const* rgb = nullptr) override;
    void updateTexture(uint8_t const* rgba, uint32_t width, uint32_t height) override;
    void useSharedImage(ImagePtr const& shared, Rect const& region) override;
    void forceLoadInternal() override;
    void load() override;
    void free() override;
};
}
```

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_image.cpp`, add it to `FIFE_CORE_TEST_SOURCES`:

```cpp
TEST_CASE("Atlas texel correction matches the legacy half-pixel rule", "[glcore][image]") {
    auto corrected = FIFE::glcoreAtlasTexCoords({2,3,4,5}, 16, 16, true);
    CHECK(corrected[0] == Approx(2.5f/16)); CHECK(corrected[1] == Approx(3.5f/16));
    CHECK(corrected[2] == Approx(5.5f/16)); CHECK(corrected[3] == Approx(7.5f/16));
    auto raw = FIFE::glcoreAtlasTexCoords({2,3,4,5}, 16, 16, false);
    CHECK(raw[2] == Approx(6.0f/16)); CHECK(raw[3] == Approx(8.0f/16));
}
TEST_CASE("FBO render target and captureScreen work on GLCore", "[glcore][image]") {
    // SKIP without context; attachRenderTarget(image32x32,true), draw red rect,
    // detach, render image to screen, CHECK(readPixel centre .r == 255);
    // backend.captureScreen(path); CHECK(std::filesystem::exists(path));
}
TEST_CASE("Image::render batch count matches the legacy backend", "[glcore][image]") {
    // core context + legacy context (each SKIP-guarded)
    // CountingGLCore : RenderBackendGL (constructed with makeCore33Config()) overrides
    //   addImageToArray(uint32_t,...) and renderVertexArrays() with counters;
    //   CountingLegacy : RenderBackendOpenGL likewise.
    // draw five GLCoreImages with img.render(rect); backend.renderVertexArrays();
    // CHECK(core.addCalls == 5); CHECK(core.flushCalls == 1);
    // repeat with legacy images; CHECK(legacy.addCalls == core.addCalls);
    // CHECK(legacy.flushCalls == core.flushCalls);
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][image]"`
Expected: FAIL to compile (`video/glcore/glcoreimage.h: No such file or directory`).

- [ ] **Step 3: Implement `GLCoreImage` and wire the batching/render-target path**

`GLCoreImage` reuses the base SDL surface handling, owns one GL texture and four UVs in the same corner layout as `GLImage`; `glcoreAtlasTexCoords()` applies the half-pixel correction when filtering or mipmapping is enabled and plain division otherwise. `render()`/`renderZ()` call the three inherited base hooks through `RenderBackend::instance()` — no concrete-backend casts. `RenderBackendGL::addImageToArray()` and `renderVertexArrays()` append to a dynamic VBO/IBO keyed by texture and blend mode and flush in one draw per batch; `forceNewBatch` flushes first. Implement `attachRenderTarget`/`detachRenderTarget` with core FBO entry points, keep the compressed-texture workaround desktop-only, and implement `createImage()` overloads returning `GLCoreImage`.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][image]"`
Expected: PASS (or SKIP without a context).

- [ ] **Step 5: Commit**

```bash
git add src/fife/video/glcore/glcoreimage.* src/fife/video/glcore/renderbackendgl.h src/fife/video/glcore/renderbackendgl.cpp tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_image.cpp
git commit -m "feat(glcore): add GLCoreImage, FBO targets and VBO batching"
```

### Task 8: Integration — generic Fifechan adapter and manager dispatch

**Files:**
- Create: `src/fife/gui/fifechan/base/glcore/glcore_gui_graphics.h`, `src/fife/gui/fifechan/base/glcore/glcore_gui_graphics.cpp`
- Modify: `src/fife/gui/fifechan/fifechanmanager.cpp:210-218,259-282`, `CMakeLists.txt:517-533,754-770`, `tests/core_tests/CMakeLists.txt`
- Test: `tests/core_tests/test_glcore_gui.cpp`

**Interfaces:**
- Consumes: `RenderBackendGL` / `RenderBackendGLES2` (Task 5), `Image::render()` (Task 7), `DummyGuiGraphics` override set (`dummy_gui_graphics.h:45-82`) as the template.
- Produces:

```cpp
namespace FIFE {
class GLCoreGuiGraphics final : public fcn::Graphics {
public:
    GLCoreGuiGraphics();
    void updateTarget();
    // all 14 fcn::Graphics pure virtuals, mirroring dummy_gui_graphics.h:45-82
    // (drawImage, drawPoint, both drawLine overloads, drawPolyLine, drawBezier,
    //  drawRectangle, fillRectangle, drawCircle, drawFillCircle, drawCircleSegment,
    //  drawFillCircleSegment, fillTriangle, _beginDraw, _endDraw, pushClipArea,
    //  popClipArea, setColor, plus drawSurface and getColor as in the dummy adapter)
private:
    RenderBackend* m_renderbackend; // never a concrete cast
};
}
```

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_gui.cpp`, add it to `FIFE_CORE_TEST_SOURCES`:

```cpp
TEST_CASE("FifechanManager dispatches and resizes both new backend names", "[glcore][gui]") {
    for (auto const* name : {"OpenGL_CORE", "GLES2"}) {
        FIFE::RenderBackendSurface backend({0,0,0,255}, 160, 90); // headless stand-in for the singleton
        backend.init(""); backend.createMainScreen("headless", "");
        auto manager = std::make_unique<FIFE::FifechanManager>();
        manager->setConsoleEnabled(false);
        CHECK_NOTHROW(manager->init(name, 160, 90));
        CHECK_NOTHROW(manager->resizeTopContainer(0, 0, 160, 90));
        CHECK(manager->getFifechanGUI() != nullptr);
    }
}
TEST_CASE("GLCoreGuiGraphics implements every drawing virtual", "[glcore][gui]") {
    // keep a RenderBackendGL (or surface backend) singleton alive; core context with SKIP;
    // GLCoreGuiGraphics g; g.updateTarget();
    // call drawImage/drawPoint/both drawLine overloads/drawPolyLine/drawBezier/
    // drawRectangle/fillRectangle/drawCircle/drawFillCircle/drawCircleSegment/
    // drawFillCircleSegment/fillTriangle/pushClipArea/popClipArea with benign args;
    // CHECK_NOTHROW(...) for each; CHECK(g.getColor() == fcn::Color(...));
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][gui]"`
Expected: FAIL to compile (`gui/fifechan/base/glcore/glcore_gui_graphics.h: No such file or directory`).

- [ ] **Step 3: Implement the adapter and manager branches**

Copy the `DummyGuiGraphics` forwarding pattern: each virtual forwards to a base `RenderBackend` primitive or `Image::render()`; image drawing converts `fcn::Image` via the same path the dummy adapter uses; `updateTarget()` refreshes the cached `RenderBackend*` and dimensions. In `FifechanManager::init()` add `"OpenGL_CORE"` and `"GLES2"` branches constructing `GLCoreGuiGraphics` under `HAVE_OPENGL_CORE`/`HAVE_OPENGL_ES2`; in `resizeTopContainer()` route both names to `GLCoreGuiGraphics::updateTarget()`. Add the new files to the FifeGUI source/header lists and link `fifechan::fifechan` as today.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][gui]" && ./out/build/glcore-core/tests/core_tests/all_tests "[dummy]"`
Expected: PASS; dummy suite still green.

- [ ] **Step 5: Commit**

```bash
git add src/fife/gui/fifechan/base/glcore/ src/fife/gui/fifechan/fifechanmanager.cpp CMakeLists.txt tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_gui.cpp
git commit -m "feat(glcore): add generic Fifechan adapter and manager dispatch"
```

### Task 9: Integration — engine/settings selection and all four backend-name branch sites

**Files:**
- Create: `src/fife/video/glcore/glcore_names.h`, `src/fife/video/glcore/glcore_names.cpp`
- Modify: `src/fife/controller/enginesettings.cpp:160-168`, `src/fife/controller/engine.cpp:393-423,579-591`, `src/fife/view/renderers/instancerenderer.cpp:137-162`, `src/fife/view/layercache.cpp:192-201`, `src/fife/view/camera.cpp:1019-1028`, `src/fife/loaders/native/video/imageloader.cpp:130-133`, `CMakeLists.txt`, `tests/core_tests/CMakeLists.txt`
- Test: `tests/core_tests/test_glcore_selection.cpp`

**Interfaces:**
- Consumes: compile definitions `HAVE_OPENGL_CORE` / `HAVE_OPENGL_ES2` (Task 1), `getName()` values (Task 5).
- Produces:

```cpp
namespace FIFE {
bool isGlDepthBackend(std::string const& backendName);          // OpenGL, OpenGL_CORE, GLES2
bool usesOpenGLRenderTargetClip(std::string const& backendName); // those + both DUMMY_*; not SDL
bool usesGLRgba32UploadFormat(std::string const& backendName);   // those + both DUMMY_*; not SDL
bool bindsColorLikeSDL(std::string const& backendName);          // SDL only
bool isCompiledRenderBackend(std::string const& backendName);
std::string resolveCompiledBackendName(std::string const& requested); // logs warning, returns "SDL" for stale names
}
```

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_selection.cpp`, add it to `FIFE_CORE_TEST_SOURCES`:

```cpp
TEST_CASE("Backend-name predicates preserve the existing truth tables", "[glcore][selection]") {
    for (auto const* n : {"OpenGL", "OpenGL_CORE", "GLES2"}) {
        CHECK(FIFE::isGlDepthBackend(n));
        CHECK(FIFE::usesOpenGLRenderTargetClip(n));
        CHECK(FIFE::usesGLRgba32UploadFormat(n));
        CHECK_FALSE(FIFE::bindsColorLikeSDL(n));
    }
    CHECK_FALSE(FIFE::isGlDepthBackend("DUMMY_NULL"));          // layercache keeps CPU sorting
    CHECK(FIFE::usesOpenGLRenderTargetClip("DUMMY_NULL"));      // camera keeps Y flip
    CHECK(FIFE::usesGLRgba32UploadFormat("DUMMY_SURFACE"));     // imageloader keeps RGBA32
    CHECK(FIFE::bindsColorLikeSDL("SDL"));
    CHECK_FALSE(FIFE::usesOpenGLRenderTargetClip("SDL"));
}
TEST_CASE("Settings expose only compiled backend names", "[glcore][selection]") {
    auto names = FIFE::EngineSettings().getPossibleRenderBackends();
#ifdef HAVE_OPENGL_CORE
    CHECK(std::ranges::find(names, "OpenGL_CORE") != names.end());
#else
    CHECK(std::ranges::find(names, "OpenGL_CORE") == names.end());
#endif
#ifdef HAVE_OPENGL_ES2
    CHECK(std::ranges::find(names, "GLES2") != names.end());
#else
    CHECK(std::ranges::find(names, "GLES2") == names.end());
#endif
#ifdef HAVE_OPENGL_CORE
    FIFE::EngineSettings s; s.setRenderBackend("OpenGL_CORE");
    CHECK(s.getRenderBackend() == "OpenGL_CORE");
    FIFE::RenderBackendGLCore backend({0,0,0,255});
    CHECK(backend.getName() == "OpenGL_CORE");
#endif
}
TEST_CASE("A stale backend name resolves to SDL", "[glcore][selection]") {
    CHECK(FIFE::resolveCompiledBackendName("SDL") == "SDL");
#ifndef HAVE_OPENGL_CORE
    CHECK(FIFE::resolveCompiledBackendName("OpenGL_CORE") == "SDL");
#endif
#ifndef HAVE_OPENGL_ES2
    CHECK(FIFE::resolveCompiledBackendName("GLES2") == "SDL");
#endif
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][selection]"`
Expected: FAIL to compile (`video/glcore/glcore_names.h: No such file or directory`).

- [ ] **Step 3: Implement the predicates and wire the four sites, settings and factory**

Implement the predicates in `glcore_names.cpp` with the literal truth tables asserted above; `resolveCompiledBackendName()` uses `isCompiledRenderBackend()` and logs via `FL_WARN` before returning `"SDL"`. In `engine.cpp` resolve the configured name once before the factory, add `RenderBackendGLCore`/`RenderBackendGLES2` branches under the compile guards, and change `winSettings.opengl` at `:589` to `isGlDepthBackend(rbackend)` so both new names request `SDL_WINDOW_OPENGL`. In `enginesettings.cpp` append the two names under the same guards. Replace only the inline conditions at the four branch sites with the named predicates (keep their surrounding code and dummy asymmetry unchanged).

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][selection]" && ./out/build/glcore-core/tests/core_tests/all_tests "[dummy]"`
Expected: PASS in both option configurations; dummy suite green.

- [ ] **Step 5: Commit**

```bash
git add src/fife/video/glcore/glcore_names.* src/fife/controller/engine.cpp src/fife/controller/enginesettings.cpp src/fife/view/renderers/instancerenderer.cpp src/fife/view/layercache.cpp src/fife/view/camera.cpp src/fife/loaders/native/video/imageloader.cpp CMakeLists.txt tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_selection.cpp
git commit -m "feat(glcore): wire engine selection and backend-name branch sites"
```

### Task 10: ES2 — dialect, precision, texture fallbacks and ES2 CI job

**Files:**
- Modify: `src/fife/video/glcore/shaderloader.cpp`, `src/fife/video/glcore/glcoreimage.cpp`, `src/fife/video/glcore/renderbackendgl.cpp`, `.github/workflows/build-on-linux.yml:108-120,526-543,566-598`, `tests/core_tests/CMakeLists.txt`
- Test: `tests/core_tests/test_glcore_es2.cpp`

**Interfaces:**
- Consumes: `glcorePreamble(GLProfile)` (Task 2), `GLContext`/`makeES2Config()` (Task 1), `RenderBackendGLES2` (Task 5).
- Produces: ESSL 100 preamble and `mediump` qualifiers for all four programs; ES2 texture path using `GL_RGBA`/`GL_UNSIGNED_BYTE`, `GL_CLAMP_TO_EDGE` for NPOT and no mipmaps; FBO attachment through core ES2 entry points; an ES2 CI matrix entry.

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_es2.cpp`, add it to `FIFE_CORE_TEST_SOURCES`:

```cpp
TEST_CASE("ESSL 100 programs link in an ES2 context", "[glcore][es2]") {
    // create ES2 window/context via makeES2Config() with try/catch(SDLException) + SKIP
    FIFE::ShaderLoader loader(FIFE::GLProfile::ES2);
    for (auto p : {FIFE::GLCoreProgram::Sprite, FIFE::GLCoreProgram::Primitive,
                   FIFE::GLCoreProgram::Overlay, FIFE::GLCoreProgram::Light}) {
        CHECK(loader.get(p).isLinked());
    }
}
TEST_CASE("ES2 NPOT textures clamp and skip mipmaps", "[glcore][es2]") {
    // ES2 context with SKIP; upload 17x13 RGBA image via GLCoreImage
    // CHECK(texture wrap S/T == GL_CLAMP_TO_EDGE);
    // CHECK(min filter is GL_LINEAR/GL_NEAREST, not a mipmap filter);
    // CHECK(glGetError() == GL_NO_ERROR);
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake -S . -B out/build/glcore-es2 -DENABLE_OPENGL=ON -DENABLE_OPENGL_CORE=OFF -DENABLE_OPENGL_ES2=ON -DBUILD_TESTS=ON -DBUILD_PYTHON_BINDINGS=OFF && cmake --build out/build/glcore-es2 --target all_tests && ./out/build/glcore-es2/tests/core_tests/all_tests "[glcore][es2]"`
Expected: FAIL — ESSL 100 sources still contain GLSL 330 syntax, so program link fails (or SKIP if no ES2 context, in which case the shader-preamble assertions from Task 2 still fail on the ES2 string).

- [ ] **Step 3: Implement the ES2 dialect and fallbacks, then add the ES2 CI entry**

Complete `glcorePreamble(ES2)` with `#version 100` and explicit `precision mediump float;` (and `precision mediump sampler2D;` where needed), replacing `in/out` with `attribute/varying` through shared macros/aliases. In `glcoreimage.cpp` use `GL_RGBA`/`GL_UNSIGNED_BYTE` instead of sized formats, clamp NPOT to `GL_CLAMP_TO_EDGE` with no mipmaps, use `glGenerateMipmap` where available instead of `GL_GENERATE_MIPMAP`, and ignore compression/anisotropy settings on ES2. In `renderbackendgl.cpp` use core ES2 FBO entry points and keep the compressed-texture workaround desktop-only. In CI add a matrix entry with `ENABLE_OPENGL_ES2=ON`, `ENABLE_OPENGL_CORE=OFF`, `gl-tests: "true"` and an EGL surfaceless/Xvfb GLES2 context; if none can be obtained, have that job build and run only the shader-validation tests and document the limitation in `docs/glcore-backends.md`.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-es2 --target all_tests && ./out/build/glcore-es2/tests/core_tests/all_tests "[glcore][es2]" && ctest --test-dir out/build/glcore-es2 -R all_tests_dummy --output-on-failure`
Expected: PASS (or documented SKIP when no ES2 context); dummy suite green.

- [ ] **Step 5: Commit**

```bash
git add src/fife/video/glcore/shaderloader.cpp src/fife/video/glcore/glcoreimage.cpp src/fife/video/glcore/renderbackendgl.cpp .github/workflows/build-on-linux.yml docs/glcore-backends.md tests/core_tests/CMakeLists.txt tests/core_tests/test_glcore_es2.cpp
git commit -m "feat(glcore): add ES2 shaders, texture fallbacks and CI job"
```

### Task 11: Validation — aggregate pixel self-consistency, core CI and documentation

**Files:**
- Modify: `.github/workflows/build-on-linux.yml:295-344,526-598`, `docs/glcore-backends.md`, `tests/core_tests/CMakeLists.txt`
- Test: `tests/core_tests/test_glcore_validation.cpp`

**Interfaces:**
- Consumes: every backend/GUI interface from Tasks 5–10; the CI matrix convention (`gl-tests`, `ENABLE_MOVIE`, `-D` flags at `build-on-linux.yml:526-540`).
- Produces: one aggregate self-consistency test case set; a core-profile CI configuration; operator documentation.

- [ ] **Step 1: Write the failing test**

Create `tests/core_tests/test_glcore_validation.cpp`, add it to `FIFE_CORE_TEST_SOURCES`:

```cpp
TEST_CASE("Representative primitives are self-consistent on GLCore", "[glcore][validation]") {
    // core 3.3 backend with SKIP; draw textured quad, coloured triangle, filled circle,
    // each of the four OverlayType modes, and a stencil-based light;
    // for each: CHECK(readPixel(p) == firstRunPixel); CHECK(readPixel(p) != backgroundPixel);
    // no comparison against the legacy backend's exact colours.
}
TEST_CASE("Track 1 style Image::render batching survives a window recreate", "[glcore][validation]") {
    // after Window::create() replacement and createMainScreen(), draw five images;
    // CHECK(addImageToArray call count == 5); CHECK(renderVertexArrays flush count == 1);
    // CHECK(readPixel(...) == preRecreatePixel);
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][validation]"`
Expected: FAIL (`tests/core_tests/test_glcore_validation.cpp` is not yet registered in the build).

- [ ] **Step 3: Complete CI coverage and documentation**

Add a core-profile matrix entry (or extend the existing GL job) that configures with `ENABLE_OPENGL_CORE=ON`, `ENABLE_OPENGL_ES2=OFF`, keeps Xvfb + llvmpipe + `MESA_GL_VERSION_OVERRIDE=3.3`, and runs the full `ctest` suite. Document in `docs/glcore-backends.md`: the two selection strings, the three CMake options and their mutual-exclusion/`ENABLE_OPENGL=ON` rule, SKIP behaviour, and any ES2 CI context limitation. Verify the workflow contains both new option names and that `ENABLE_OPENGL_CORE`/`ENABLE_OPENGL_ES2` are never both ON in a single matrix entry.

- [ ] **Step 4: Run the test to verify it passes**

Run: `cmake --build out/build/glcore-core --target all_tests && ./out/build/glcore-core/tests/core_tests/all_tests "[glcore][validation]" && ctest --test-dir out/build/glcore-core --output-on-failure && ctest --test-dir out/build/glcore-core -R all_tests_dummy --output-on-failure && grep -q ENABLE_OPENGL_CORE .github/workflows/build-on-linux.yml && grep -q ENABLE_OPENGL_ES2 .github/workflows/build-on-linux.yml`
Expected: PASS; full suite green in both `BUILD` and `BUILD_ES2`; dummy suite green; both grep checks succeed.

- [ ] **Step 5: Commit**

```bash
git add tests/core_tests/test_glcore_validation.cpp tests/core_tests/CMakeLists.txt .github/workflows/build-on-linux.yml docs/glcore-backends.md
git commit -m "test(glcore): validate self-consistency, CI coverage and docs"
```
