// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include "model/structures/particleemitter.h"

TEST_CASE("seeded emitters reproduce counts and positions", "[particle]")
{
    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 4.0F;
    cfg.lifetimeMin = cfg.lifetimeMax = 1.0F;
    cfg.speedMin = cfg.speedMax = 1.0F;
    cfg.maxParticles = 8;
    FIFE::ParticleEmitter a(cfg), b(cfg);
    a.setSeed(1234);
    b.setSeed(1234);
    for (int i = 0; i < 12; ++i) { a.update(0.25F); b.update(0.25F); }
    CHECK(a.getEmittedCount() == 12);
    CHECK(a.getAliveCount() == 3);
    for (size_t s = 0; s < a.getCapacity(); ++s) {
        CHECK(a.getParticle(s).alive == b.getParticle(s).alive);
        CHECK(a.getParticle(s).x == Catch::Approx(b.getParticle(s).x));
        CHECK(a.getParticle(s).y == Catch::Approx(b.getParticle(s).y));
    }
    // Slot reuse order is an implementation detail; the newest live particle
    // must have integrated exactly one step with vx = speed = 1.
    float newestAge = 100.0F;
    float newestX   = 0.0F;
    for (size_t s = 0; s < a.getCapacity(); ++s) {
        auto const p = a.getParticle(s);
        if (p.alive && p.age < newestAge) {
            newestAge = p.age;
            newestX   = p.x;
        }
    }
    CHECK(newestAge == Catch::Approx(0.25));
    CHECK(newestX == Catch::Approx(0.25));
}

TEST_CASE("capacity uses the ceil formula and the 2048 clamp", "[particle]")
{
    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 4.0F; cfg.lifetimeMax = 1.0F; cfg.burstCount = 2;
    CHECK(FIFE::ParticleEmitter(cfg).getCapacity() == 6);
    cfg.emissionRate = 2000.0F; cfg.lifetimeMax = 2.0F; cfg.burstCount = 100;
    CHECK(FIFE::ParticleEmitter(cfg).getCapacity() == 2048);
}

TEST_CASE("maxParticles can be changed after construction", "[particle]")
{
    FIFE::ParticleConfig cfg;
    cfg.maxParticles = 4;
    FIFE::ParticleEmitter e(cfg);
    CHECK(e.getMaxParticles() == 4);
    e.setMaxParticles(64);
    CHECK(e.getMaxParticles() == 64);
    CHECK(e.getCapacity() == 64);
    e.setMaxParticles(4096);
    CHECK(e.getMaxParticles() == 2048);
    CHECK(e.getCapacity() == 2048);
}

TEST_CASE("pool overflow drops emissions without recycling live particles", "[particle]")
{
    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 0.0F; cfg.burstCount = 5; cfg.burstInterval = 0.25F;
    cfg.maxParticles = 2; cfg.lifetimeMin = cfg.lifetimeMax = 10.0F;
    FIFE::ParticleEmitter e(cfg);
    e.update(0.25F);
    CHECK(e.getEmittedCount() == 2);
    CHECK(e.getAliveCount() == 2);
    e.update(0.25F);
    CHECK(e.getEmittedCount() == 2);
    CHECK(e.getParticle(0).age == Catch::Approx(0.5));
}

TEST_CASE("emitter integrates gravity, colour, size and atlas frames", "[particle]")
{
    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 4.0F; cfg.lifetimeMin = cfg.lifetimeMax = 1.0F; cfg.maxParticles = 4;
    cfg.speedMin = cfg.speedMax = 1.0F; cfg.gravity = -2.0F;
    cfg.startSize = 4.0F; cfg.endSize = 8.0F;
    cfg.startColor[0] = cfg.startColor[1] = cfg.startColor[2] = 0; cfg.startColor[3] = 255;
    cfg.endColor[0] = cfg.endColor[1] = cfg.endColor[2] = 255; cfg.endColor[3] = 0;
    cfg.frameCount = 2; cfg.frameDuration = 0.5F;
    FIFE::ParticleEmitter e(cfg);
    e.update(0.25F); e.update(0.25F);
    auto const p = e.getParticle(0);
    CHECK(p.size == Catch::Approx(6.0));
    CHECK(p.rgba[0] == 128);
    CHECK(p.rgba[3] == 128);
    CHECK(p.frame == 1);
    CHECK(p.vy == Catch::Approx(-1.0));
    CHECK(p.y == Catch::Approx(-0.375));
}

TEST_CASE("drag damps velocity without reversing it", "[particle]")
{
    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 4.0F; cfg.lifetimeMin = cfg.lifetimeMax = 10.0F; cfg.maxParticles = 4;
    cfg.speedMin = cfg.speedMax = 1.0F; cfg.drag = 2.0F;
    FIFE::ParticleEmitter e(cfg);
    e.update(0.25F);
    CHECK(e.getParticle(0).vx == Catch::Approx(0.5));
    CHECK(e.getParticle(0).x == Catch::Approx(0.125));
}

TEST_CASE("one-shot emitter fires only its first burst", "[particle]")
{
    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 0.0F; cfg.burstCount = 2; cfg.burstInterval = 0.25F;
    cfg.maxParticles = 8; cfg.lifetimeMin = cfg.lifetimeMax = 10.0F; cfg.looping = false;
    FIFE::ParticleEmitter e(cfg);
    e.update(0.25F);
    CHECK(e.getEmittedCount() == 2);
    e.update(0.25F);
    CHECK(e.getEmittedCount() == 2);
}
