# Backend-agnostic particle engine Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Ship a simple, scriptable, backend-agnostic particle engine: model-side simulation ticked once per frame, drawn in the view through `Image::render()`.

**Architecture:** A model-side `ParticleManager` owns `ParticleEmitter`s (config, preallocated pool, seeded RNG, `RendererNode` placement) and ticks them through a period-0 `TimeEvent` with a fixed 1/120 s step. A view-side `ParticleRenderer` clones into cameras like every other renderer and turns each live particle into a `Rect` drawn through the current atlas sub-image's `Image::render(rect, alpha, rgba)`; no backend code changes. `Engine::init()` creates the manager and renderer prototype; CMake/SWIG registration and a demo script complete the feature.

**Tech Stack:** C++20, Catch2 (`all_tests` / `all_tests_dummy`), SWIG + pytest (`tests/swig_tests/`), SDL3, four existing backends (`SDL`, `OpenGL`, `DUMMY_NULL`, `DUMMY_SURFACE`), FIFE conventions (`RendererBase`, `RendererNode`, `TimeEvent`, `Image::useSharedImage()`).

**Spec:** `docs/superpowers/specs/2026-11-23-particle-engine-design.md`

## Global Constraints

- Particles are **world-space by default**; `Config::localSpace = true` is the only opt-in to emitter-relative offsets.
- Simulation advances in **fixed steps of 1/120 s**.
- **Clamp any single frame delta to 50 ms** before accumulation.
- `maxParticles` defaults to `ceil(rate * maxLifetime) + burstCount` and is **clamped to 2048**.
- When the pool is full, **new emissions are dropped**; live particles are never recycled early.
- The RNG is a **seeded `std::mt19937`**; a given seed reproduces the same particle sequence.
- **No angular-velocity field in v1** — arbitrary per-particle geometry rotation and additive blending are non-goals; pre-rotated atlas frames are the v1 workaround.
- **No `RenderBackend` changes** — not even a new non-pure virtual in v1.
- **No GL/SDL types in model** simulation code.
- **`Image::render()` is the only drawing path**; it stays out of Python.
- Backends: `SDL`, `OpenGL` (falls back to `SDL` without `HAVE_OPENGL`), `DUMMY_NULL`, `DUMMY_SURFACE`; headless drawing must be a safe no-op.
- C++20 (`CXX_STANDARD 20` on test targets); tests are Catch2 `all_tests` / `all_tests_dummy` and pytest under `tests/swig_tests/`.
- Singleton teardown: `DynamicSingleton::instance()` asserts when null; unregister the `TimeEvent` before `TimeManager` is destroyed and guard renderer access with `ParticleManager::exists()`.

## Review Focus

- Deleting an attached instance must stop emission and keep the plume on its original drawn layer — otherwise `isDrawnOn()` falls through to "every layer" and it is redrawn at (0,0); pinned in Task 3.
- A long init-to-first-frame gap (or engine re-init) must not deliver the whole gap as one first tick; pinned by `resetClock()` in Task 2.
- `Camera::render()` calls each renderer once per layer, so calling `render()` must never advance the simulation; pinned in Task 4.
- A burst larger than `maxParticles` must respect the cap without recycling live particles and must not inflate the reported emitted count; pinned in Task 1.
- Under `DUMMY_NULL` / `DUMMY_SURFACE` the `Image::render()` path must not throw or assert without a window or GL context; pinned in Task 5.

---

## File Structure

Model (new):
- `src/fife/model/structures/particleconfig.h` — `ParticleShape`, `ParticleConfig` POD, `ParticleSnapshot` POD.
- `src/fife/model/structures/particleemitter.h/.cpp` — pool, seeded RNG, simulation, `RendererNode` placement, attachment/deletion policy, atlas frames.
- `src/fife/model/structures/particlemanager.h/.cpp` — emitter ownership, period-0 `TimeEvent`, fixed-step accumulator, lifecycle.

View (new):
- `src/fife/view/renderers/particlerenderer.h/.cpp` — `RendererBase` subclass; layer filtering, camera transform, atlas frames, `Image::render()`.
- `src/fife/view/renderers/particlerenderer.i` — SWIG declarations for renderer, manager, emitter, config, enums.

Wiring (modify):
- `src/fife/controller/engine.h/.cpp` — manager creation/teardown, renderer prototype, accessor, forward declarations.
- `src/fife/controller/engine.i` — expose `getParticleManager()`.
- `CMakeLists.txt` — source list (`:407-423`, `:502-514`), header list (`:630-750`), SWIG list (`:825-898`).
- `src/python/fife/fife.pyi` — minimal typing surface for new classes.
- `tests/fife_test/tests/ParticleTest.py`, `tests/fife_test/settings.xml`, `tests/fife_test/settings-dist.xml` — runnable visual demo + registration.

Tests (new, each added to `FIFE_CORE_TEST_SOURCES` in `tests/core_tests/CMakeLists.txt` by its owning task):
- `tests/core_tests/test_particle_emitter.cpp` — determinism, capacity, overflow, interpolation.
- `tests/core_tests/test_particle_manager.cpp` — tick cadence, clamp, clock reset, mutation during tick.
- `tests/core_tests/test_particle_attachment.cpp` — world/local space, instance deletion, atlas frames.
- `tests/core_tests/test_particle_renderer.cpp` — draw path, layer filtering, no tick in `render()`, `reset()`.
- `tests/core_tests/test_particle_dummy.cpp` — `[dummy]` safety on both headless backends.
- `tests/swig_tests/test_particle.py` — Python API, pump, `getInstance(cam)`.

---

### Task 1: ParticleConfig, seeded RNG, pool and simulation

**Files:**
- Create: `src/fife/model/structures/particleconfig.h`, `src/fife/model/structures/particleemitter.h`, `src/fife/model/structures/particleemitter.cpp`
- Modify: `CMakeLists.txt` (source list at `:421`, header list at `:637`)
- Test: `tests/core_tests/test_particle_emitter.cpp` (create), `tests/core_tests/CMakeLists.txt`

**Interfaces:**
- Consumes: `FIFE::ExactModelCoordinate` (`model/metamodel/modelcoords.h`), `FIFE::ImagePtr` / `Rect` (`video/image.h`, `util/structures/rect.h`), Catch2 `Approx` (`catch2/catch_approx.hpp`).
- Produces: `enum class ParticleShape { Point, Box, Circle };` and `struct ParticleConfig` with fields `float emissionRate`, `uint32_t burstCount`, `float burstInterval`, `ParticleShape shape`, `float shapeWidth`, `float shapeHeight`, `float lifetimeMin`, `float lifetimeMax`, `float speedMin`, `float speedMax`, `float gravity`, `float drag`, `float startSize`, `float endSize`, `uint8_t startColor[4]`, `uint8_t endColor[4]`, `uint32_t frameCount`, `float frameDuration`, `bool localSpace`, `uint32_t maxParticles` (`0` = default formula), `bool looping`. `struct ParticleSnapshot { bool alive; float x, y, vx, vy, age, lifetime, size; uint8_t rgba[4]; uint32_t frame; };`. `class ParticleEmitter` with `explicit ParticleEmitter(ParticleConfig const& config)`, `virtual ~ParticleEmitter()`, `void setSeed(uint32_t seed)`, `virtual void update(float dt)`, `size_t getAliveCount() const`, `size_t getEmittedCount() const`, `uint32_t getCapacity() const`, `uint32_t getMaxParticles() const` (returns the effective cap), `void setMaxParticles(uint32_t maxParticles)` (updates config and resizes the pool, applying the 2048 clamp on the CPU path), `ParticleSnapshot getParticle(size_t slot) const`, `ExactModelCoordinate getParticleWorldPosition(size_t slot) const`.

- [ ] **Step 1: Write the failing test**

```cpp
// tests/core_tests/test_particle_emitter.cpp
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
    CHECK(a.getParticle(0).x == Catch::Approx(0.25));
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
```

- [ ] **Step 2: Add the test file to the build**

Add `test_particle_emitter.cpp` to `FIFE_CORE_TEST_SOURCES` in `tests/core_tests/CMakeLists.txt`.

- [ ] **Step 3: Run it to verify it fails**

Run: `cmake --build <BUILD_DIR> --target all_tests && <BUILD_DIR>/tests/core_tests/all_tests "[particle]"`
Expected: compile failure — `model/structures/particleemitter.h` not found.

- [ ] **Step 4: Implement the model PODs and `ParticleEmitter` in the exact files above**

Use `std::vector<Particle>` of `getCapacity()` entries plus `std::vector<size_t> m_free` as a free-index stack; `getParticle(slot)` returns the stored snapshot (including `alive == false` for free slots). Integration order per step: `v *= max(0, 1 - drag*dt)`, `v.y += gravity*dt`, `pos += v*dt`, `age += dt`, then recycle when `age >= lifetime`; emit before integration, and fire a burst when a separate burst accumulator reaches `burstInterval`. Interpolate colour/size by `age / lifetime` with `std::lround` for bytes. Step `dt` directly (no accumulation here — that is Task 2). Enforce `maxParticles == 0 ? min(ceil(rate * maxLifetime) + burstCount, 2048) : min(maxParticles, 2048)`; count only successful allocations in `getEmittedCount()`. `getMaxParticles()` returns the effective cap and `setMaxParticles()` updates the config, re-applies the clamp and rebuilds the pool plus free-index stack. Frame index is `frameCount == 0 ? 0 : (uint32_t)(age / frameDuration) % frameCount`; when `looping == false`, only the first burst fires and continuous emission stops. `getParticleWorldPosition(slot)` returns `ExactModelCoordinate(p.x, p.y, 0)` for now (Task 3 replaces the body with node-aware placement). In Task 1, `ParticleEmitter` has no renderer code, GL/SDL types or `ImagePtr` members.

- [ ] **Step 5: Run tests to verify they pass**

Run: `cmake --build <BUILD_DIR> --target all_tests && <BUILD_DIR>/tests/core_tests/all_tests "[particle]"`
Expected: all six `[particle]` tests PASS.

- [ ] **Step 6: Commit**

```bash
git add src/fife/model/structures/particleconfig.h \
  src/fife/model/structures/particleemitter.h src/fife/model/structures/particleemitter.cpp \
  CMakeLists.txt tests/core_tests/test_particle_emitter.cpp tests/core_tests/CMakeLists.txt
git commit -m "feat(particle): add seeded emitter pool and simulation"
```

---

### Task 2: ParticleManager ownership and once-per-frame fixed-step tick

**Files:**
- Create: `src/fife/model/structures/particlemanager.h`, `src/fife/model/structures/particlemanager.cpp`
- Modify: `CMakeLists.txt` (source list at `:421`, header list at `:637`)
- Test: `tests/core_tests/test_particle_manager.cpp` (create), `tests/core_tests/CMakeLists.txt`

**Interfaces:**
- Consumes: `ParticleEmitter(ParticleConfig const&)`, `virtual void update(float dt)`, `ParticleEmitter::getEmittedCount() const` (Task 1); `TimeEvent(int32_t period)`, `updateEvent(uint32_t time)`, `setLastUpdateTime64(uint64_t)`, `getLastUpdateTime64()`, `registerEvent`, `unregisterEvent` (`util/time/timeevent.h`, `timemanager.h`); `TestFixture` (`tests/core_tests/fixture.h`).
- Produces: `class ParticleManager : public TimeEvent, public DynamicSingleton<ParticleManager>` with `ParticleManager()`, `~ParticleManager() override`, `static ParticleManager* getInstance()`, `ParticleEmitter* createEmitter(ParticleConfig const& config)`, `ParticleEmitter* adoptEmitter(std::unique_ptr<ParticleEmitter> emitter)`, `void removeEmitter(ParticleEmitter* emitter)`, `size_t getEmitterCount() const`, `std::vector<ParticleEmitter*> const& getEmitters() const`, `void resetClock()`, `void setSeed(uint32_t seed)`, `void updateEvent(uint32_t time) override`.

- [ ] **Step 1: Write the failing test**

```cpp
// tests/core_tests/test_particle_manager.cpp
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include "fixture.h"
#include "model/structures/particlemanager.h"
#include "util/time/timemanager.h"

namespace {
FIFE::ParticleConfig fastConfig()
{
    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 120.0F; cfg.lifetimeMin = cfg.lifetimeMax = 10.0F; cfg.maxParticles = 1024;
    return cfg;
}
struct MutatingEmitter : FIFE::ParticleEmitter {
    using FIFE::ParticleEmitter::ParticleEmitter;
    FIFE::ParticleManager* manager = nullptr;
    FIFE::ParticleEmitter* victim = nullptr;
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
    FIFE::ParticleManager manager;                 // stamps now64() while it is still 0
    auto* e = manager.createEmitter(fastConfig());
    fixture.tm->update();                          // now64() advances to real SDL ticks
    auto const now = FIFE::TimeManager::instance()->now64();
    REQUIRE(now > 0);
    manager.resetClock();
    CHECK(manager.getLastUpdateTime64() == now);   // fails if resetClock is a no-op
    auto const emitted = e->getEmittedCount();
    fixture.tm->update();
    CHECK(e->getEmittedCount() - emitted <= 6);    // any gap is clamped, never one huge step
}

TEST_CASE("emitters can be removed during a tick", "[particle]")
{
    TestFixture fixture;
    FIFE::ParticleManager manager;
    auto cfg = fastConfig();
    auto* victim = manager.createEmitter(cfg);
    auto mut = manager.adoptEmitter(std::make_unique<MutatingEmitter>(cfg));
    mut->manager = &manager;
    mut->victim = victim;
    CHECK_NOTHROW(manager.updateEvent(50));
    CHECK(manager.getEmitterCount() == 1);
    CHECK(manager.getEmitters().at(0) == mut);
}
```

- [ ] **Step 2: Add the test file to the build**

Add `test_particle_manager.cpp` to `FIFE_CORE_TEST_SOURCES` in `tests/core_tests/CMakeLists.txt`.

- [ ] **Step 3: Run it to verify it fails**

Run: `cmake --build <BUILD_DIR> --target all_tests && <BUILD_DIR>/tests/core_tests/all_tests "[particle]"`
Expected: compile failure — `model/structures/particlemanager.h` not found.

- [ ] **Step 4: Implement `ParticleManager` in the exact files above**

Constructor: `TimeEvent(0)`, `TimeManager::instance()->registerEvent(this)`, then `resetClock()` (this explicit reset is the pin for the gap input). Destructor: `TimeManager::exists()` guard then `unregisterEvent(this)`. `resetClock()` sets `setLastUpdateTime64(TimeManager::instance()->now64())`. `updateEvent(uint32_t time)` clamps `time` to 50 ms, adds `min(time,50) / 1000.0` to `m_accumulator`, and calls `update(1.0F/120.0F)` on every emitter while `m_accumulator >= 1.0/120`. Maintain `m_emitters` (owning) and `m_emitters_view` (raw pointers); `removeEmitter()` appends the pointer to `m_pending_removal` and erases it only at the end of `updateEvent()`, so `createEmitter()`/`adoptEmitter()` during a tick cannot invalidate iteration. `setSeed()` forwards to every emitter. `getInstance()` returns `instance()`.

- [ ] **Step 5: Run tests to verify they pass**

Run: `cmake --build <BUILD_DIR> --target all_tests && <BUILD_DIR>/tests/core_tests/all_tests "[particle]"`
Expected: all nine `[particle]` tests PASS.

- [ ] **Step 6: Commit**

```bash
git add src/fife/model/structures/particlemanager.h src/fife/model/structures/particlemanager.cpp \
  CMakeLists.txt tests/core_tests/test_particle_manager.cpp tests/core_tests/CMakeLists.txt
git commit -m "feat(particle): add manager with fixed-step TimeEvent tick"
```

---

### Task 3: RendererNode placement, world/local space, deletion policy and atlas frames

**Files:**
- Modify: `src/fife/model/structures/particleemitter.h`, `src/fife/model/structures/particleemitter.cpp`
- Test: `tests/core_tests/test_particle_attachment.cpp` (create), `tests/core_tests/CMakeLists.txt`

**Interfaces:**
- Consumes: `ParticleEmitter::update(float)`, `getParticleWorldPosition(size_t)`, `getCapacity()`, `getEmittedCount()` (Task 1); `RendererNode` (`model/structures/renderernode.h`), `InstanceDeleteListener`, `Layer::deleteInstance(Instance*)`, `Instance::setLocation(Location const&)`, `Image::useSharedImage(ImagePtr const&, Rect const&)` (`video/image.h:190`).
- Produces: additional `ParticleEmitter` methods `void attachToInstance(Instance* instance, Point const& offset = Point(0,0))`, `void attachToLayer(Layer* layer, Point const& offset = Point(0,0))`, `void attachToPoint(Point const& point)`, `void detach()`, `bool isDrawnOn(Layer const* layer) const`, `bool isEmissionEnabled() const`, `Instance* getAttachedInstance() const`, `ExactModelCoordinate getNodeWorldPosition() const`, `void setFrames(std::vector<ImagePtr> frames)`, `size_t getFrameCount() const`, `ImagePtr const& getFrame(uint32_t frame) const`; `getParticleWorldPosition(slot)` now returns the position the renderer must transform.

- [ ] **Step 1: Write the failing test**

```cpp
// tests/core_tests/test_particle_attachment.cpp
#include <array>
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

namespace {
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
```

- [ ] **Step 2: Add the test file to the build**

Add `test_particle_attachment.cpp` to `FIFE_CORE_TEST_SOURCES` in `tests/core_tests/CMakeLists.txt`.

- [ ] **Step 3: Run it to verify it fails**

Run: `cmake --build <BUILD_DIR> --target all_tests && <BUILD_DIR>/tests/core_tests/all_tests "[particle]"`
Expected: compile failure — `attachToInstance`, `setFrames` are not members of `ParticleEmitter`.

- [ ] **Step 4: Implement placement and frames in `particleemitter.h/.cpp`**

Own a `RendererNode m_node{Point(0,0)}` and a separate `InstanceDeleteListener` registered on the attached instance; on `onInstanceDeleted`, capture `getNodeWorldPosition()` into `m_last_world`, set `m_emission_enabled = false`, and clear the node attachment with `m_node.setAttached(Point(0,0))`. Store `m_drawn_layer` at attach time: `attachToInstance` uses `instance->getLocationRef().getLayer()`, `attachToLayer` uses that layer (`m_node.setAttached(layer)` plus `setRelative(Point)` for the offset), `attachToPoint` leaves it null; `isDrawnOn(layer)` returns `m_drawn_layer == nullptr || m_drawn_layer == layer`, so it never falls through after deletion. `getNodeWorldPosition()` returns the instance's `getLocationRef().getMapCoordinates()` plus a valid offset location, else a valid attached location's map coordinates, else `ExactModelCoordinate(m_node.getPoint().x, m_node.getPoint().y, 0)`; after deletion it returns `m_last_world`. `getParticleWorldPosition(slot)` returns the stored world coordinate for a world-space particle and the stored offset plus `getNodeWorldPosition()` when `localSpace` is true. `detach()` clears the node, layer and re-enables emission; `attachTo*` re-enables it. `setFrames()` stores `ImagePtr`s; `getFrame(i)` returns `m_frames.empty() ? ImagePtr() : m_frames[i % m_frames.size()]`; keep the atlas `ImagePtr` as an owner member. Shape offsets for `Box`/`Circle` are drawn from the seeded RNG at emission time.

- [ ] **Step 5: Run tests to verify they pass**

Run: `cmake --build <BUILD_DIR> --target all_tests && <BUILD_DIR>/tests/core_tests/all_tests "[particle]"`
Expected: all twelve `[particle]` tests PASS.

- [ ] **Step 6: Commit**

```bash
git add src/fife/model/structures/particleemitter.h src/fife/model/structures/particleemitter.cpp \
  tests/core_tests/test_particle_attachment.cpp tests/core_tests/CMakeLists.txt
git commit -m "feat(particle): add RendererNode placement and deletion policy"
```

---

### Task 4: ParticleRenderer draw path

**Files:**
- Create: `src/fife/view/renderers/particlerenderer.h`, `src/fife/view/renderers/particlerenderer.cpp`
- Modify: `CMakeLists.txt` (source list at `:504`, header list at `:738`)
- Test: `tests/core_tests/test_particle_renderer.cpp` (create), `tests/core_tests/CMakeLists.txt`

**Interfaces:**
- Consumes: `RendererBase(RenderBackend*, int32_t)`, pure virtual `render(Camera*, Layer*, RenderList&)`, `getName()`, `clone()`, `setEnabled`, `activateAllLayers` (`view/rendererbase.h`); `ParticleManager::exists()/instance()/getEmitters()` (Task 2); `ParticleEmitter::isDrawnOn`, `getCapacity`, `getParticle`, `getParticleWorldPosition`, `getFrame` (Tasks 1, 3); `Camera::toScreenCoordinates(ExactModelCoordinate const&) -> ScreenPoint` (`view/camera.h:264`); `Image::render(Rect const&, uint8_t alpha = 255, uint8_t const* rgb = nullptr)` (`video/image.h:68`).
- Produces: `class ParticleRenderer : public RendererBase` with `ParticleRenderer(RenderBackend* renderbackend, int32_t position)`, copy constructor, deleted copy assignment, `std::unique_ptr<RendererBase> clone() override`, `void render(Camera* cam, Layer* layer, RenderList& instances) override`, `std::string getName() override` returning `"ParticleRenderer"`, `void reset() override`, `static ParticleRenderer* getInstance(IRendererContainer* cnt)` (dynamic_cast of `cnt->getRenderer("ParticleRenderer")`).

- [ ] **Step 1: Write the failing test**

```cpp
// tests/core_tests/test_particle_renderer.cpp
#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include "fixture.h"
#include "model/structures/map.h"
#include "model/structures/particlemanager.h"
#include "model/metamodel/grids/squaregrid.h"
#include "video/dummy/dummyimage.h"
#include "video/dummy/renderbackendnull.h"
#include "view/camera.h"
#include "view/renderers/particlerenderer.h"

namespace {
struct CountingImage : FIFE::DummyImage {
    int calls = 0;
    FIFE::Rect lastRect;
    uint8_t lastAlpha = 0;
    std::array<uint8_t, 4> lastRgba{};
    using FIFE::DummyImage::DummyImage;
    void render(FIFE::Rect const& rect, uint8_t alpha = 255, uint8_t const* rgb = nullptr) override
    {
        ++calls; lastRect = rect; lastAlpha = alpha;
        if (rgb != nullptr) { std::copy(rgb, rgb + 4, lastRgba.begin()); }
    }
};
} // namespace

TEST_CASE("renderer draws each live particle through Image::render only on its layer", "[particle]")
{
    TestFixture fixture;
    FIFE::RenderBackendNull backend({0, 0, 0, 255}, 640, 480);
    backend.init(""); backend.createMainScreen("pr", "");
    FIFE::ParticleManager manager;
    FIFE::Map map("pr_map", &backend, {});
    FIFE::SquareGrid grid;
    FIFE::Layer* layer = map.createLayer("pr_layer", &grid);
    FIFE::Layer* other = map.createLayer("pr_other", &grid);
    FIFE::Camera* cam = map.addCamera("pr_cam", FIFE::Rect(0, 0, 640, 480));
    cam->setLocation(FIFE::Location(layer));

    FIFE::ParticleConfig cfg;
    cfg.emissionRate = 4.0F; cfg.lifetimeMin = cfg.lifetimeMax = 10.0F; cfg.maxParticles = 2;
    cfg.startSize = cfg.endSize = 10.0F;
    cfg.startColor[0] = 255; cfg.startColor[1] = cfg.startColor[2] = 0; cfg.startColor[3] = 255;
    cfg.endColor[0] = 255; cfg.endColor[1] = cfg.endColor[2] = 0; cfg.endColor[3] = 255;
    auto* emitter = manager.createEmitter(cfg);
    emitter->setSeed(1);
    emitter->attachToLayer(layer, FIFE::Point(100, 100));
    emitter->update(0.25F);
    auto frame = FIFE::ImagePtr(new CountingImage("pr_frame"));
    emitter->setFrames({frame});

    FIFE::ParticleRenderer renderer(&backend, 15);
    FIFE::RenderList list;
    renderer.render(cam, other, list);
    CHECK(static_cast<CountingImage const*>(frame.get())->calls == 0);

    renderer.render(cam, layer, list);
    auto const* image = static_cast<CountingImage const*>(frame.get());
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
```

- [ ] **Step 2: Add the test file to the build**

Add `test_particle_renderer.cpp` to `FIFE_CORE_TEST_SOURCES` in `tests/core_tests/CMakeLists.txt`.

- [ ] **Step 3: Run it to verify it fails**

Run: `cmake --build <BUILD_DIR> --target all_tests && <BUILD_DIR>/tests/core_tests/all_tests "[particle]"`
Expected: compile failure — `view/renderers/particlerenderer.h` not found.

- [ ] **Step 4: Implement `ParticleRenderer` in the exact files above**

`render()` returns early if `!ParticleManager::exists()`; marks `instances` unused. For each emitter with `isDrawnOn(layer)`, iterate slots `0..getCapacity()`, skip `!alive`, convert `getParticleWorldPosition(slot)` with `cam->toScreenCoordinates()`, build `Rect(sp.x - w/2, sp.y - h/2, w, h)` with `w = h = std::lround(p.size)` (minimum 1), take `getFrame(p.frame)` and call `frame->render(rect, p.rgba[3], p.rgba)` (the fourth byte is the colour-mod alpha; the API calls the pointer `rgb` but all backends read four bytes). `reset()` is a deliberate no-op: there are no per-camera caches in v1, and model state must survive it. `getInstance()` follows `instancerenderer.i` conventions.

- [ ] **Step 5: Run tests to verify they pass**

Run: `cmake --build <BUILD_DIR> --target all_tests && <BUILD_DIR>/tests/core_tests/all_tests "[particle]"`
Expected: all thirteen `[particle]` tests PASS.

- [ ] **Step 6: Commit**

```bash
git add src/fife/view/renderers/particlerenderer.h src/fife/view/renderers/particlerenderer.cpp \
  CMakeLists.txt tests/core_tests/test_particle_renderer.cpp tests/core_tests/CMakeLists.txt
git commit -m "feat(particle): add ParticleRenderer draw path"
```

---

### Task 5: Engine, CMake, SWIG wiring and dummy-backend safety

**Files:**
- Modify: `src/fife/controller/engine.h`, `src/fife/controller/engine.cpp` (includes `:59`, `:71`; `init()` `:327-328`, `:699-715`; `destroy()` `:798-801`), `src/fife/controller/engine.i:176`, `CMakeLists.txt:825-898`
- Create: `src/fife/view/renderers/particlerenderer.i`, `tests/core_tests/test_particle_dummy.cpp`
- Test: `tests/core_tests/test_particle_dummy.cpp`, `tests/core_tests/CMakeLists.txt`

**Interfaces:**
- Consumes: `Engine::init()/destroy()/pump()`, `Model::createMap`, `Map::createLayer`, `Map::addCamera`, `TimeManager` created at `engine.cpp:327`; `ParticleManager` (Task 2), `ParticleRenderer` and `ParticleRenderer::getInstance` (Task 4).
- Produces: `Engine::getParticleManager()` returning `ParticleManager*`; registered CTest target `all_tests_dummy` covers the new `[dummy]` test; `PARTICLERENDERER` CMake target registration; `fife.ParticleRenderer`, `fife.ParticleManager`, `fife.ParticleEmitter`, `fife.ParticleConfig`, `fife.ParticleShape` Python names (Task 6 depends on them).

- [ ] **Step 1: Write the failing test**

```cpp
// tests/core_tests/test_particle_dummy.cpp
#include <catch2/catch_test_macros.hpp>
#include "controller/engine.h"
#include "model/structures/map.h"
#include "model/structures/particlemanager.h"
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
```

- [ ] **Step 2: Add the test file to the build**

Add `test_particle_dummy.cpp` to `FIFE_CORE_TEST_SOURCES` in `tests/core_tests/CMakeLists.txt` (already registered by `add_test(NAME all_tests_dummy ...)` at `:154`).

- [ ] **Step 3: Run it to verify it fails**

Run: `cmake --build <BUILD_DIR> --target all_tests && <BUILD_DIR>/tests/core_tests/all_tests "[dummy][particle]"`
Expected: compile/link failure — `getParticleManager()` and `ParticleRenderer::getInstance` unresolved.

- [ ] **Step 4: Wire engine, CMake and SWIG**

In `engine.h`, forward-declare `class ParticleManager;`, add `std::unique_ptr<ParticleManager> m_particlemanager;` beside `m_timemanager` (`:335`) and `ParticleManager* getParticleManager() const { return m_particlemanager.get(); }`. In `engine.cpp`, include the manager and renderer headers near `:59`/`:71`; after `m_timemanager` creation at `:327-328` add `m_particlemanager = std::make_unique<ParticleManager>();` (its constructor registers the `TimeEvent`); after the `InstanceRenderer` prototype at `:702` add `m_renderers.push_back(std::make_unique<ParticleRenderer>(m_renderbackend.get(), 15));` — disabled by default like the other renderers, so callers enable it explicitly; before `m_timemanager.reset()` at `:801` add `m_particlemanager.reset()` so unregistration happens while `TimeManager` still exists. Register the three new source files and three new headers in the `CMakeLists.txt` lists named in Files. In `engine.i:176` add `ParticleManager* getParticleManager();`. Create `particlerenderer.i` following `instancerenderer.i`: `%{ #include ... %}` for renderer/manager/emitter/config headers, manual SWIG declarations of `ParticleShape`, `ParticleConfig` (all fields from Task 1), `ParticleEmitter` (public methods from Tasks 1 and 3), `ParticleManager` (public methods from Task 2 plus `static ParticleManager* getInstance()`), and `ParticleRenderer` with `static ParticleRenderer* getInstance(IRendererContainer* cnt)`; add it to `FIFE_SWIG` at `:887`. Do not declare `Image::render()` in SWIG.

- [ ] **Step 5: Run tests to verify they pass**

Run: `cmake --build <BUILD_DIR> --target all_tests && ctest --test-dir <BUILD_DIR> -R all_tests_dummy --output-on-failure`
Expected: `all_tests_dummy` PASSES, including the new `[dummy][particle]` case on both backends.

- [ ] **Step 6: Commit**

```bash
git add src/fife/controller/engine.h src/fife/controller/engine.cpp src/fife/controller/engine.i \
  src/fife/view/renderers/particlerenderer.i CMakeLists.txt \
  tests/core_tests/test_particle_dummy.cpp tests/core_tests/CMakeLists.txt
git commit -m "feat(particle): wire engine, CMake, SWIG and dummy safety"
```

---

### Task 6: Python API and pytest coverage

**Files:**
- Modify: `src/python/fife/fife.pyi` (add classes after `TimeEvent` at `:28`)
- Test: `tests/swig_tests/test_particle.py` (create)

**Interfaces:**
- Consumes: `fife.Engine.getParticleManager()`, `fife.ParticleManager.getInstance()`, `createEmitter(cfg)`, `fife.ParticleEmitter.setSeed/getEmittedCount/getAliveCount/attachToPoint`, `fife.ParticleRenderer.getInstance(cam)` (Task 5); `shared_engine` fixture (`tests/swig_tests/conftest.py:97-110`, single engine per module — do not use `engine` here).
- Produces: no new C++ API; Python smoke assertions that the C++/SWIG surface is usable.

- [ ] **Step 1: Write the failing test**

```python
# tests/swig_tests/test_particle.py
# SPDX-License-Identifier: LGPL-2.1-or-later
import time

from fife import fife


def test_particle_api_and_renderer_lookup(shared_engine):
    engine = shared_engine
    model = engine.getModel()
    map_obj = model.createMap("particle_py_map")
    layer = map_obj.createLayer("particle_py_layer", model.getCellGrid("square"))
    rb = engine.getRenderBackend()
    cam = map_obj.addCamera("particle_py_cam", fife.Rect(0, 0, rb.getWidth(), rb.getHeight()))
    cam.setLocation(fife.Location(layer))

    renderer = fife.ParticleRenderer.getInstance(cam)
    assert renderer is not None
    renderer.setEnabled(True)
    renderer.activateAllLayers(map_obj)

    manager = engine.getParticleManager()
    assert manager is not None
    assert fife.ParticleManager.getInstance() is manager

    cfg = fife.ParticleConfig()
    cfg.emissionRate = 120.0
    cfg.lifetimeMin = 10.0
    cfg.lifetimeMax = 10.0
    cfg.maxParticles = 1024
    cfg.localSpace = False
    emitter = manager.createEmitter(cfg)
    emitter.setSeed(1234)
    emitter.attachToPoint(fife.Point(10, 10))

    engine.initializePumping()
    engine.pump()
    time.sleep(0.05)
    engine.pump()
    assert emitter.getEmittedCount() >= 1
    assert emitter.getAliveCount() >= 1
    engine.finalizePumping()
```

- [ ] **Step 2: Run it to verify it fails**

Run: `python -m pytest tests/swig_tests/test_particle.py -v`
Expected: FAIL with `AttributeError: module 'fife.fife' has no attribute 'ParticleRenderer'` (or `ParticleManager`).

- [ ] **Step 3: Add typing stubs in `src/python/fife/fife.pyi`**

Add minimal `_Dynamic` subclasses `ParticleConfig`, `ParticleEmitter`, `ParticleManager`, `ParticleRenderer` and `class ParticleShape(_Dynamic): POINT: int; BOX: int; CIRCLE: int`, plus `Engine.getParticleManager` is inherited dynamically — no generated stub work.

- [ ] **Step 4: Run the test to verify it passes**

Run: `python -m pytest tests/swig_tests/test_particle.py -v`
Expected: 1 passed.

- [ ] **Step 5: Commit**

```bash
git add src/python/fife/fife.pyi tests/swig_tests/test_particle.py
git commit -m "test(particle): add Python API smoke coverage"
```

---

### Task 7: Runnable visual example

**Files:**
- Create: `tests/fife_test/tests/ParticleTest.py`
- Modify: `tests/fife_test/settings.xml:39`, `tests/fife_test/settings-dist.xml:39` (Tests module), `tests/fife_test/README.md`
- Test: `python -m py_compile tests/fife_test/tests/ParticleTest.py`

**Interfaces:**
- Consumes: `fife.ParticleManager`, `fife.ParticleEmitter`, `fife.ParticleConfig`, `fife.ParticleRenderer` (Task 6); `test.Test` base class and `TestManager` auto-discovery (`tests/fife_test/scripts/test.py:17-40` — any enabled `tests/*.py` exporting a same-named `Test` subclass is loaded).
- Produces: an interactive smoke/rain/sparks demo selectable in `run.py`; no pixel snapshots (per spec — output legitimately differs per backend).

- [ ] **Step 1: Verify the example file does not exist yet**

Run: `python -m py_compile tests/fife_test/tests/ParticleTest.py`
Expected: FAIL with `error: can't open file ... ParticleTest.py`.

- [ ] **Step 2: Implement the example in `tests/fife_test/tests/ParticleTest.py`**

Define `class ParticleTest(test.Test)` with `getName()` returning `"ParticleTest"`; in `instantiate()`/`reset()` (following `BenchmarkTest.py`) create a map/layer/camera, get `ParticleManager.getInstance()`, create three emitters from `ParticleConfig` presets (rain, smoke, sparks — use shapes, burstCount/burstInterval, gravity/drag, colour/size interpolation and a two-frame atlas made with `Image::useSharedImage`), enable `ParticleRenderer` and call `activateAllLayers(map)`. Use `engine.pump()` in `pump()` — never tick emitters from Python. Register the test with `<Setting name="ParticleTest" type="bool">True</Setting>` in both settings files and document one-line run instructions (`cd tests/fife_test && python run.py`) plus the documented budgets/caveats in the README: roughly 1k–3k particles on SDL, 5k+ on OpenGL, and no promise of identical appearance across backends.

- [ ] **Step 3: Run the smoke check**

Run: `python -m py_compile tests/fife_test/tests/ParticleTest.py`
Expected: exits 0 with no output; manual inspection via `run.py` confirms visible particles under the configured backend.

- [ ] **Step 4: Commit**

```bash
git add tests/fife_test/tests/ParticleTest.py tests/fife_test/settings.xml \
  tests/fife_test/settings-dist.xml tests/fife_test/README.md
git commit -m "feat(particle): add runnable visual example"
```
