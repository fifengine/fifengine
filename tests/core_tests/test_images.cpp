// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <cstring>
#include <memory>
#include <span>
#include <string>
#include <vector>

// Platform specific includes
#include "fixture.h"

// 3rd party library includes
#include <SDL3/SDL.h>

#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/renderbackend.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/window/window.h"

static char const * const IMAGE_FILE       = "docs/logo/FIFE_small_c3.png";
static char const * const ALPHA_IMAGE_FILE = "tests/data/alpha_fidgit.png";
static char const * const SUBIMAGE_FILE    = "tests/data/rpg_tiles_01.png";

TEST_CASE("RenderBackendSDL renders FIFE_small_c3.png with position shift", "[core][images]")
{
    TestFixture const _init;
    FIFE::Window window;
    window.create(
        FIFE::WindowSettings{.width = 800, .height = 600, .opengl = false, .windowMode = FIFE::WindowMode::Windowed});
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");

    {
        SDL_Rect const rect = {.x = 0, .y = 0, .w = 1, .h = 1};
        SDL_Surface* surf   = SDL_RenderReadPixels(renderbackend.getRenderer(), &rect);
        REQUIRE(surf != nullptr);
        SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(surf->format);
        uint8_t r                          = 0;
        uint8_t g                          = 0;
        uint8_t b                          = 0;
        uint8_t a                          = 0;
        SDL_GetRGBA(*static_cast<uint32_t const *>(surf->pixels), fmt, nullptr, &r, &g, &b, &a);
        CHECK((r) == (0));
        CHECK((g) == (0));
        CHECK((b) == (0));
        CHECK((a) == (255));
        SDL_DestroySurface(surf);
    }

    FIFE::ImagePtr img = FIFE::ImageManager::instance()->load(IMAGE_FILE);
    REQUIRE(img);

    // Read pixel (134,172) from the loaded surface for ground truth
    uint8_t surf_r = 0;
    uint8_t surf_g = 0;
    uint8_t surf_b = 0;
    uint8_t surf_a = 0;
    {
        SDL_Surface const * s = img->getSurface();
        REQUIRE(s->w > 134);
        REQUIRE(s->h > 172);
        SDL_PixelFormatDetails const * f = SDL_GetPixelFormatDetails(s->format);
        uint32_t pix                     = 0;
        auto pixel_span                  = std::span<uint8_t const>(
            static_cast<uint8_t const *>(s->pixels), static_cast<size_t>(s->h) * static_cast<size_t>(s->pitch));
        size_t const offset =
            (static_cast<size_t>(172) * static_cast<size_t>(s->pitch)) + (static_cast<size_t>(134) * 4U);
        std::memcpy(&pix, pixel_span.data() + offset, sizeof(pix));
        SDL_GetRGBA(pix, f, nullptr, &surf_r, &surf_g, &surf_b, &surf_a);
    }

    int const h = static_cast<int>(img->getHeight());
    int const w = static_cast<int>(img->getWidth());

    // Render first frame at (0,0) and verify a known pixel from the image
    renderbackend.startFrame();
    img->render(FIFE::Rect(0, 0, w, h));

    {
        SDL_Rect const rect = {.x = 134, .y = 172, .w = 1, .h = 1};
        SDL_Surface* surf   = SDL_RenderReadPixels(renderbackend.getRenderer(), &rect);
        REQUIRE(surf != nullptr);
        SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(surf->format);
        uint8_t r                          = 0;
        uint8_t g                          = 0;
        uint8_t b                          = 0;
        uint8_t a                          = 0;
        SDL_GetRGBA(*static_cast<uint32_t const *>(surf->pixels), fmt, nullptr, &r, &g, &b, &a);
        CHECK((r) == (surf_r));
        CHECK((g) == (surf_g));
        CHECK((b) == (surf_b));
        CHECK((a) == (surf_a));
        SDL_DestroySurface(surf);
    }

    renderbackend.endFrame();

    for (int i = 1; i < 100; i++) {
        renderbackend.startFrame();
        img->render(FIFE::Rect(i, i, w, h));
        renderbackend.endFrame();
    }
    for (int j = 0; j < 5; j++) {
        for (int i = -10; i < 10; i++) {
            renderbackend.startFrame();
            img->setXShift(i);
            img->setYShift(i);
            img->render(FIFE::Rect(200, 200, w, h));
            renderbackend.endFrame();
        }
    }
}

TEST_CASE("RenderBackendOpenGL renders fife_logo.png", "[core][images]")
{
    TestFixture const _init;
    FIFE::Window window;
    try {
        window.create(
            FIFE::WindowSettings{
                .width = 800, .height = 600, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    FIFE::RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    try {
        renderbackend.createMainScreen("FIFE", "");
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }

    FIFE::ImagePtr img = FIFE::ImageManager::instance()->load(IMAGE_FILE);
    REQUIRE(img);

    int const h = static_cast<int>(img->getHeight());
    int const w = static_cast<int>(img->getWidth());
    for (int i = 0; i < 100; i++) {
        renderbackend.startFrame();
        img->render(FIFE::Rect(i, i, w, h));
        renderbackend.endFrame();
    }
}

TEST_CASE("RenderBackendSDL renders subimages from rpg_tiles_01.png", "[core][images]")
{
    TestFixture const _init;
    FIFE::Window window;
    window.create(
        FIFE::WindowSettings{.width = 800, .height = 600, .opengl = false, .windowMode = FIFE::WindowMode::Windowed});
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");

    FIFE::ImagePtr img = FIFE::ImageManager::instance()->load(SUBIMAGE_FILE);
    REQUIRE(img);

    int const W = static_cast<int>(img->getWidth());
    int const w = W / 12;
    int const H = static_cast<int>(img->getHeight());
    int const h = H / 12;
    std::vector<FIFE::ImagePtr> subimages;

    for (int x = 0; x < (W - w); x += w) {
        for (int y = 0; y < (H - h); y += h) {
            FIFE::ImagePtr const sub = FIFE::ImageManager::instance()->create();
            sub->useSharedImage(img, FIFE::Rect(x, y, w, h));
            subimages.push_back(sub);
        }
    }

    for (unsigned int i = 0; i < 200; i++) {
        renderbackend.startFrame();
        subimages.at(i / 40).get()->render(FIFE::Rect(200, 200, w, h));
        renderbackend.endFrame();
    }
}

TEST_CASE("RenderBackendOpenGL renders subimages from rpg_tiles_01.png", "[core][images]")
{
    TestFixture const _init;
    FIFE::Window window;
    try {
        window.create(
            FIFE::WindowSettings{
                .width = 800, .height = 600, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    FIFE::RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    try {
        renderbackend.createMainScreen("FIFE", "");
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }

    FIFE::ImagePtr img = FIFE::ImageManager::instance()->load(SUBIMAGE_FILE);
    REQUIRE(img);

    int const W = static_cast<int>(img->getWidth());
    int const w = W / 12;
    int const H = static_cast<int>(img->getHeight());
    int const h = H / 12;
    std::vector<FIFE::ImagePtr> subimages;

    for (int x = 0; x < (W - w); x += w) {
        for (int y = 0; y < (H - h); y += h) {
            FIFE::ImagePtr const sub = FIFE::ImageManager::instance()->create();
            sub->useSharedImage(img, FIFE::Rect(x, y, w, h));
            subimages.push_back(sub);
        }
    }

    for (unsigned int i = 0; i < 200; i++) {
        renderbackend.startFrame();
        subimages.at(i / 40).get()->render(FIFE::Rect(200, 200, w, h));
        renderbackend.endFrame();
    }
}

TEST_CASE("RenderBackendSDL alpha optimizer renders fife_logo.png and alpha_fidgit.png", "[core][images]")
{
    TestFixture const _init;
    FIFE::Window window;
    window.create(
        FIFE::WindowSettings{.width = 800, .height = 600, .opengl = false, .windowMode = FIFE::WindowMode::Windowed});
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");
    renderbackend.setAlphaOptimizerEnabled(true);

    FIFE::ImagePtr img       = FIFE::ImageManager::instance()->load(IMAGE_FILE);
    FIFE::ImagePtr alpha_img = FIFE::ImageManager::instance()->load(ALPHA_IMAGE_FILE);
    REQUIRE(img);
    REQUIRE(alpha_img);

    int const h0 = static_cast<int>(img->getHeight());
    int const w0 = static_cast<int>(img->getWidth());

    int const h1 = static_cast<int>(alpha_img->getHeight());
    int const w1 = static_cast<int>(alpha_img->getWidth());
    for (int i = 0; i != 200; ++i) {
        renderbackend.startFrame();
        img.get()->render(FIFE::Rect(i, i, w0, h0));
        alpha_img.get()->render(FIFE::Rect(i, i, w1, h1));
        alpha_img.get()->render(FIFE::Rect(i, h0 + i, w1, h1));
        img.get()->render(FIFE::Rect(i, h0 + i, w0, h0));
        renderbackend.endFrame();
    }

    CHECK((img.get()->getSurface()) != (nullptr));
    CHECK((SDL_GetPixelFormatDetails(alpha_img.get()->getSurface()->format)->Amask) != (0));
}
