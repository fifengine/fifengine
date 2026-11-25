// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// 3rd party library includes
#include <SDL3/SDL.h>

#include <fifechan.hpp>

#include <catch2/catch_test_macros.hpp>

// Standard C++ library includes
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

// FIFE includes
#include "controller/engine.h"
#include "eventchannel/eventmanager.h"
#include "eventchannel/mouse/imouselistener.h"
#include "eventchannel/mouse/mouseevent.h"
#include "fixture.h"
#include "gui/fifechan/base/dummy/dummy_gui_graphics.h"
#include "gui/fifechan/fifechanmanager.h"
#include "util/time/timemanager.h"
#include "video/dummy/dummyimage.h"
#include "video/dummy/renderbackenddummy.h"
#include "video/dummy/renderbackendnull.h"
#include "video/dummy/renderbackendsurface.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"

namespace
{
    std::string temporaryCapturePath(char const * filename)
    {
        return (std::filesystem::temp_directory_path() / filename).string();
    }

    std::vector<char> readBinaryFile(std::string const & filename)
    {
        std::ifstream stream(filename, std::ios::binary);
        REQUIRE(stream.good());
        return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
    }
} // namespace

TEST_CASE("Engine initializes with a dummy backend and no display", "[dummy]")
{
    FIFE::Engine engine; // Engine has no settings-taking constructor
    engine.getSettings().setRenderBackend("DUMMY_NULL");
    engine.getSettings().setScreenWidth(640);
    engine.getSettings().setScreenHeight(480);
    CHECK_NOTHROW(engine.init());
    // No exit() method exists; the Engine destructor performs teardown at scope end.
    REQUIRE(engine.getRenderBackend() != nullptr);
    CHECK(engine.getRenderBackend()->getName() == "DUMMY_NULL");
    CHECK(engine.getRenderBackend()->getWidth() == 640);
    CHECK(engine.getRenderBackend()->getHeight() == 480);
    CHECK(engine.getWindow() == nullptr);
}

TEST_CASE("Engine initializes with the surface backend and no display", "[dummy]")
{
    FIFE::Engine engine;
    engine.getSettings().setRenderBackend("DUMMY_SURFACE");
    engine.getSettings().setScreenWidth(320);
    engine.getSettings().setScreenHeight(240);
    CHECK_NOTHROW(engine.init());
    REQUIRE(engine.getRenderBackend() != nullptr);
    CHECK(engine.getRenderBackend()->getName() == "DUMMY_SURFACE");
    CHECK(engine.getRenderBackend()->getWidth() == 320);
    CHECK(engine.getRenderBackend()->getHeight() == 240);
    CHECK(engine.getWindow() == nullptr);
}

TEST_CASE("dummy base reports configured geometry without a window or screen", "[dummy]")
{
    FIFE::RenderBackendDummy backend({0, 0, 0, 255}, 640, 480);
    CHECK(backend.getWidth() == 640);
    CHECK(backend.getHeight() == 480);
    CHECK(backend.getArea().w == 640);
    CHECK(backend.getArea().h == 480);
}

TEST_CASE("dummy backend can be destroyed and recreated without assert", "[dummy]")
{
    {
        FIFE::RenderBackendDummy first({0, 0, 0, 255}, 320, 240);
        CHECK(first.getWidth() == 320);
    }
    FIFE::RenderBackendDummy second({0, 0, 0, 255}, 800, 600);
    CHECK(second.getWidth() == 800);
}

TEST_CASE("Null backend creates images and never needs a surface", "[dummy]")
{
    FIFE::RenderBackendNull backend({0, 0, 0, 255}, 320, 240);
    backend.init("");
    backend.createMainScreen("headless", "");

    std::vector<uint8_t> rgba(4 * 4 * 4, 255);
    auto img = backend.createImage(rgba.data(), 4, 4);
    REQUIRE(img);
    CHECK(img->getWidth() == 4);
    CHECK(img->getHeight() == 4);
    CHECK_NOTHROW(img->render(FIFE::Rect(0, 0, 4, 4)));
    CHECK_NOTHROW(backend.captureScreen(temporaryCapturePath("fife-null-capture.png")));
}

TEST_CASE("Surface backend produces deterministic pixels", "[dummy]")
{
    FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 64, 64);
    backend.init("");
    backend.createMainScreen("headless", "");

    backend.fillRectangle(FIFE::Point(4, 4), 16, 16, 255, 0, 0, 255);
    backend.endFrame();

    std::vector<uint8_t> first(64 * 64 * 4, 0);
    auto* surf = backend.getScreenSurface();
    REQUIRE(surf != nullptr);
    std::copy(static_cast<uint8_t*>(surf->pixels), static_cast<uint8_t*>(surf->pixels) + first.size(), first.begin());

    backend.fillRectangle(FIFE::Point(4, 4), 16, 16, 255, 0, 0, 255);
    backend.endFrame();
    std::vector<uint8_t> second(64 * 64 * 4, 0);
    std::copy(static_cast<uint8_t*>(surf->pixels), static_cast<uint8_t*>(surf->pixels) + second.size(), second.begin());

    CHECK(first == second);
    CHECK_NOTHROW(backend.captureScreen(temporaryCapturePath("fife-surface-capture.png")));
}

TEST_CASE("Surface backend attachRenderTarget preserves pixels of standard images", "[dummy]")
{
    FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 32, 32);
    backend.init("");
    backend.createMainScreen("headless", "");

    // Images built from RGBA bytes use SDL_PIXELFORMAT_RGBA32, not RGBA8888.
    // With discard=false their existing pixels must survive attach/detach.
    constexpr uint32_t size = 16;
    std::vector<uint8_t> rgba(size * size * 4);
    for (uint32_t i = 0; i < size * size; ++i) {
        rgba[i * 4 + 0] = 17;
        rgba[i * 4 + 1] = 61;
        rgba[i * 4 + 2] = 103;
        rgba[i * 4 + 3] = 255;
    }
    FIFE::ImagePtr img(backend.createImage(rgba.data(), size, size).release());
    REQUIRE(img);

    backend.attachRenderTarget(img, false);
    backend.detachRenderTarget();

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
    img->getPixelRGBA(7, 9, &r, &g, &b, &a);
    CHECK(r == 17);
    CHECK(g == 61);
    CHECK(b == 103);
    CHECK(a == 255);
}

TEST_CASE("Surface backend renders dummy images through the backend virtual", "[dummy]")
{
    FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 32, 32);
    backend.init("");
    backend.createMainScreen("headless", "");
    CHECK(backend.getName() == "DUMMY_SURFACE");

    std::vector<uint8_t> rgba(8 * 8 * 4, 0);
    for (std::size_t i = 0; i < 8 * 8; ++i) {
        rgba[i * 4 + 0] = 0;
        rgba[i * 4 + 1] = 200;
        rgba[i * 4 + 2] = 0;
        rgba[i * 4 + 3] = 255;
    }
    auto img = backend.createImage(rgba.data(), 8, 8);
    REQUIRE(img);
    CHECK_NOTHROW(img->render(FIFE::Rect(2, 2, 8, 8)));
    backend.endFrame();

    auto* surf = backend.getScreenSurface();
    REQUIRE(surf != nullptr);
    SDL_Surface* copy = SDL_DuplicateSurface(surf);
    REQUIRE(copy != nullptr);
    FIFE::DummyImage probe(copy);
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
    probe.getPixelRGBA(5, 5, &r, &g, &b, &a);
    CHECK(r == 0);
    CHECK(g == 200);
    CHECK(b == 0);
    CHECK(a == 255);
}

TEST_CASE("Surface backend drawBezier works with three or more control points", "[dummy]")
{
    FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 64, 64);
    backend.init("");
    backend.createMainScreen("headless", "");

    std::vector<FIFE::Point> const control = {
        FIFE::Point(4, 4), FIFE::Point(20, 4), FIFE::Point(20, 20), FIFE::Point(36, 20)};
    CHECK_NOTHROW(backend.drawBezier(control, 8, 1, 255, 0, 0, 255));
    backend.endFrame();

    auto* surf = backend.getScreenSurface();
    REQUIRE(surf != nullptr);
    SDL_Surface* copy = SDL_DuplicateSurface(surf);
    REQUIRE(copy != nullptr);
    FIFE::DummyImage probe(copy);
    int written = 0;
    for (int32_t y = 0; y < surf->h; ++y) {
        for (int32_t x = 0; x < surf->w; ++x) {
            uint8_t r = 0;
            uint8_t g = 0;
            uint8_t b = 0;
            uint8_t a = 0;
            probe.getPixelRGBA(x, y, &r, &g, &b, &a);
            if (r > 0) {
                ++written;
            }
        }
    }
    CHECK(written > 0);
}

TEST_CASE("Surface backend applies alpha when blitting a dummy image", "[dummy]")
{
    FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 32, 32);
    backend.init("");
    backend.createMainScreen("headless", "");

    auto* surf = backend.getScreenSurface();
    REQUIRE(surf != nullptr);
    // A transparent target makes the blit's effective alpha observable.
    SDL_FillSurfaceRect(surf, nullptr, SDL_MapSurfaceRGBA(surf, 0, 0, 0, 0));

    std::vector<uint8_t> rgba(8 * 8 * 4, 0);
    for (std::size_t i = 0; i < 8 * 8; ++i) {
        rgba[i * 4 + 0] = 0;
        rgba[i * 4 + 1] = 200;
        rgba[i * 4 + 2] = 0;
        rgba[i * 4 + 3] = 255;
    }
    auto img = backend.createImage(rgba.data(), 8, 8);
    REQUIRE(img);
    CHECK_NOTHROW(img->render(FIFE::Rect(2, 2, 8, 8), 128));
    backend.endFrame();

    SDL_Surface* copy = SDL_DuplicateSurface(surf);
    REQUIRE(copy != nullptr);
    FIFE::DummyImage probe(copy);
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
    probe.getPixelRGBA(5, 5, &r, &g, &b, &a);
    CHECK(a > 110);
    CHECK(a < 146);
}

TEST_CASE("DummyGuiGraphics draws widgets through the Surface backend", "[dummy]")
{
    FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 200, 100);
    backend.init("");
    backend.createMainScreen("headless", "");

    FIFE::DummyGuiGraphics graphics;
    graphics.updateTarget();

    fcn::Gui gui;
    gui.setGraphics(&graphics);
    auto top = std::make_unique<fcn::Container>();
    top->setDimension(fcn::Rectangle(0, 0, 200, 100));
    auto label = std::make_unique<fcn::Label>("headless");
    top->add(label.get(), 10, 10);
    gui.setTop(top.get());

    CHECK_NOTHROW(gui.draw());
    CHECK_NOTHROW(backend.captureScreen(temporaryCapturePath("fife-dummy-gui.png")));
}

TEST_CASE("Surface backend PNG snapshot matches the checked-in golden image", "[dummy]")
{
    TestFixture const init;
    FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 64, 48);
    backend.init("");
    backend.createMainScreen("headless", "");

    // A small, fully deterministic primitive set. Everything is opaque so
    // PNG loading cannot alter pixels through transparency sanitization.
    backend.fillRectangle(FIFE::Point(2, 2), 16, 12, 220, 30, 40, 255);
    backend.fillRectangle(FIFE::Point(24, 6), 12, 10, 20, 190, 70, 255);
    backend.fillRectangle(FIFE::Point(42, 3), 8, 16, 30, 60, 210, 255);
    backend.drawLine(FIFE::Point(2, 24), FIFE::Point(50, 24), 240, 240, 0, 255);
    backend.drawLine(FIFE::Point(8, 30), FIFE::Point(8, 44), 200, 0, 200, 255);
    backend.putPixel(60, 40, 10, 120, 90, 255);
    backend.endFrame();

    std::string const capture  = temporaryCapturePath("fife-dummy-snapshot-capture.png");
    std::string const repeated = temporaryCapturePath("fife-dummy-snapshot-repeat.png");
    std::string const golden   = "tests/data/dummy_surface_golden.png";

    backend.captureScreen(capture);
    backend.captureScreen(repeated);

    // Encoder output must be byte-identical for identical input within a run.
    std::vector<char> const firstPng  = readBinaryFile(capture);
    std::vector<char> const secondPng = readBinaryFile(repeated);
    REQUIRE_FALSE(firstPng.empty());
    CHECK(firstPng == secondPng);

    // Compare decoded pixels with the committed snapshot; raw PNG bytes may
    // differ across platforms and SDL_image versions.
    FIFE::ImagePtr const actual   = FIFE::ImageManager::instance()->load(capture);
    FIFE::ImagePtr const expected = FIFE::ImageManager::instance()->load(golden);
    REQUIRE(actual);
    REQUIRE(expected);
    REQUIRE(actual->getWidth() == expected->getWidth());
    REQUIRE(actual->getHeight() == expected->getHeight());

    for (int32_t y = 0; y < static_cast<int32_t>(actual->getHeight()); ++y) {
        for (int32_t x = 0; x < static_cast<int32_t>(actual->getWidth()); ++x) {
            uint8_t ar = 0;
            uint8_t ag = 0;
            uint8_t ab = 0;
            uint8_t aa = 0;
            uint8_t er = 0;
            uint8_t eg = 0;
            uint8_t eb = 0;
            uint8_t ea = 0;
            actual->getPixelRGBA(x, y, &ar, &ag, &ab, &aa);
            expected->getPixelRGBA(x, y, &er, &eg, &eb, &ea);
            INFO("pixel (" << x << ", " << y << ")");
            CHECK(ar == er);
            CHECK(ag == eg);
            CHECK(ab == eb);
            CHECK(aa == ea);
        }
    }
}

TEST_CASE("FifechanManager accepts the dummy backend names", "[dummy]")
{
    for (auto const * backendName : {"DUMMY_NULL", "DUMMY_SURFACE"}) {
        FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 160, 90);
        backend.init("");
        backend.createMainScreen("headless", "");

        auto manager = std::make_unique<FIFE::FifechanManager>();
        manager->setConsoleEnabled(false);
        CHECK_NOTHROW(manager->init(backendName, 160, 90));
        CHECK_NOTHROW(manager->turn());
    }
}

namespace
{
    /** Records presses on a Fifechan widget. Fifechan has no lambda listener
     *  registration, so tests use a concrete MouseListener subclass. */
    class ClickRecorder final : public fcn::MouseListener
    {
        public:
            bool pressed = false;
            void mousePressed(fcn::MouseEvent&) override
            {
                pressed = true;
            }
    };

    /** Records the FIFE-level MouseEvent produced by EventManager. */
    class FifeMouseRecorder final : public FIFE::IMouseListener
    {
        public:
            int32_t x = -1;
            int32_t y = -1;
            void mousePressed(FIFE::MouseEvent& evt) override
            {
                x = evt.getX();
                y = evt.getY();
            }
    };

    void pushMouseDown(float x, float y)
    {
        SDL_Event evt{};
        evt.type          = SDL_EVENT_MOUSE_BUTTON_DOWN;
        evt.button.button = SDL_BUTTON_LEFT;
        evt.button.down   = true;
        evt.button.x      = x;
        evt.button.y      = y;
        REQUIRE(SDL_PushEvent(&evt));
    }
} // namespace

TEST_CASE("synthetic mouse events reach widgets with no window", "[dummy]")
{
    // MouseEvent timestamps itself through TimeManager; direct-backend tests
    // bypass Engine::init(), so keep a TimeManager alive for the duration.
    std::shared_ptr<FIFE::TimeManager> timeManager;
    if (!FIFE::TimeManager::exists()) {
        timeManager = std::make_shared<FIFE::TimeManager>();
    }

    // Direct-backend tests also bypass the headless Engine lifecycle that
    // normally initializes the SDL event subsystem (Task 5).
    REQUIRE(SDL_InitSubSystem(SDL_INIT_EVENTS));

    FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 200, 100);
    backend.init("");
    backend.createMainScreen("headless", "");

    // FifechanManager owns the real fcn::Gui and its SDL input bridge; it is
    // the project's widget pipeline and is also an ISdlEventListener.
    auto manager = std::make_unique<FIFE::FifechanManager>();
    manager->setConsoleEnabled(false);
    manager->init("DUMMY_SURFACE", 200, 100);

    auto button = std::make_unique<fcn::Button>("click");
    button->setDimension(fcn::Rectangle(20, 20, 40, 20));
    manager->add(button.get());
    ClickRecorder recorder;
    button->addMouseListener(&recorder);

    FifeMouseRecorder fifeRecorder;
    // Global so FIFE listeners still see events consumed by widgets.
    fifeRecorder.setGlobalListener(true);
    FIFE::EventManager events;
    events.addMouseListener(&fifeRecorder);
    events.addSdlEventListener(manager.get());

    SECTION("an in-bounds click reaches the widget")
    {
        pushMouseDown(30.0F, 30.0F);
        events.processEvents();
        manager->getFifechanGUI()->logic();

        CHECK(recorder.pressed);
        CHECK(fifeRecorder.x == 30);
        CHECK(fifeRecorder.y == 30);
    }

    SECTION("coordinates normalize to the dummy backend size without a window")
    {
        // No Window is attached, so fillMouseEvent must use the backend's
        // configured dimensions instead of a window point size.
        pushMouseDown(250.0F, 150.0F);
        events.processEvents();

        CHECK(fifeRecorder.x == 199);
        CHECK(fifeRecorder.y == 99);
    }
}
