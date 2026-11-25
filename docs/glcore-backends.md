# GLCore backends (OpenGL 3.3 core / OpenGL ES 2.0)

## Build options

- `ENABLE_OPENGL_CORE` — build the `"OpenGL_CORE"` backend (GL 3.3 core profile).
- `ENABLE_OPENGL_ES2` — build the `"GLES2"` backend (OpenGL ES 2.0).

- OpenGL Core (3.3): Works natively on Wayland with LIBGL_ALWAYS_SOFTWARE=true +
GALLIUM_DRIVER=llvmpipe (Mesa's software renderer supports core profile on Wayland).
- OpenGL ES 2.0: Requires Xvfb (X11 virtual framebuffer) because
the EGL/GLES2 implementation in Mesa needs X11 to create an ES2 context.

The two options are mutually exclusive and both require `ENABLE_OPENGL=ON`.
They default to OFF; when neither is set, the shader backend is not compiled
and the legacy SDL/OpenGL backends behave exactly as before.

## Selection

Set the backend name in `EngineSettings::setRenderBackend()` to `"OpenGL_CORE"`
or `"GLES2"`. `EngineSettings::getPossibleRenderBackends()` only lists names
that were compiled in. A stale config naming a backend that was not compiled
logs a warning and falls back to SDL.

## Loader boundary

GLEW and glad are compiled into the same executable but never into the same
translation unit:

- `renderbackendgl.cpp` includes `renderbackend.h` (and therefore GLEW) and
  uses GLEW entry points only.
- Every other `video/glcore/*.cpp` file includes only the generated glad
  header for the active profile.
- Shared headers (`glcore_bridge.h`, `glcore_names.h`, `glcoreimage.h`, …)
  include neither loader.

Both loaders are initialized against the same current SDL GL context:
`GLContext::create()` calls `glcoreLoadProcs()`, and the backend shell calls
`glewInit()`.

## Backend-name behaviour

The four historical string-branch sites use named predicates from
`glcore_names.h` so the dummy-backend asymmetry is preserved exactly. See the
Tier A design doc for the truth table.

## Implemented features

- `changeRenderInfos()` maps data type, blend, depth, stencil and overlay mode
  onto the state cache and per-batch shader uniforms.
- Overlays use the four `OverlayType` modes through the two-texture batching
  hook and the overlay shader program.
- Stencil lighting goes through `GLCoreState::setStencil()`;
  `drawLightPrimitive()` draws an intensity-scaled fill.
- `renderGuiGeometry()` converts indexed `GuiVertex` data into batched
  triangles; `setScalingMode()` recomputes the projection from point/pixel
  sizes.
- Render targets use core FBO entry points; `captureScreen()` reads back to an
  `SDL_Surface` and writes PNG through `Image::saveAsPng()`.
- Window replacement destroys and recreates the context, shaders, draw
  buffers and FBOs.
- `GLCoreImage` implements single-texture, overlay and Z rendering plus
  `updateTexture()`.

## Test Commands

### OpenGL Core (Wayland native):

export LIBGL_ALWAYS_SOFTWARE=true
export GALLIUM_DRIVER=llvmpipe
export FIFE_GL_REQUIRE_CONTEXT=1
./out/build/clang22-x64-linux-dbg-cov/tests/core_tests/all_tests

### OpenGL ES 2.0 (requires Xvfb):

export LIBGL_ALWAYS_SOFTWARE=true
export GALLIUM_DRIVER=llvmpipe
export EGL_PLATFORM=x11
export FIFE_GL_REQUIRE_CONTEXT=1
xvfb-run -a -s "-screen 0 1280x800x24" ./out/build/clang22-x64-linux-dbg-cov/tests/core_tests/all_tests

## Limitations / follow-ups

- ES2 has no core VAO support; `GLCoreVertexArray` and the draw helper set
  attribute state directly. All GL calls for the new backend are made by the
  glad-only `glcore_draw.cpp`; `renderbackendgl.cpp` makes none.
- A real GLES2 context is exercised locally with Mesa llvmpipe under Xvfb:
  `./scripts/test-glcore.sh`. The `[glcore][es2][render]` test creates an ES2
  context through SDL, links the ESSL 100 programs, draws a triangle via the
  glad-only helper and verifies pixel readback. Set `FIFE_GL_REQUIRE_CONTEXT=1`
  to turn any missing-context SKIP into a failure; CI does this in the GLCore
  matrix entries. llvmpipe validates the GLES2 API path but not mobile GPU
  vendor drivers.
- GLES2 and GL 3.3 core are the floor; compute shaders, instancing and modern
  profile variants are Tier B.
