// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>

#include <filesystem>
#include <string>
#include <vector>

#include "glcore_test_env.h"
#include "util/base/exception.h"
#include "video/glcore/renderbackendgl.h"
#include "video/window/window.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("Tier A state, GUI geometry, scaling, targets and capture are implemented", "[glcore][features]")
{
    FIFE::Window window;
    try {
        window.create({.width = 64, .height = 64, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    FIFE::RenderBackendGLCore backend({0, 0, 0, 255});
    backend.init("");
    backend.setWindowObject(&window);
    try {
        backend.createMainScreen("features", "");
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL context not available in this environment");
    }
    glGetError();

    // changeRenderInfos maps onto the state cache.
    backend.changeRenderInfos(
        FIFE::RENDER_DATA_TEXTURE_Z,
        6,
        0x0302,
        0x0303,
        true,
        true,
        1,
        FIFE::REPLACE,
        FIFE::GEQUAL,
        FIFE::OVERLAY_TYPE_COLOR);
    CHECK(backend.debugBlendSrc() == 0x0302U);
    CHECK(backend.debugBlendDst() == 0x0303U);
    CHECK(backend.debugStencilEnabled());
    CHECK(backend.debugOverlayType() == FIFE::OVERLAY_TYPE_COLOR);

    // renderGuiGeometry emits indexed GUI triangles.
    std::vector<FIFE::GuiVertex> vertices(3);
    vertices[0].position = FIFE::DoublePoint(10.0, 10.0);
    vertices[1].position = FIFE::DoublePoint(40.0, 10.0);
    vertices[2].position = FIFE::DoublePoint(10.0, 40.0);
    for (auto& v : vertices) {
        v.color.set(255, 0, 0, 255);
    }
    backend.clearBackBuffer();
    backend.renderGuiGeometry(vertices, {0, 1, 2}, FIFE::DoublePoint(0.0, 0.0), FIFE::ImagePtr());
    backend.renderVertexArrays();
    bool redFound = false;
    int w         = 0;
    int h         = 0;
    SDL_GetWindowSize(window.getSDLWindow(), &w, &h);
    std::vector<unsigned char> frame(static_cast<size_t>(w * h * 4));
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, frame.data());
    for (size_t i = 0; i + 3 < frame.size(); i += 4) {
        if (frame[i] == 255 && frame[i + 3] == 255) {
            redFound = true;
            break;
        }
    }
    CHECK(redFound);

    // setScalingMode recomputes the projection.
    float const * before = backend.debugProjection();
    float const before0  = before[0];
    backend.setScalingMode(FIFE::ScalingMode::Integer);
    CHECK(backend.debugProjection()[0] != 0.0F);
    CHECK(backend.debugProjection()[0] == before0);

    // captureScreen writes a PNG.
    std::filesystem::path const path = "glcore-capture-test.png";
    backend.captureScreen(path.string());
    CHECK(std::filesystem::exists(path));
    std::error_code ec;
    std::filesystem::remove(path, ec);
}
