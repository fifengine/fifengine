// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "util/time/timemanager.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "video/devicecaps.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/sdl/renderbackendsdl.h"

using FIFE::Image;
using FIFE::ImageManager;
using FIFE::ImagePtr;
using FIFE::Rect;
using FIFE::RenderBackend;
using FIFE::RenderBackendOpenGL;
using FIFE::RenderBackendSDL;
using FIFE::ScreenMode;
using FIFE::SDLException;
using FIFE::TimeManager;
using FIFE::VFS;
using FIFE::VFSDirectory;

static std::string const IMAGE_FILE       = "tests/data/beach_e1.png";
static std::string const ALPHA_IMAGE_FILE = "tests/data/alpha_fidgit.png";
static std::string const SUBIMAGE_FILE    = "tests/data/rpg_tiles_01.png";

// Environment
struct environment
{
        std::shared_ptr<TimeManager> timemanager;
        std::shared_ptr<VFS> vfs;
        std::shared_ptr<ImageManager> imageManager;

        environment() :
            timemanager(std::make_shared<TimeManager>()),
            vfs(std::make_shared<VFS>()),
            imageManager(std::make_shared<ImageManager>())
        {
            vfs->addSource(new VFSDirectory(vfs.get()));
            if (!SDL_Init(SDL_INIT_TIMER)) {
                throw SDLException(SDL_GetError());
            }
        }
};

void test_image(RenderBackend& renderbackend, ScreenMode const & mode)
{
    renderbackend.init("");
    renderbackend.createMainScreen(mode, "FIFE", "");

    ImagePtr img = ImageManager::instance()->load(IMAGE_FILE);
    REQUIRE(img);

    int h = img->getHeight();
    int w = img->getWidth();
    for (int i = 0; i < 100; i++) {
        renderbackend.startFrame();
        img->render(Rect(i, i, w, h));
        renderbackend.endFrame();
    }
    for (int j = 0; j < 5; j++) {
        for (int i = -10; i < 10; i++) {
            renderbackend.startFrame();
            img->setXShift(i);
            img->setYShift(i);
            img->render(Rect(200, 200, w, h));
            renderbackend.endFrame();
        }
    }
}
void test_subimage(RenderBackend& renderbackend, ScreenMode const & mode)
{
    renderbackend.init("");
    renderbackend.createMainScreen(mode, "FIFE", "");

    ImagePtr img = ImageManager::instance()->load(SUBIMAGE_FILE);
    REQUIRE(img);

    int W = img->getWidth();
    int w = W / 12;
    int H = img->getHeight();
    int h = H / 12;
    std::vector<ImagePtr> subimages;

    for (int x = 0; x < (W - w); x += w) {
        for (int y = 0; y < (H - h); y += h) {
            ImagePtr sub = ImageManager::instance()->create();
            sub->useSharedImage(img, Rect(x, y, w, h));
            subimages.push_back(sub);
        }
    }

    for (unsigned int i = 0; i < 200; i++) {
        renderbackend.startFrame();
        subimages[i / 40].get()->render(Rect(200, 200, w, h));
        renderbackend.endFrame();
    }
}

TEST_CASE("test_sdl_alphaoptimize")
{
    environment env;
    RenderBackendSDL renderbackend(SDL_Color{0, 0, 0, 0});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_SDL), "FIFE", "");
    renderbackend.setAlphaOptimizerEnabled(true);

    ImagePtr img       = ImageManager::instance()->load(IMAGE_FILE);
    ImagePtr alpha_img = ImageManager::instance()->load(ALPHA_IMAGE_FILE);
    REQUIRE(img);
    REQUIRE(alpha_img);

    int h0 = img->getHeight();
    int w0 = img->getWidth();

    int h1 = alpha_img->getHeight();
    int w1 = alpha_img->getWidth();
    for (int i = 0; i != 200; ++i) {
        renderbackend.startFrame();
        img.get()->render(Rect(i, i, w0, h0));
        alpha_img.get()->render(Rect(i, i, w1, h1));
        alpha_img.get()->render(Rect(i, h0 + i, w1, h1));
        img.get()->render(Rect(i, h0 + i, w0, h0));
        renderbackend.endFrame();
    }

    CHECK_NE(img.get()->getSurface(), nullptr);
    CHECK_NE(SDL_GetPixelFormatDetails(alpha_img.get()->getSurface()->format)->Amask, 0);
}

TEST_CASE("test_sdl_image")
{
    environment env;
    RenderBackendSDL renderbackend(SDL_Color{0, 0, 0, 0});
    test_image(renderbackend, ScreenMode(800, 600, 32, ScreenMode::WINDOWED_SDL));
}

TEST_CASE("test_ogl_image")
{
    environment env;
    RenderBackendOpenGL renderbackend(SDL_Color{0, 0, 0, 0});
    test_image(renderbackend, ScreenMode(800, 600, 32, ScreenMode::WINDOWED_OPENGL));
}

TEST_CASE("test_sdl_subimage")
{
    environment env;
    RenderBackendSDL renderbackend(SDL_Color{0, 0, 0, 0});
    test_subimage(renderbackend, ScreenMode(800, 600, 32, ScreenMode::WINDOWED_SDL));
}

TEST_CASE("test_ogl_subimage")
{
    environment env;
    RenderBackendOpenGL renderbackend(SDL_Color{0, 0, 0, 0});
    test_subimage(renderbackend, ScreenMode(800, 600, 32, ScreenMode::WINDOWED_OPENGL));
}
