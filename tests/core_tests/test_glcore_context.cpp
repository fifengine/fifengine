// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>

#include "glcore_test_env.h"
#include "util/base/exception.h"
#include "video/glcore/glcontext.h"
#include "video/window/window.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("GLContextConfig pins core 3.3 and ES 2.0 values", "[glcore][context]")
{
    auto core = FIFE::makeCore33Config();
    CHECK(core.profile == FIFE::GLProfile::Core33);
    CHECK(core.major == 3);
    CHECK(core.minor == 3);
    CHECK(core.stencilBits == 8);

    auto es = FIFE::makeES2Config();
    CHECK(es.profile == FIFE::GLProfile::ES2);
    CHECK(es.major == 2);
    CHECK(es.minor == 0);
}

TEST_CASE("GLContext creates a core 3.3 context through SDL", "[glcore][context]")
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
    int value = 0;
    REQUIRE(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value));
    CHECK(value == 3);
    REQUIRE(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value));
    CHECK(value == 3);
    REQUIRE(SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &value));
    CHECK(value == SDL_GL_CONTEXT_PROFILE_CORE);

    if (!ctx.create(window.getSDLWindow())) {
        SKIP("OpenGL ES 2.0 not available in this environment");
    }
    CHECK(ctx.isLoaded());
    ctx.destroy();
}
