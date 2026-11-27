// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Compile-only guard: the shared particle bridge header must never expose GL.
#ifdef GL_VERSION
    #error "GL leaked into video/iparticlerenderbackend.h"
#endif

#include <type_traits>

#include "video/iparticlerenderbackend.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("Particle bridge is GL-free", "[modern-gl][gpu-particles]")
{
    STATIC_REQUIRE(std::has_virtual_destructor_v<FIFE::IParticleRenderBackend>);
    STATIC_REQUIRE(std::is_standard_layout_v<FIFE::ParticleGpuRecord>);
    STATIC_REQUIRE(std::is_trivially_copyable_v<FIFE::ParticleGpuRecord>);
}
