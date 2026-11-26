// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>

#include <algorithm>
#include <vector>

#include "glcore_test_env.h"
#include "util/base/exception.h"
#include "video/glcore/renderbackendgl.h"
#include "video/window/window.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

namespace
{
    std::vector<unsigned char> snapshot(int w, int h)
    {
        std::vector<unsigned char> pixels(static_cast<size_t>(w * h * 4));
        glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        return pixels;
    }
} // namespace

TEST_CASE("Representative primitives are self-consistent on GLCore", "[glcore][validation]")
{
    FIFE::Window window;
    try {
        window.create({.width = 128, .height = 128, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    FIFE::RenderBackendGLCore backend({0, 0, 0, 255});
    backend.init("");
    backend.setWindowObject(&window);
    try {
        backend.createMainScreen("validation", "");
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL context not available in this environment");
    }

    int w = 0;
    int h = 0;
    SDL_GetWindowSize(window.getSDLWindow(), &w, &h);

    auto draw = [&]() {
        backend.clearBackBuffer();
        backend.drawLine(FIFE::Point(10, 10), FIFE::Point(80, 10), 255, 0, 0);
        backend.fillTriangle(FIFE::Point(5, 20), FIFE::Point(60, 20), FIFE::Point(30, 60), 0, 255, 0);
        backend.drawFillCircle(FIFE::Point(80, 80), 12, 0, 0, 255);
        backend.renderVertexArrays();
        return snapshot(w, h);
    };

    auto const first  = draw();
    auto const second = draw();
    CHECK(first == second);
    CHECK(std::any_of(first.begin(), first.end(), [](unsigned char c) {
        return c != 0;
    }));
}
