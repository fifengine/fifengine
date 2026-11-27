// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>

#include <algorithm>
#include <vector>

#include "controller/engine.h"
#include "fixture.h"
#include "model/metamodel/grids/squaregrid.h"
#include "model/structures/map.h"
#include "model/structures/particleemitter.h"
#include "model/structures/particlemanager.h"
#include "util/base/exception.h"
#include "video/dummy/dummyimage.h"
#include "video/dummy/renderbackendnull.h"
#include "video/glcore/glcontext.h"
#include "video/glcore/glcore_draw.h"
#include "video/glcore/glcoreimage.h"
#include "video/glcore/particle_render_backend.h"
#include "video/iparticlerenderbackend.h"
#include "video/window/window.h"
#include "view/camera.h"
#include "view/renderers/particlerenderer.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

namespace
{
    class RecordingParticleBackend final : public FIFE::IParticleRenderBackend
    {
        public:
            uint32_t clampCapacity(uint32_t requested) const override
            {
                uint32_t cap = requested < 262144U ? requested : 262144U;
                if (ssboMaxBytes > 0) {
                    cap = static_cast<uint32_t>(std::min<size_t>(cap, ssboMaxBytes / sizeof(FIFE::ParticleGpuRecord)));
                }
                return cap;
            }
            void setEmitterCapacity(uint32_t, uint32_t) override
            {
                ++capacityCalls;
            }
            void uploadNewParticles(uint32_t, FIFE::ParticleGpuRecord const *, uint32_t, uint32_t) override
            {
                ++uploadCalls;
            }
            void releaseEmitter(uint32_t) override
            {
                ++releaseCalls;
            }
            void dispatch(uint32_t, uint32_t, float, uint64_t step) override
            {
                if (lastStep != step) {
                    lastStep = step;
                    ++dispatchCalls;
                }
            }
            void draw(uint32_t, uint32_t, FIFE::Image const &, float const *, FIFE::Rect const &) override
            {
                ++drawCalls;
            }
            void onContextRecreated() override
            {
                ++contextRecreations;
            }

            mutable uint32_t ssboMaxBytes{0};
            uint32_t capacityCalls{0};
            uint32_t uploadCalls{0};
            uint32_t releaseCalls{0};
            uint32_t dispatchCalls{0};
            uint32_t drawCalls{0};
            uint32_t contextRecreations{0};
            uint64_t lastStep{UINT64_MAX};
    };

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

TEST_CASE("CPU owns spawn/recycle/RNG and the GPU path uploads every spawn", "[modern-gl][gpu-particles]")
{
    RecordingParticleBackend bridge;
    FIFE::ParticleConfig const c = cfg();
    FIFE::ParticleEmitter cpu{c};
    FIFE::ParticleEmitter gpu{c};
    cpu.setSeed(12345);
    gpu.setSeed(12345);
    gpu.setComputeEnabled(true);
    gpu.setGpuBridge(&bridge);
    gpu.setMaxParticles(1024);

    for (int i = 0; i < 120; ++i) {
        cpu.update(1.0F / 120.0F);
        gpu.update(1.0F / 120.0F);
    }

    CHECK(gpu.getSpawnedCount() == cpu.getSpawnedCount());
    CHECK(gpu.getDeathSchedule() == cpu.getDeathSchedule());
    CHECK(bridge.uploadCalls == gpu.getSpawnedCount());
}

TEST_CASE("GPU caps diverge and drop-on-full holds", "[modern-gl][gpu-particles]")
{
    RecordingParticleBackend bridge;
    FIFE::ParticleConfig const c = cfg();
    FIFE::ParticleEmitter cpu{c};
    cpu.setMaxParticles(5000);
    CHECK(cpu.getCapacity() == 2048U);

    FIFE::ParticleEmitter gpu{c};
    gpu.setComputeEnabled(true);
    gpu.setGpuBridge(&bridge);
    gpu.setMaxParticles(5000);
    CHECK(gpu.getCapacity() == 5000U);
    gpu.setMaxParticles(300000);
    CHECK(gpu.getCapacity() == 262144U);
    bridge.ssboMaxBytes = 1024;
    gpu.setMaxParticles(262144);
    CHECK(gpu.getCapacity() == 1024 / sizeof(FIFE::ParticleGpuRecord));
    CHECK(gpu.getAliveEstimate() <= gpu.getCapacity());
}

TEST_CASE("SSBOs reallocate only on explicit invalidation triggers", "[modern-gl][gpu-particles]")
{
    RecordingParticleBackend bridge;
    FIFE::ParticleEmitter emitter(cfg());
    emitter.setGpuBridge(&bridge);
    emitter.setComputeEnabled(true);
    auto baseline = bridge.capacityCalls; // cppcheck-suppress knownConditionTrueFalse
    emitter.setRate(99.0F);
    uint8_t rgba[4]{1, 2, 3, 4};
    emitter.setColor(rgba);
    emitter.setLifetime(9.0F);
    emitter.setSeed(7);
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(bridge.capacityCalls == baseline);
    CHECK(bridge.releaseCalls == 0);
    emitter.setMaxParticles(4096);
    CHECK(bridge.capacityCalls == baseline + 1);
    emitter.setComputeEnabled(false);
    CHECK(bridge.releaseCalls == 1);
    bridge.onContextRecreated();
    CHECK(bridge.contextRecreations == 1);
}

TEST_CASE("Renderer dispatch and draw are exercised through the bridge", "[modern-gl][gpu-particles]")
{
    RecordingParticleBackend bridge;
    FIFE::ParticleEmitter emitter(cfg());
    emitter.setGpuBridge(&bridge);
    emitter.setComputeEnabled(true);
    for (int i = 0; i < 10; ++i) {
        emitter.update(1.0F / 120.0F);
    }
    CHECK(bridge.uploadCalls == emitter.getSpawnedCount());
}

TEST_CASE("GLCore bridge executes real compute and instanced draw calls", "[modern-gl][gpu-particles][opengl]")
{
#if defined(HAVE_OPENGL_43) || defined(HAVE_OPENGLES_31)
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SKIP("SDL video unavailable");
    }
    FIFE::Window window;
    try {
        window.create({.width = 64, .height = 64, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL context not available in this environment");
    }
    FIFE::GLContext ctx(
    #if defined(HAVE_OPENGL_43)
        FIFE::makeCore43Config()
    #else
        FIFE::makeES31Config()
    #endif
    );
    ctx.applyAttributes();
    if (!ctx.create(window.getSDLWindow())) {
        SKIP("Tier B context not available in this environment");
    }
    while (FIFE::glcore_draw::lastGlError() != 0) {
    }

    FIFE::GLCoreParticleRenderBackend bridge(ctx.config().profile);
    bridge.setEmitterCapacity(1, 16);
    FIFE::ParticleGpuRecord record{};
    record.position[0] = 0.0F;
    record.position[1] = 0.0F;
    record.velocity[0] = 0.01F;
    record.velocity[1] = 0.0F;
    bridge.uploadNewParticles(1, &record, 1, 0);
    bridge.dispatch(1, 1, 1.0F / 60.0F, 1);
    CHECK(bridge.lastDispatchStep(1) == 1);
    bridge.onContextRecreated();
    CHECK(bridge.lastDispatchStep(1) == 0); // dispatch state reset with GPU resources
    float const projection[16] = {
        1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F};
    FIFE::GLCoreImage atlas;
    bridge.draw(1, 1, atlas, projection, FIFE::Rect(0, 0, 64, 64));
    CHECK(FIFE::glcore_draw::lastGlError() == 0);
    SDL_GL_MakeCurrent(nullptr, nullptr);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
#else
    SKIP("Tier B profile not compiled in");
#endif
}

TEST_CASE("Dispatch follows simulation steps, not cameras or layers", "[modern-gl][gpu-particles][dispatch]")
{
    TestFixture fixture;

    class MockBackend final : public FIFE::RenderBackendNull
    {
        public:
            using FIFE::RenderBackendNull::RenderBackendNull;
            RecordingParticleBackend mock;
            FIFE::IParticleRenderBackend* getParticleRenderBackend() override
            {
                return &mock;
            }
    } backend({0, 0, 0, 255}, 640, 480);
    backend.init("");
    backend.createMainScreen("dispatch", "");

    FIFE::ParticleManager manager;
    FIFE::ParticleEmitter* emitter = manager.createEmitter(cfg());
    emitter->setComputeEnabled(true);

    FIFE::SquareGrid grid;
    FIFE::Map map("dispatch_map", &backend, {});
    FIFE::Layer* layer = map.createLayer("dispatch_layer", &grid);
    FIFE::Camera cam1("cam1", &map, FIFE::Rect(0, 0, 640, 480), &backend);
    FIFE::Camera cam2("cam2", &map, FIFE::Rect(0, 0, 320, 240), &backend);
    FIFE::Layer* otherLayer = map.createLayer("other_layer", &grid);
    FIFE::ParticleRenderer renderer(&backend, 15);
    FIFE::RenderList list;

    renderer.render(&cam1, layer, list);
    renderer.render(&cam2, layer, list);
    renderer.render(&cam1, otherLayer, list);
    CHECK(backend.mock.dispatchCalls == 1);

    manager.updateEvent(9); // one fixed step
    renderer.render(&cam1, layer, list);
    CHECK(backend.mock.dispatchCalls == 2);

    manager.updateEvent(9); // second fixed step, same rendered frame interval
    emitter->addFrame(FIFE::ImagePtr(new FIFE::DummyImage("dispatch_frame")));
    renderer.render(&cam1, layer, list);
    renderer.render(&cam2, layer, list);
    CHECK(backend.mock.dispatchCalls == 3);
    CHECK(backend.mock.drawCalls == 2); // one draw per camera/render pass
}

TEST_CASE("Non-GLCore backends expose the null particle bridge", "[modern-gl][gpu-particles]")
{
    FIFE::RenderBackendNull backend({0, 0, 0, 255}, 64, 64);
    CHECK(backend.getParticleRenderBackend() == nullptr);
}
