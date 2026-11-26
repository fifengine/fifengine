// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <algorithm>
#include <vector>

#include "controller/enginesettings.h"
#include "glcore_test_env.h"
#include "video/glcore/glcore_names.h"
#include "video/glcore/renderbackendgl.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("Backend-name predicates preserve the existing truth tables", "[glcore][selection]")
{
    for (auto const * n : {"OpenGL", "OpenGL_CORE", "GLES2"}) {
        CHECK(FIFE::isGlDepthBackend(n));
        CHECK(FIFE::usesOpenGLRenderTargetClip(n));
        CHECK(FIFE::usesGLRgba32UploadFormat(n));
        CHECK_FALSE(FIFE::bindsColorLikeSDL(n));
    }
    CHECK_FALSE(FIFE::isGlDepthBackend("DUMMY_NULL"));
    CHECK(FIFE::usesOpenGLRenderTargetClip("DUMMY_NULL"));
    CHECK(FIFE::usesGLRgba32UploadFormat("DUMMY_SURFACE"));
    CHECK(FIFE::bindsColorLikeSDL("SDL"));
    CHECK_FALSE(FIFE::usesOpenGLRenderTargetClip("SDL"));
}

TEST_CASE("Settings expose only compiled backend names", "[glcore][selection]")
{
    auto names = FIFE::EngineSettings().getPossibleRenderBackends();
#ifdef HAVE_OPENGL_CORE
    CHECK(std::find(names.begin(), names.end(), "OpenGL_CORE") != names.end());
#else
    CHECK(std::find(names.begin(), names.end(), "OpenGL_CORE") == names.end());
#endif
#ifdef HAVE_OPENGL_ES2
    CHECK(std::find(names.begin(), names.end(), "GLES2") != names.end());
#else
    CHECK(std::find(names.begin(), names.end(), "GLES2") == names.end());
#endif
#ifdef HAVE_OPENGL_CORE
    FIFE::EngineSettings s;
    s.setRenderBackend("OpenGL_CORE");
    CHECK(s.getRenderBackend() == "OpenGL_CORE");
    FIFE::RenderBackendGLCore backend({0, 0, 0, 255});
    CHECK(backend.getName() == "OpenGL_CORE");
#endif
}

TEST_CASE("A stale backend name resolves to SDL", "[glcore][selection]")
{
    CHECK(FIFE::resolveCompiledBackendName("SDL") == "SDL");
#ifndef HAVE_OPENGL_CORE
    CHECK(FIFE::resolveCompiledBackendName("OpenGL_CORE") == "SDL");
#endif
#ifndef HAVE_OPENGL_ES2
    CHECK(FIFE::resolveCompiledBackendName("GLES2") == "SDL");
#endif
}
