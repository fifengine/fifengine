// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

// Platform specific includes
#include "fife_unittest.h"
#include "fixture.h"

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/renderbackend.h"
#include "video/sdl/renderbackendsdl.h"

static char const * const IMAGE_FILE       = "docs/logo/FIFE_small_c3.png";
static char const * const ALPHA_IMAGE_FILE = "tests/data/alpha_fidgit.png";
static char const * const SUBIMAGE_FILE    = "tests/data/rpg_tiles_01.png";

TEST_CASE("RenderBackendSDL renders FIFE_small_c3.png with position shift", "[core][images]")
{
    TestFixture const _init;
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(FIFE::ScreenMode(800, 600, 32, FIFE::ScreenMode::WINDOWED_SDL), "FIFE", "");

    {
        SDL_Rect rect     = {.x = 0, .y = 0, .w = 1, .h = 1};
        SDL_Surface* surf = SDL_RenderReadPixels(renderbackend.getRenderer(), &rect);
        REQUIRE(surf != nullptr);
        SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(surf->format);
        uint8_t r                          = 0;
        uint8_t g                          = 0;
        uint8_t b                          = 0;
        uint8_t a                          = 0;
        SDL_GetRGBA(*static_cast<uint32_t const *>(surf->pixels), fmt, nullptr, &r, &g, &b, &a);
        CHECK_EQ(r, 0);
        CHECK_EQ(g, 0);
        CHECK_EQ(b, 0);
        CHECK_EQ(a, 255);
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
        std::memcpy(
            &pix,
            static_cast<uint8_t const *>(s->pixels) + (172 * static_cast<ptrdiff_t>(s->pitch)) + (134 * 4),
            sizeof(pix));
        SDL_GetRGBA(pix, f, nullptr, &surf_r, &surf_g, &surf_b, &surf_a);
    }

    int h = static_cast<int>(img->getHeight());
    int w = static_cast<int>(img->getWidth());

    // Render first frame at (0,0) and verify a known pixel from the image
    renderbackend.startFrame();
    img->render(FIFE::Rect(0, 0, w, h));

    {
        SDL_Rect rect     = {.x = 134, .y = 172, .w = 1, .h = 1};
        SDL_Surface* surf = SDL_RenderReadPixels(renderbackend.getRenderer(), &rect);
        REQUIRE(surf != nullptr);
        SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(surf->format);
        uint8_t r                          = 0;
        uint8_t g                          = 0;
        uint8_t b                          = 0;
        uint8_t a                          = 0;
        SDL_GetRGBA(*static_cast<uint32_t const *>(surf->pixels), fmt, nullptr, &r, &g, &b, &a);
        CHECK_EQ(r, surf_r);
        CHECK_EQ(g, surf_g);
        CHECK_EQ(b, surf_b);
        CHECK_EQ(a, surf_a);
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
    FIFE::RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(FIFE::ScreenMode(800, 600, 32, FIFE::ScreenMode::WINDOWED_OPENGL), "FIFE", "");

    FIFE::ImagePtr img = FIFE::ImageManager::instance()->load(IMAGE_FILE);
    REQUIRE(img);

    int h = static_cast<int>(img->getHeight());
    int w = static_cast<int>(img->getWidth());
    for (int i = 0; i < 100; i++) {
        renderbackend.startFrame();
        img->render(FIFE::Rect(i, i, w, h));
        renderbackend.endFrame();
    }
}

TEST_CASE("RenderBackendSDL renders subimages from rpg_tiles_01.png", "[core][images]")
{
    TestFixture const _init;
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(FIFE::ScreenMode(800, 600, 32, FIFE::ScreenMode::WINDOWED_SDL), "FIFE", "");

    FIFE::ImagePtr img = FIFE::ImageManager::instance()->load(SUBIMAGE_FILE);
    REQUIRE(img);

    int W = static_cast<int>(img->getWidth());
    int w = W / 12;
    int H = static_cast<int>(img->getHeight());
    int h = H / 12;
    std::vector<FIFE::ImagePtr> subimages;

    for (int x = 0; x < (W - w); x += w) {
        for (int y = 0; y < (H - h); y += h) {
            FIFE::ImagePtr sub = FIFE::ImageManager::instance()->create();
            sub->useSharedImage(img, FIFE::Rect(x, y, w, h));
            subimages.push_back(sub);
        }
    }

    for (unsigned int i = 0; i < 200; i++) {
        renderbackend.startFrame();
        subimages[i / 40].get()->render(FIFE::Rect(200, 200, w, h));
        renderbackend.endFrame();
    }
}

TEST_CASE("RenderBackendOpenGL renders subimages from rpg_tiles_01.png", "[core][images]")
{
    TestFixture const _init;
    FIFE::RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(FIFE::ScreenMode(800, 600, 32, FIFE::ScreenMode::WINDOWED_OPENGL), "FIFE", "");

    FIFE::ImagePtr img = FIFE::ImageManager::instance()->load(SUBIMAGE_FILE);
    REQUIRE(img);

    int W = static_cast<int>(img->getWidth());
    int w = W / 12;
    int H = static_cast<int>(img->getHeight());
    int h = H / 12;
    std::vector<FIFE::ImagePtr> subimages;

    for (int x = 0; x < (W - w); x += w) {
        for (int y = 0; y < (H - h); y += h) {
            FIFE::ImagePtr sub = FIFE::ImageManager::instance()->create();
            sub->useSharedImage(img, FIFE::Rect(x, y, w, h));
            subimages.push_back(sub);
        }
    }

    for (unsigned int i = 0; i < 200; i++) {
        renderbackend.startFrame();
        subimages[i / 40].get()->render(FIFE::Rect(200, 200, w, h));
        renderbackend.endFrame();
    }
}

TEST_CASE("RenderBackendSDL alpha optimizer renders fife_logo.png and alpha_fidgit.png", "[core][images]")
{
    TestFixture const _init;
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(FIFE::ScreenMode(800, 600, 32, FIFE::ScreenMode::WINDOWED_SDL), "FIFE", "");
    renderbackend.setAlphaOptimizerEnabled(true);

    FIFE::ImagePtr img       = FIFE::ImageManager::instance()->load(IMAGE_FILE);
    FIFE::ImagePtr alpha_img = FIFE::ImageManager::instance()->load(ALPHA_IMAGE_FILE);
    REQUIRE(img);
    REQUIRE(alpha_img);

    int h0 = static_cast<int>(img->getHeight());
    int w0 = static_cast<int>(img->getWidth());

    int h1 = static_cast<int>(alpha_img->getHeight());
    int w1 = static_cast<int>(alpha_img->getWidth());
    for (int i = 0; i != 200; ++i) {
        renderbackend.startFrame();
        img.get()->render(FIFE::Rect(i, i, w0, h0));
        alpha_img.get()->render(FIFE::Rect(i, i, w1, h1));
        alpha_img.get()->render(FIFE::Rect(i, h0 + i, w1, h1));
        img.get()->render(FIFE::Rect(i, h0 + i, w0, h0));
        renderbackend.endFrame();
    }

    CHECK_NE(img.get()->getSurface(), nullptr);
    CHECK_NE(SDL_GetPixelFormatDetails(alpha_img.get()->getSurface()->format)->Amask, 0);
}
