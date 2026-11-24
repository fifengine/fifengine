// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// 3rd party library includes
#include <fifechan.hpp>

#include <catch2/catch_test_macros.hpp>

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>

// Platform specific includes
#include "fixture.h"

// FIFE includes
#include "gui/fifechan/base/gui_image.h"
#include "gui/fifechan/base/gui_imageloader.h"
#include "gui/fifechan/base/opengl/opengl_gui_graphics.h"
#include "gui/fifechan/base/sdl/sdl_gui_graphics.h"
#include "gui/fifechan/fifechanmanager.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/window/window.h"

using FIFE::FifechanManager;
using FIFE::GuiImageLoader;
using FIFE::ImageManager;
using FIFE::ImagePtr;
using FIFE::OpenGLGuiGraphics;
using FIFE::Rect;
using FIFE::RenderBackend;
using FIFE::RenderBackendOpenGL;
using FIFE::RenderBackendSDL;
using FIFE::SdlGuiGraphics;
using FIFE::Window;
using FIFE::WindowMode;
using FIFE::WindowSettings;

static char const * const IMAGE_FILE = "tests/data/beach_e1.png";
struct environment : TestFixture
{
};

void test_gui_image(RenderBackend& renderbackend, fcn::Graphics& graphics)
{
    GuiImageLoader imageloader;
    fcn::Image::setImageLoader(&imageloader);

    auto top = std::make_unique<fcn::Container>();
    top->setDimension(fcn::Rectangle(0, 0, 200, 300));
    auto gui = std::make_unique<fcn::Gui>();
    gui->setGraphics(&graphics);
    gui->setTop(top.get());
    auto label = std::make_unique<fcn::Label>("Label");

    fcn::Image const * guiimage = fcn::Image::load(IMAGE_FILE);
    auto icon                   = std::make_unique<fcn::Icon>(guiimage);

    top->add(label.get(), 10, 10);
    top->add(icon.get(), 10, 30);

    ImagePtr img = ImageManager::instance()->load(IMAGE_FILE);
    REQUIRE(img);

    int const h = static_cast<int>(img->getHeight());
    int const w = static_cast<int>(img->getWidth());
    for (int i = 0; i < 100; i += 2) {
        renderbackend.startFrame();
        img->render(Rect(i, i, w, h));
        gui->logic();
        gui->draw();
        renderbackend.endFrame();
    }
}

TEST_CASE("GuiImageLoader renders beach_e1.png via SDL", "[gui][sdl]")
{
    environment const env;
    Window window;
    window.create(WindowSettings{.width = 800, .height = 600, .opengl = false, .windowMode = WindowMode::Windowed});
    RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");
    SdlGuiGraphics graphics;
    graphics.updateTarget();
    test_gui_image(renderbackend, graphics);
}

TEST_CASE("GuiImageLoader renders beach_e1.png via OpenGL", "[gui][opengl]")
{
    environment const env;
    Window window;
    try {
        window.create(WindowSettings{.width = 800, .height = 600, .opengl = true, .windowMode = WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");
    OpenGLGuiGraphics graphics;
    graphics.updateTarget();
    test_gui_image(renderbackend, graphics);
}

// ---------------------------------------------------------------------------
// An application may call Window::create() directly. SDL destroys the old
// window (and its renderer/GL context); the backend must reattach instead of
// keeping pointers to the destroyed window.
// ---------------------------------------------------------------------------

TEST_CASE("RenderBackendSDL_ReattachesAfterWindowRecreation", "[renderbackend][sdl]")
{
    environment const env;
    Window window;
    window.create(WindowSettings{.width = 800, .height = 600, .opengl = false, .windowMode = WindowMode::Windowed});

    RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");
    REQUIRE(renderbackend.isWindowAttached());
    REQUIRE(renderbackend.getRenderer() != nullptr);

    window.create(WindowSettings{.width = 640, .height = 480, .opengl = false, .windowMode = WindowMode::Windowed});
    renderbackend.createMainScreen("FIFE", "");

    CHECK(renderbackend.isWindowAttached());
    REQUIRE(renderbackend.getRenderer() != nullptr);
    CHECK(renderbackend.getRenderer() == SDL_GetRenderer(window.getSDLWindow()));
    renderbackend.clearBackBuffer();
}

TEST_CASE("RenderBackendOpenGL_ReattachesAfterWindowRecreation", "[renderbackend][opengl]")
{
    environment const env;
    Window window;
    try {
        window.create(WindowSettings{.width = 800, .height = 600, .opengl = true, .windowMode = WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }

    RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    try {
        renderbackend.init("");
        renderbackend.setWindowObject(&window);
        renderbackend.createMainScreen("FIFE", "");
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL context not available in this environment");
    }
    REQUIRE(renderbackend.isWindowAttached());

    try {
        window.create(WindowSettings{.width = 640, .height = 480, .opengl = true, .windowMode = WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL window recreation not available in this environment");
    }

    CHECK_NOTHROW(renderbackend.createMainScreen("FIFE", ""));
    CHECK(renderbackend.isWindowAttached());
}

// Regression test: createImage must convert surfaces to the backend's internal
// format (RGBA8888 for SDL, RGBA32 for OpenGL). SDL_ttf returns BGRA8888 surfaces;
// if no conversion happens, the byte order mismatch corrupts the alpha channel on
// texture upload.
namespace
{
    void test_create_image_converts_format(RenderBackend& backend, SDL_PixelFormat expected_fmt)
    {
        // Create a BGRA8888 surface mimicking what SDL_ttf produces
        SDL_Surface* src = SDL_CreateSurface(32, 32, SDL_PIXELFORMAT_BGRA8888);
        REQUIRE(src != nullptr);
        REQUIRE(src->format == SDL_PIXELFORMAT_BGRA8888);

        // Fill with a known pattern: pixel (R=68, G=42, B=2) = UH dark brown
        SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(src->format);
        REQUIRE(SDL_LockSurface(src));
        auto* pixels = static_cast<uint32_t*>(src->pixels);

        for (int y = 0; y < src->h; ++y) {
            for (int x = 0; x < src->w; ++x) {
                size_t const i =
                    (static_cast<size_t>(y) * static_cast<size_t>(src->pitch / 4)) + static_cast<size_t>(x);
                // SDL surfaces expose a raw pixel buffer indexed via pitch; bounds come from the surface.
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                pixels[i] = SDL_MapRGBA(fmt, nullptr, 68, 42, 2, 255);
            }
        }
        SDL_UnlockSurface(src);

        // Pass through createImage (takes ownership of src)
        ImagePtr img(backend.createImage(src).release());

        // The resulting surface MUST be in the backend's upload format
        SDL_Surface* out = img->getSurface();
        CHECK(out->format == expected_fmt);

        // Verify pixel values survive conversion using SDL_GetRGBA (format-aware).
        // BGRA8888 pixel (R=68,G=42,B=2,A=255) must still read back correctly
        // after conversion.
        REQUIRE(SDL_LockSurface(out));
        SDL_PixelFormatDetails const * out_fmt = SDL_GetPixelFormatDetails(out->format);
        auto* out_pixels                       = static_cast<uint32_t*>(out->pixels);
        uint8_t r                              = 0;
        uint8_t g                              = 0;
        uint8_t b                              = 0;
        uint8_t a                              = 0;
        // The SDL surface pixel buffer is raw memory; element zero is the first converted pixel.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        SDL_GetRGBA(out_pixels[0], out_fmt, nullptr, &r, &g, &b, &a);
        CHECK(r == 68);
        CHECK(g == 42);
        CHECK(b == 2);
        CHECK(a == 255);
        SDL_UnlockSurface(out);
    }
} // namespace

TEST_CASE("createImage converts BGRA8888 to RGBA8888 in SDL backend", "[gui][sdl]")
{
    environment const env;
    Window window;
    window.create(WindowSettings{.width = 800, .height = 600, .opengl = false, .windowMode = WindowMode::Windowed});
    RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");
    test_create_image_converts_format(renderbackend, SDL_PIXELFORMAT_RGBA8888);
}

TEST_CASE("createImage converts BGRA8888 to RGBA32 in OpenGL backend", "[gui][opengl]")
{
    environment const env;
    Window window;
    try {
        window.create(WindowSettings{.width = 800, .height = 600, .opengl = true, .windowMode = WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");
    test_create_image_converts_format(renderbackend, SDL_PIXELFORMAT_RGBA32);
}

TEST_CASE("FifechanManager ignores raw finger events", "[gui][sdl][fifechan]")
{
    environment const env;
    Window window;
    window.create(WindowSettings{.width = 800, .height = 600, .opengl = false, .windowMode = WindowMode::Windowed});

    RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");

    auto manager = std::make_unique<FifechanManager>();
    manager->init("SDL", 800, 600);

    auto label = std::make_unique<fcn::Label>("target");
    label->setDimension(fcn::Rectangle(100, 100, 60, 30));
    manager->add(label.get());

    SDL_WindowID const windowId = SDL_GetWindowID(window.getSDLWindow());

    // Coordinate that maps to the widget under SDL's normalized touch space.
    SDL_Event finger{};
    finger.type             = SDL_EVENT_FINGER_DOWN;
    finger.tfinger.touchID  = 1;
    finger.tfinger.fingerID = 0;
    finger.tfinger.x        = 120.0F / 800.0F;
    finger.tfinger.y        = 110.0F / 600.0F;
    finger.tfinger.windowID = windowId;

    // FIFE translates touch in EventManager and never forwards raw fingers.
    // Feeding them here as well would process every gesture twice, so the
    // event must not be consumed even when it is over a widget.
    CHECK_FALSE(manager->onSdlEvent(finger));

    finger.type = SDL_EVENT_FINGER_UP;
    CHECK_FALSE(manager->onSdlEvent(finger));
}

TEST_CASE("Fifechan SDL3 backend provides native touch input", "[gui][fifechan]")
{
    STATIC_REQUIRE(fcn::sdl3::Input::hasNativeTouchInput());
}

TEST_CASE("FifechanManager hit-tests widgets at the event position", "[gui][sdl][fifechan]")
{
    environment const env;
    Window window;
    window.create(WindowSettings{.width = 800, .height = 600, .opengl = false, .windowMode = WindowMode::Windowed});

    RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");

    auto manager = std::make_unique<FifechanManager>();
    manager->init("SDL", 800, 600);

    auto label = std::make_unique<fcn::Label>("target");
    label->setDimension(fcn::Rectangle(100, 100, 60, 30));
    manager->add(label.get());

    SDL_WindowID const windowId = SDL_GetWindowID(window.getSDLWindow());

    SDL_Event button{};
    button.type            = SDL_EVENT_MOUSE_BUTTON_DOWN;
    button.button.button   = SDL_BUTTON_LEFT;
    button.button.down     = true;
    button.button.x        = 120.0F;
    button.button.y        = 110.0F;
    button.button.windowID = windowId;

    // No motion event precedes this button press. Hit-testing must use the
    // button event's own coordinates instead of the stale (0, 0) position.
    CHECK(manager->onSdlEvent(button));

    SDL_Event outside = button;
    outside.button.x  = 10.0F;
    outside.button.y  = 10.0F;
    CHECK_FALSE(manager->onSdlEvent(outside));

    SDL_Event wheel{};
    wheel.type            = SDL_EVENT_MOUSE_WHEEL;
    wheel.wheel.x         = 1.0F;
    wheel.wheel.y         = 0.0F;
    wheel.wheel.mouse_x   = 120.0F;
    wheel.wheel.mouse_y   = 110.0F;
    wheel.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
    wheel.wheel.windowID  = windowId;
    CHECK(manager->onSdlEvent(wheel));

    wheel.wheel.mouse_x = 10.0F;
    wheel.wheel.mouse_y = 10.0F;
    CHECK_FALSE(manager->onSdlEvent(wheel));
}
