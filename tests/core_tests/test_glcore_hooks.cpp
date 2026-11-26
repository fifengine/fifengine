// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "glcore_test_env.h"
#include "util/base/exception.h"
#include "video/dummy/renderbackendsurface.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/window/window.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

class CountingSurfaceBackend final : public FIFE::RenderBackendSurface
{
    public:
        using FIFE::RenderBackend::addImageToArray;
        using FIFE::RenderBackend::addImageToArrayZ;
        using FIFE::RenderBackendSurface::RenderBackendSurface;
        int calls       = 0;
        uint32_t lastId = 0;
        void addImageToArray(uint32_t id, FIFE::Rect const &, float const *, uint8_t, uint8_t const *) override
        {
            ++calls;
            lastId = id;
        }
};

TEST_CASE("Batching hook defaults fall back to the first texture", "[dummy][glcore][hooks]")
{
    CountingSurfaceBackend backend({0, 0, 0, 255}, 64, 64);
    FIFE::Rect const rect(0, 0, 8, 8);
    float st[4]{0, 0, 1, 1};
    uint8_t rgba[4]{255, 255, 255, 255};
    backend.addImageToArray(rect, 11u, st, 22u, st, 255, rgba);
    CHECK(backend.calls == 1);
    CHECK(backend.lastId == 11u);
    backend.addImageToArrayZ(33u, rect, 0.5F, st, 255, rgba);
    CHECK(backend.calls == 2);
    CHECK(backend.lastId == 33u);
    backend.addImageToArrayZ(rect, 0.5F, 44u, st, 55u, st, 255, rgba);
    CHECK(backend.calls == 3);
    CHECK(backend.lastId == 44u);
}

TEST_CASE("Legacy OpenGL backend keeps its batching overrides", "[glcore][hooks]")
{
    FIFE::Window window;
    try {
        window.create({.width = 64, .height = 64, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    FIFE::RenderBackendOpenGL backend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    backend.init("");
    backend.setWindowObject(&window);
    backend.createMainScreen("glcore-hooks", "");

    FIFE::Rect const rect(0, 0, 8, 8);
    float st[4]{0, 0, 1, 1};
    uint8_t rgba[4]{255, 255, 255, 255};
    CHECK_NOTHROW(backend.addImageToArrayZ(1u, rect, 0.5F, st, 255, rgba));
    CHECK_NOTHROW(backend.addImageToArrayZ(rect, 0.5F, 1u, st, 2u, st, 255, rgba));
}
