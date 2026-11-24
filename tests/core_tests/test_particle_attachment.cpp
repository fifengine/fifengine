// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <string>
#include <vector>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include "model/metamodel/grids/squaregrid.h"
#include "model/metamodel/modelcoords.h"
#include "model/metamodel/object.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/particleemitter.h"
#include "video/dummy/dummyimage.h"

namespace
{
FIFE::ParticleConfig placementConfig(bool local)
{
    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 4.0F; cfg.lifetimeMin = cfg.lifetimeMax = 10.0F;
    cfg.maxParticles = 4; cfg.localSpace = local;
    return cfg;
}
} // namespace

TEST_CASE("world-space particles stay put; localSpace particles follow the node", "[particle]")
{
    FIFE::SquareGrid grid;
    FIFE::Layer layer("pl_layer", nullptr, &grid);
    FIFE::Object obj("pl_obj", "test");
    FIFE::Instance* inst = layer.createInstance(&obj, FIFE::ModelCoordinate(2, 3, 0));

    FIFE::ParticleEmitter world(placementConfig(false));
    world.setSeed(1);
    world.attachToInstance(inst);
    world.update(0.25F);
    CHECK(world.getParticleWorldPosition(0).x == Catch::Approx(2.0));
    CHECK(world.getParticleWorldPosition(0).y == Catch::Approx(3.0));

    FIFE::Location moved(&layer);
    moved.setLayerCoordinates(FIFE::ModelCoordinate(5, 3, 0));
    inst->setLocation(moved);
    CHECK(world.getParticleWorldPosition(0).x == Catch::Approx(2.0));

    FIFE::ParticleEmitter localc(placementConfig(true));
    localc.setSeed(1);
    localc.attachToInstance(inst);
    localc.update(0.25F);
    CHECK(localc.getParticleWorldPosition(0).x == Catch::Approx(5.0));
}

TEST_CASE("deleting the attached instance stops emission and keeps the drawn layer", "[particle]")
{
    FIFE::SquareGrid grid;
    FIFE::Layer layer("pl_layer2", nullptr, &grid);
    FIFE::Layer other("pl_other", nullptr, &grid);
    FIFE::Object obj("pl_obj2", "test");
    FIFE::Instance* inst = layer.createInstance(&obj, FIFE::ModelCoordinate(2, 3, 0));

    FIFE::ParticleEmitter e(placementConfig(false));
    e.setSeed(1);
    e.attachToInstance(inst);
    e.update(0.25F);
    auto const before = e.getParticleWorldPosition(0);
    CHECK(e.isDrawnOn(&layer));
    CHECK_FALSE(e.isDrawnOn(&other));

    layer.deleteInstance(inst);
    CHECK_FALSE(e.isEmissionEnabled());
    CHECK(e.getAttachedInstance() == nullptr);
    CHECK(e.isDrawnOn(&layer));
    CHECK_FALSE(e.isDrawnOn(&other));
    CHECK(e.getParticleWorldPosition(0).x == Catch::Approx(before.x));
    auto const emitted = e.getEmittedCount();
    e.update(0.25F);
    CHECK(e.getEmittedCount() == emitted);

    FIFE::Instance* inst2 = layer.createInstance(&obj, FIFE::ModelCoordinate(8, 8, 0));
    e.attachToInstance(inst2);
    CHECK(e.isEmissionEnabled());
    e.update(0.25F);
    CHECK(e.getEmittedCount() == emitted + 1);
}

TEST_CASE("emitter exposes atlas sub-images with modulo frame lookup", "[particle]")
{
    FIFE::ParticleEmitter e(placementConfig(false));
    auto atlas = FIFE::ImagePtr(new FIFE::DummyImage("pl_atlas"));
    std::vector<FIFE::ImagePtr> frames;
    for (int i = 0; i < 2; ++i) {
        auto frame = FIFE::ImagePtr(new FIFE::DummyImage("pl_frame" + std::to_string(i)));
        frame->useSharedImage(atlas, FIFE::Rect(i * 8, 0, 8, 8));
        frames.push_back(frame);
    }
    e.setFrames(frames);
    CHECK(e.getFrameCount() == 2);
    CHECK(e.getFrame(0).get() == frames[0].get());
    CHECK(e.getFrame(5).get() == frames[1].get());
}
