// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <fifechan.hpp>

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>

// Platform specific includes
#include "fixture.h"
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "gui/fifechan/base/gui_image.h"
#include "gui/fifechan/base/gui_imageloader.h"
#include "gui/fifechan/base/opengl/opengl_gui_graphics.h"
#include "gui/fifechan/base/sdl/sdl_gui_graphics.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/sdl/renderbackendsdl.h"

using FIFE::GuiImageLoader;
using FIFE::ImageManager;
using FIFE::ImagePtr;
using FIFE::OpenGLGuiGraphics;
using FIFE::Rect;
using FIFE::RenderBackend;
using FIFE::RenderBackendOpenGL;
using FIFE::RenderBackendSDL;
using FIFE::ScreenMode;
using FIFE::SdlGuiGraphics;

static char const * const IMAGE_FILE = "tests/data/beach_e1.png";
struct environment : TestFixture
{
};

void test_gui_image(RenderBackend& renderbackend, fcn::Graphics& graphics)
{
    GuiImageLoader imageloader;
    fcn::Image::setImageLoader(&imageloader);

    fcn::Container* top = new fcn::Container();
    top->setDimension(fcn::Rectangle(0, 0, 200, 300));
    fcn::Gui* gui = new fcn::Gui();
    gui->setGraphics(&graphics);
    gui->setTop(top);
    fcn::Label* label = new fcn::Label("Label");

    fcn::Image const * guiimage = fcn::Image::load(IMAGE_FILE);
    fcn::Icon* icon             = new fcn::Icon(guiimage);

    top->add(label, 10, 10);
    top->add(icon, 10, 30);

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
    RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_SDL), "FIFE", "");
    SdlGuiGraphics graphics;
    graphics.updateTarget();
    test_gui_image(renderbackend, graphics);
}

TEST_CASE("GuiImageLoader renders beach_e1.png via OpenGL", "[gui][opengl]")
{
    environment const env;
    RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_OPENGL), "FIFE", "");
    OpenGLGuiGraphics graphics;
    graphics.updateTarget();
    test_gui_image(renderbackend, graphics);
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
    RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_SDL), "FIFE", "");
    test_create_image_converts_format(renderbackend, SDL_PIXELFORMAT_RGBA8888);
}

TEST_CASE("createImage converts BGRA8888 to RGBA32 in OpenGL backend", "[gui][opengl]")
{
    environment const env;
    RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_OPENGL), "FIFE", "");
    test_create_image_converts_format(renderbackend, SDL_PIXELFORMAT_RGBA32);
}
