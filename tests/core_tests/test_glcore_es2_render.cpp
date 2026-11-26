// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>

#include <vector>

#include "glcore_test_env.h"
#include "util/base/exception.h"
#include "video/glcore/glcontext.h"
#include "video/glcore/glcore_draw.h"
#include "video/glcore/shaderloader.h"
#include "video/window/window.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("GLES2 executes real draw calls and pixel readback", "[glcore][es2][render]")
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
    REQUIRE(loader.get(FIFE::GLCoreProgram::Primitive).isLinked());

    float const projection[16] = {
        1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F};
    FIFE::glcore_draw::setViewport(0, 0, 64, 64);
    FIFE::glcore_draw::clear();

    // One red triangle in NDC covering the centre of the target.
    float const vertices[] = {
        -0.5F, -0.5F, 1.0F, 0.0F, 0.0F, 1.0F, 0.5F, -0.5F, 1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.5F, 1.0F, 0.0F, 0.0F, 1.0F};
    FIFE::glcore_draw::drawBatches(
        vertices,
        sizeof(vertices) / sizeof(float),
        {},
        loader.get(FIFE::GLCoreProgram::Primitive).program(),
        loader.get(FIFE::GLCoreProgram::Sprite).program(),
        loader.get(FIFE::GLCoreProgram::Overlay).program(),
        projection);

    std::vector<uint8_t> pixels(64 * 64 * 4);
    FIFE::glcore_draw::readRGBA(0, 0, 64, 64, pixels.data());
    bool red = false;
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        if (pixels[i] == 255 && pixels[i + 3] == 255) {
            red = true;
            break;
        }
    }
    CHECK(red);
}
