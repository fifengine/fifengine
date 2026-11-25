// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "glcore_test_env.h"
#include "video/glcore/glcorestate.h"
#include "video/glcore/renderbackendgl.h"
#include <catch2/catch_approx.hpp>      // NOLINT(build/include_alpha)
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("GLCoreState builds the orthographic projection used by the legacy backend", "[glcore][state]")
{
    FIFE::GLCoreState state;
    state.setProjection(0.0F, 100.0F, 75.0F, 0.0F, -100.0F, 100.0F);
    CHECK(state.projection()[0] == Catch::Approx(0.02F));
    CHECK(state.projection()[5] == Catch::Approx(-0.0266667F));
    CHECK(state.projection()[10] == Catch::Approx(-0.01F));
}

TEST_CASE("Backend names and profile configs are stable", "[glcore][state]")
{
    {
        FIFE::RenderBackendGLCore core({0, 0, 0, 255});
        CHECK(core.getName() == "OpenGL_CORE");
        CHECK(core.getWindow() == nullptr);
        CHECK(core.getBaseWidth() == 1280);
    }
    {
        FIFE::RenderBackendGLES2 es({0, 0, 0, 255});
        CHECK(es.getName() == "GLES2");
    }
}
