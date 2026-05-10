// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "fife_unittest.h"
#include "fixture.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/renderbackend.h"
#include "video/sdl/renderbackendsdl.h"

static char const * const IMAGE_FILE       = "tests/data/fife_logo.png";
static char const * const ALPHA_IMAGE_FILE = "tests/data/alpha.png";
static char const * const SUBIMAGE_FILE    = "tests/data/rpg_tiles_01.png";

TEST_CASE("test_sdl_image")
{
    TestFixture const _init;
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 0});
    renderbackend.init("");
    renderbackend.createMainScreen(FIFE::ScreenMode(800, 600, 32, FIFE::ScreenMode::WINDOWED_SDL), "FIFE", "");

    FIFE::ImagePtr img = FIFE::ImageManager::instance()->load(IMAGE_FILE);
    REQUIRE(img);

    int h = static_cast<int>(img->getHeight());
    int w = static_cast<int>(img->getWidth());
    for (int i = 0; i < 100; i++) {
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

TEST_CASE("test_ogl_image")
{
    TestFixture const _init;
    FIFE::RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 0});
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

TEST_CASE("test_sdl_subimage")
{
    TestFixture const _init;
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 0});
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

TEST_CASE("test_ogl_subimage")
{
    TestFixture const _init;
    FIFE::RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 0});
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

TEST_CASE("test_sdl_alphaoptimize")
{
    TestFixture const _init;
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 0});
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
