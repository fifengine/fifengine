// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "glcore_test_env.h"
#include "util/base/exception.h"
#include "video/glcore/renderbackendgl.h"
#include "video/window/window.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

namespace
{
    int readChannel(int x, int y, int channel)
    {
        unsigned char pixel[4]{};
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
        return pixel[channel];
    }
} // namespace

TEST_CASE("GLCore primitives draw self-consistent pixels", "[glcore][primitive]")
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
        backend.createMainScreen("primitives", "");
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL context not available in this environment");
    }

    backend.clearBackBuffer();
    glClearColor(1.0F, 0.0F, 0.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    CHECK(readChannel(30, 10, 0) == 255);
    backend.clearBackBuffer();
    backend.drawLine(FIFE::Point(10, 10), FIFE::Point(50, 10), 255, 0, 0);
    CHECK(backend.bufferedPrimitiveVertices() == 6);
    backend.renderVertexArrays();
    CHECK(backend.bufferedPrimitiveVertices() == 0);
    CHECK(glGetError() == GL_NO_ERROR);
    glGetError(); // drain setup errors from context creation
    int w = 0;
    int h = 0;
    SDL_GetWindowSize(window.getSDLWindow(), &w, &h);
    CHECK(backend.debugProjection()[0] == 2.0F / static_cast<float>(w));
    int lit = 0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (readChannel(x, y, 0) > 0) {
                ++lit;
            }
        }
    }
    CHECK(lit > 0);

    backend.fillTriangle(FIFE::Point(5, 20), FIFE::Point(40, 20), FIFE::Point(20, 50), 0, 255, 0);
    backend.renderVertexArrays();
    bool greenFound = false;
    for (int y = 0; y < h && !greenFound; ++y) {
        for (int x = 0; x < w; ++x) {
            if (readChannel(x, y, 1) == 255) {
                greenFound = true;
                break;
            }
        }
    }
    CHECK(greenFound);
}
