// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "video/glcore/glcontext.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("Tier B profiles map to SDL attributes and capabilities", "[modern-gl][gl43][gles31]")
{
    FIFE::GLContextConfig cfg43;
    cfg43.profile  = FIFE::GLProfile::Core43;
    auto const a43 = FIFE::glAttributesFor(cfg43);
    CHECK(a43.major == 4);
    CHECK(a43.minor == 3);
    CHECK(a43.profileMask == SDL_GL_CONTEXT_PROFILE_CORE);

    FIFE::GLContextConfig cfg31;
    cfg31.profile  = FIFE::GLProfile::ES31;
    auto const a31 = FIFE::glAttributesFor(cfg31);
    CHECK(a31.major == 3);
    CHECK(a31.minor == 1);
    CHECK(a31.profileMask == SDL_GL_CONTEXT_PROFILE_ES);

    CHECK_FALSE(FIFE::capabilitiesFor(FIFE::GLProfile::Core33).computeShaders);
    CHECK_FALSE(FIFE::capabilitiesFor(FIFE::GLProfile::ES2).computeShaders);
    CHECK_FALSE(FIFE::capabilitiesFor(FIFE::GLProfile::Core33).shaderStorageBuffers);
    CHECK(FIFE::capabilitiesFor(FIFE::GLProfile::Core43).computeShaders);
    CHECK(FIFE::capabilitiesFor(FIFE::GLProfile::ES31).computeShaders);
    CHECK(FIFE::capabilitiesFor(FIFE::GLProfile::Core43).shaderStorageBuffers);
    CHECK(FIFE::capabilitiesFor(FIFE::GLProfile::ES31).shaderStorageBuffers);
}
