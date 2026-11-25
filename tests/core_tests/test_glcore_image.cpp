// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>

#include "glcore_test_env.h"
#include "util/base/exception.h"
#include "video/glcore/glcoreimage.h"
#include "video/glcore/renderbackendgl.h"
#include "video/window/window.h"
#include <catch2/catch_approx.hpp>      // NOLINT(build/include_alpha)
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

namespace
{
    class CountingGLCore : public FIFE::RenderBackendGL
    {
        public:
            CountingGLCore() : FIFE::RenderBackendGL({0, 0, 0, 255}, FIFE::makeCore33Config(), "COUNTING")
            {
            }
            int addCalls   = 0;
            int flushCalls = 0;
            using FIFE::RenderBackendGL::addImageToArray;
            void addImageToArray(
                uint32_t id, FIFE::Rect const & rec, float const * st, uint8_t alpha, uint8_t const * rgba) override
            {
                ++addCalls;
                FIFE::RenderBackendGL::addImageToArray(id, rec, st, alpha, rgba);
            }
            void renderVertexArrays() override
            {
                ++flushCalls;
                FIFE::RenderBackendGL::renderVertexArrays();
            }
    };
} // namespace

TEST_CASE("Atlas texel correction matches the legacy half-pixel rule", "[glcore][image]")
{
    auto corrected = FIFE::glcoreAtlasTexCoords(FIFE::Rect(2, 3, 4, 5), 16, 16, true);
    CHECK(corrected[0] == Catch::Approx(2.5F / 16));
    CHECK(corrected[1] == Catch::Approx(3.5F / 16));
    CHECK(corrected[2] == Catch::Approx(5.5F / 16));
    CHECK(corrected[3] == Catch::Approx(7.5F / 16));

    auto raw = FIFE::glcoreAtlasTexCoords(FIFE::Rect(2, 3, 4, 5), 16, 16, false);
    CHECK(raw[2] == Catch::Approx(6.0F / 16));
    CHECK(raw[3] == Catch::Approx(8.0F / 16));
}

TEST_CASE("Image::render batches through the GLCore backend", "[glcore][image]")
{
    FIFE::Window window;
    try {
        window.create({.width = 64, .height = 64, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    CountingGLCore backend;
    backend.init("");
    backend.setWindowObject(&window);
    try {
        backend.createMainScreen("image-batching", "");
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL context not available in this environment");
    }

    unsigned char data[16]{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    FIFE::GLCoreImage image(data, 2, 2);
    REQUIRE(image.texture() != 0);

    for (int i = 0; i < 5; ++i) {
        image.render(FIFE::Rect(i * 8, 0, 8, 8));
    }
    CHECK(backend.addCalls == 5);
    backend.renderVertexArrays();
    CHECK(backend.flushCalls == 1);
}
