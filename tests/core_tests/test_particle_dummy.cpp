// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <catch2/catch_test_macros.hpp>
#include "controller/engine.h"
#include "model/model.h"
#include "model/structures/map.h"
#include "model/structures/particlemanager.h"
#include "view/camera.h"
#include "view/renderers/particlerenderer.h"

TEST_CASE("particle path is a safe no-op on dummy backends", "[dummy][particle]")
{
    for (char const* backendName : {"DUMMY_NULL", "DUMMY_SURFACE"}) {
        FIFE::Engine engine;
        engine.getSettings().setRenderBackend(backendName);
        engine.getSettings().setScreenWidth(640);
        engine.getSettings().setScreenHeight(480);
        CHECK_NOTHROW(engine.init());
        REQUIRE(engine.getParticleManager() != nullptr);
        auto* map = engine.getModel()->createMap("pd_map");
        auto* layer = map->createLayer("pd_layer", engine.getModel()->getCellGrid("square"));
        auto* cam = map->addCamera("pd_cam", FIFE::Rect(0, 0, 640, 480));
        cam->setLocation(FIFE::Location(layer));
        auto* renderer = FIFE::ParticleRenderer::getInstance(cam);
        REQUIRE(renderer != nullptr);
        renderer->setEnabled(true);
        renderer->activateAllLayers(map);
        auto* emitter = engine.getParticleManager()->createEmitter(FIFE::ParticleConfig{});
        emitter->attachToPoint(FIFE::Point(10, 10));
        emitter->update(0.25F);
        CHECK_NOTHROW(engine.pump());
        CHECK(engine.getRenderBackend() != nullptr);
        CHECK(engine.getWindow() == nullptr);
        engine.destroy();
    }
}
