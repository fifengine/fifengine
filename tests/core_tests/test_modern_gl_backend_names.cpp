// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <algorithm>

#include "controller/enginesettings.h"
#include "fixture.h"
#include "video/dummy/renderbackendnull.h"
#include "video/glcore/glcore_names.h"
#include "view/camera.h"
#include "view/layercache.h"
#include "view/renderers/instancerenderer.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("Both new backend names are selectable only when compiled in", "[modern-gl][settings]")
{
    auto const names = FIFE::EngineSettings().getPossibleRenderBackends();
#ifdef HAVE_OPENGL_43
    CHECK(std::find(names.begin(), names.end(), "OpenGL_43") != names.end());
#else
    CHECK(std::find(names.begin(), names.end(), "OpenGL_43") == names.end());
#endif
#ifdef HAVE_OPENGLES_31
    CHECK(std::find(names.begin(), names.end(), "GLES_31") != names.end());
#else
    CHECK(std::find(names.begin(), names.end(), "GLES_31") == names.end());
#endif

    FIFE::EngineSettings settings;
    settings.setRenderBackend("OpenGL_43");
#ifdef HAVE_OPENGL_43
    CHECK(settings.getRenderBackend() == "OpenGL_43");
#else
    CHECK(settings.getRenderBackend() == "SDL");
#endif
}

TEST_CASE("Both new names take the OpenGL side at the branch sites", "[modern-gl][backend-names]")
{
    TestFixture fixture;
    for (auto const name : {"OpenGL_43", "GLES_31"}) {
        FIFE::RenderBackendNull backend({0, 0, 0, 255}, 640, 480);
        // The predicates are pure functions of the name, so the singleton is
        // only needed for LayerCache/Camera construction.
        CHECK(FIFE::isGlDepthBackend(name));
        CHECK(FIFE::usesOpenGLRenderTargetClip(name));
        CHECK(FIFE::usesGLRgba32UploadFormat(name));
        CHECK_FALSE(FIFE::bindsColorLikeSDL(name));

        FIFE::InstanceRenderer renderer(&backend, 10);
        CHECK_FALSE(renderer.needColorBinding());
    }
}
