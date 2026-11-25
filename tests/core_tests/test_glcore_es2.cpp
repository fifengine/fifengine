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

TEST_CASE("ESSL 100 preamble is loader-neutral", "[glcore][es2]")
{
    std::string const preamble(FIFE::glcorePreamble(FIFE::GLProfile::ES2));
    CHECK(preamble.rfind("#version 100", 0) == 0U);
    CHECK(preamble.find("precision mediump float;") != std::string::npos);
    CHECK(preamble.find("#define GLCORE_ATTRIBUTE attribute") != std::string::npos);
    CHECK(preamble.find("#define GLCORE_TEXTURE texture2D") != std::string::npos);
}

TEST_CASE("All four built-in programs link in an ES2 context", "[glcore][es2]")
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SKIP("SDL video unavailable");
    }
    FIFE::Window window;
    try {
        window.create({.width = 64, .height = 64, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL ES 2.0 not available in this environment");
    }
    FIFE::GLContext ctx(FIFE::makeES2Config());
    ctx.applyAttributes();
    if (!ctx.create(window.getSDLWindow())) {
        SKIP("OpenGL ES 2.0 not available in this environment");
    }

    FIFE::ShaderLoader loader(FIFE::GLProfile::ES2);
    for (auto p :
         {FIFE::GLCoreProgram::Sprite,
          FIFE::GLCoreProgram::Primitive,
          FIFE::GLCoreProgram::Overlay,
          FIFE::GLCoreProgram::Light}) {
        CHECK(loader.get(p).isLinked());
    }
}
