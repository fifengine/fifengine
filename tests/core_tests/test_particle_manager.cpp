// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include "fixture.h"
#include "model/structures/particlemanager.h"
#include "util/time/timemanager.h"

namespace
{
FIFE::ParticleConfig fastConfig()
{
    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 120.0F; cfg.lifetimeMin = cfg.lifetimeMax = 10.0F; cfg.maxParticles = 1024;
    return cfg;
}
struct MutatingEmitter : FIFE::ParticleEmitter
{
    using FIFE::ParticleEmitter::ParticleEmitter;
    FIFE::ParticleManager* manager = nullptr;
    FIFE::ParticleEmitter* victim  = nullptr;
    void update(float dt) override
    {
        FIFE::ParticleEmitter::update(dt);
        manager->removeEmitter(victim);
    }
};
} // namespace

TEST_CASE("manager takes fixed 1/120 s steps and clamps a frame to 50 ms", "[particle]")
{
    TestFixture fixture;
    FIFE::ParticleManager manager;
    auto* e = manager.createEmitter(fastConfig());
    CHECK(manager.getPeriod() == 0);
    manager.updateEvent(100);
    CHECK(e->getEmittedCount() == 6);
    manager.updateEvent(8);
    CHECK(e->getEmittedCount() == 6);
    manager.updateEvent(9);
    CHECK(e->getEmittedCount() == 8);
}

TEST_CASE("manager resets its last update time", "[particle]")
{
    TestFixture fixture;
    FIFE::ParticleManager manager;
    auto* e = manager.createEmitter(fastConfig());
    manager.setLastUpdateTime64(0);
    manager.resetClock();
    CHECK(manager.getLastUpdateTime64() == FIFE::TimeManager::instance()->now64());

    auto const emitted = e->getEmittedCount();
    manager.updateEvent(1000);                     // a huge gap must still be clamped
    CHECK(e->getEmittedCount() - emitted <= 6);
}

TEST_CASE("emitters can be removed during a tick", "[particle]")
{
    TestFixture fixture;
    FIFE::ParticleManager manager;
    auto cfg = fastConfig();
    auto* victim = manager.createEmitter(cfg);
    auto owned   = std::make_unique<MutatingEmitter>(cfg);
    auto* mut    = owned.get();
    manager.adoptEmitter(std::move(owned));
    mut->manager = &manager;
    mut->victim = victim;
    CHECK_NOTHROW(manager.updateEvent(50));
    CHECK(manager.getEmitterCount() == 1);
    CHECK(manager.getEmitters().at(0) == mut);
}
