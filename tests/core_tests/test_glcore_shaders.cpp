// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>

#include <string>

#include "glcore_test_env.h"
#include "util/base/exception.h"
#include "video/glcore/glcontext.h"
#include "video/glcore/shaderloader.h"
#include "video/window/window.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("Built-in shader preambles pin GLSL 330 and ESSL 100", "[glcore][shader]")
{
    CHECK(std::string(FIFE::glcorePreamble(FIFE::GLProfile::Core33)).rfind("#version 330 core", 0) == 0U);
    CHECK(std::string(FIFE::glcorePreamble(FIFE::GLProfile::ES2)).rfind("#version 100", 0) == 0U);
    CHECK(
        std::string(FIFE::glcorePreamble(FIFE::GLProfile::ES2)).find("precision mediump float;") != std::string::npos);
}

TEST_CASE("All four built-in programs compile and link", "[glcore][shader]")
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SKIP("SDL video unavailable");
    }
    FIFE::Window window;
    try {
        window.create({.width = 64, .height = 64, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    FIFE::GLContext ctx(FIFE::makeCore33Config());
    ctx.applyAttributes();
    if (!ctx.create(window.getSDLWindow())) {
        SKIP("OpenGL ES 2.0 not available in this environment");
    }

    FIFE::ShaderLoader loader(FIFE::GLProfile::Core33);
    for (auto p :
         {FIFE::GLCoreProgram::Sprite,
          FIFE::GLCoreProgram::Primitive,
          FIFE::GLCoreProgram::Overlay,
          FIFE::GLCoreProgram::Light}) {
        CHECK(loader.get(p).isLinked());
        CHECK(loader.uniform(p, "uProjection") >= 0);
    }
    CHECK(loader.uniform(FIFE::GLCoreProgram::Sprite, "uTexture0") >= 0);
    CHECK(loader.uniform(FIFE::GLCoreProgram::Sprite, "uAlphaTest") >= 0);
    CHECK(loader.uniform(FIFE::GLCoreProgram::Overlay, "uTexture1") >= 0);
    CHECK(loader.uniform(FIFE::GLCoreProgram::Overlay, "uOverlayMode") >= 0);
    CHECK(loader.uniform(FIFE::GLCoreProgram::Light, "uIntensity") >= 0);
}
