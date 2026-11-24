// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <algorithm>
#include <array>

#include "fixture.h"
#include "model/metamodel/grids/squaregrid.h"
#include "model/structures/map.h"
#include "model/structures/particlemanager.h"
#include "video/dummy/dummyimage.h"
#include "video/dummy/renderbackendnull.h"
#include "view/camera.h"
#include "view/renderers/particlerenderer.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha): clang-format groups Catch2 with project headers

namespace
{
    struct CountingImage : FIFE::DummyImage
    {
            int calls = 0;
            FIFE::Rect lastRect;
            uint8_t lastAlpha = 0;
            std::array<uint8_t, 4> lastRgba{};
            using FIFE::DummyImage::DummyImage;
            void render(FIFE::Rect const & rect, uint8_t alpha = 255, uint8_t const * rgb = nullptr) override
            {
                ++calls;
                lastRect  = rect;
                lastAlpha = alpha;
                if (rgb != nullptr) {
                    std::copy(rgb, rgb + 4, lastRgba.begin());
                }
            }
    };
} // namespace

TEST_CASE("renderer draws each live particle through Image::render only on its layer", "[particle]")
{
    TestFixture fixture;
    FIFE::RenderBackendNull backend({0, 0, 0, 255}, 640, 480);
    backend.init("");
    backend.createMainScreen("pr", "");
    FIFE::ParticleManager manager;
    FIFE::Map map("pr_map", &backend, {});
    FIFE::SquareGrid grid;
    FIFE::Layer* layer = map.createLayer("pr_layer", &grid);
    FIFE::Layer* other = map.createLayer("pr_other", &grid);
    FIFE::Camera* cam  = map.addCamera("pr_cam", FIFE::Rect(0, 0, 640, 480));
    cam->setLocation(FIFE::Location(layer));

    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 4.0F;
    cfg.lifetimeMin = cfg.lifetimeMax = 10.0F;
    cfg.maxParticles                  = 2;
    cfg.startSize = cfg.endSize = 10.0F;
    cfg.startColor[0]           = 255;
    cfg.startColor[1] = cfg.startColor[2] = 0;
    cfg.startColor[3]                     = 255;
    cfg.endColor[0]                       = 255;
    cfg.endColor[1] = cfg.endColor[2] = 0;
    cfg.endColor[3]                   = 255;
    auto* emitter                     = manager.createEmitter(cfg);
    emitter->setSeed(1);
    emitter->attachToLayer(layer, FIFE::Point(100, 100));
    emitter->update(0.25F);
    auto frame = FIFE::ImagePtr(new CountingImage("pr_frame"));
    emitter->setFrames({frame});

    FIFE::ParticleRenderer renderer(&backend, 15);
    FIFE::RenderList list;
    renderer.render(cam, other, list);
    CHECK(static_cast<CountingImage const *>(frame.get())->calls == 0);

    renderer.render(cam, layer, list);
    auto const * image = static_cast<CountingImage const *>(frame.get());
    CHECK(image->calls == 1);
    auto const sp = cam->toScreenCoordinates(FIFE::ExactModelCoordinate(100, 100, 0));
    CHECK(image->lastRect.x == sp.x - 5);
    CHECK(image->lastRect.y == sp.y - 5);
    CHECK(image->lastRect.w == 10);
    CHECK(image->lastRect.h == 10);
    CHECK(image->lastAlpha == 255);
    CHECK(image->lastRgba[0] == 255);

    auto const emitted = emitter->getEmittedCount();
    renderer.render(cam, layer, list);
    renderer.reset();
    renderer.render(cam, layer, list);
    CHECK(emitter->getEmittedCount() == emitted);
    CHECK(image->calls == 3);
}
