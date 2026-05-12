// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>

// Platform specific includes
#include "fife_unittest.h"
#include "fixture.h"

// FIFE includes
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/sdl/renderbackendsdl.h"

using FIFE::Image;
using FIFE::ImageManager;
using FIFE::ImagePtr;
using FIFE::Rect;
using FIFE::RenderBackendSDL;
using FIFE::ScreenMode;
using FIFE::TimeManager;

static char const * const IMAGE_FILE    = "tests/data/beach_e1.png";
static char const * const SUBIMAGE_FILE = "tests/data/rpg_tiles_01.png";

struct environment : TestFixture
{
};

TEST_CASE_METHOD(environment, "ImageManager pool loads and frees beach_e1.png and rpg_tiles_01.png", "[imagepool]")
{
    ImageManager* imageManager = ImageManager::instance();
    imageManager->removeAll();

    RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_SDL), "FIFE", "");

    CHECK((0) == (imageManager->getTotalResources()));
    CHECK((0) == (imageManager->getTotalResourcesLoaded()));

    ImagePtr image = imageManager->load(IMAGE_FILE);
    CHECK((1) == (imageManager->getTotalResourcesLoaded()));

    ImagePtr atlas = imageManager->load(SUBIMAGE_FILE);
    CHECK((2) == (imageManager->getTotalResourcesLoaded()));

    uint32_t W = atlas->getWidth();
    int w      = static_cast<int>(W / 12);
    uint32_t H = atlas->getHeight();
    int h      = static_cast<int>(H / 12);
    CHECK_NE(w, 0);
    CHECK_NE(h, 0);

    ImagePtr subImage = imageManager->create();
    subImage->useSharedImage(atlas, Rect(0, 0, w, h));

    CHECK((3) == (imageManager->getTotalResourcesLoaded()));

    for (int k = 0; k < 3; k++) {
        for (int j = 0; j < 3; j++) {
            Image* r   = (j == 0) ? image.get() : ((j == 1) ? atlas.get() : subImage.get());
            int height = static_cast<int>(r->getHeight());
            int width  = static_cast<int>(r->getWidth());
            for (int i = 20; i > 0; i -= 2) {
                renderbackend.startFrame();
                r->render(Rect(i, i, width, height));
                renderbackend.endFrame();
                TimeManager::instance()->update();
            }
        }
        CHECK((3) == (imageManager->getTotalResourcesLoaded()));
    }
    CHECK((3) == (imageManager->getTotalResourcesLoaded()));

    imageManager->freeAll();
    CHECK((0) == (imageManager->getTotalResourcesLoaded()));

    imageManager->removeAll();
    CHECK((0) == (imageManager->getTotalResources()));
}
// need this here because SDL redefines

// main to SDL_main in SDL_main.h
