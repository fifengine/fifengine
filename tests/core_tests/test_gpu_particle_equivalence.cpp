// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <cmath>

#include "model/structures/particleemitter.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

namespace
{
    // World units. CI may widen this for ES 3.1 software rendering.
    constexpr float kEquivalenceTolerance = 10.0F;

    FIFE::ParticleConfig cfg()
    {
        FIFE::ParticleConfig c;
        c.emissionRate = 120.0F;
        c.lifetimeMin  = 10.0F;
        c.lifetimeMax  = 10.0F;
        c.maxParticles = 1024;
        return c;
    }
} // namespace

TEST_CASE("CPU and GPU paths agree statistically, not bitwise", "[modern-gl][gpu-particles][equivalence]")
{
    FIFE::ParticleConfig const c = cfg();
    FIFE::ParticleEmitter cpu{c};
    FIFE::ParticleEmitter gpu{c};
    cpu.setSeed(2026);
    gpu.setSeed(2026);
    gpu.setComputeEnabled(true);

    for (int i = 0; i < 240; ++i) {
        cpu.update(1.0F / 120.0F);
        gpu.update(1.0F / 120.0F);
    }

    CHECK(gpu.getSpawnedCount() == cpu.getSpawnedCount());
    CHECK(gpu.getDeathSchedule() == cpu.getDeathSchedule());
    auto const ccpu = cpu.getCentroid();
    auto const cgpu = gpu.getCentroid();
    CHECK(std::abs(cgpu.x - ccpu.x) <= static_cast<double>(kEquivalenceTolerance));
    CHECK(std::abs(cgpu.y - ccpu.y) <= static_cast<double>(kEquivalenceTolerance));
    CHECK(gpu.getAliveEstimate() <= gpu.getCapacity());
    // Deliberately no per-particle index-by-index equality check.
}
