/***************************************************************************
 *   Copyright (C) 2005-2019 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
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
using FIFE::RenderBackendSDL;
using FIFE::ScreenMode;
using FIFE::SDLException;
using FIFE::TimeManager;
using FIFE::VFS;
using FIFE::VFSDirectory;

static const std::string IMAGE_FILE    = "tests/data/beach_e1.png";
static const std::string SUBIMAGE_FILE = "tests/data/rpg_tiles_01.png";
static const std::string ANIM_FILE     = "tests/data/crate_full_001.xml";

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
        if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {
            throw SDLException(SDL_GetError());
        }
    }
};

TEST_CASE_METHOD(environment, "test_image_pool")
{
    ImageManager* imageManager = ImageManager::instance();
    imageManager->removeAll();

    RenderBackendSDL renderbackend(SDL_Color{0, 0, 0, 0});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_SDL), "FIFE", "");

    CHECK((0) == (imageManager->getTotalResources()));
    CHECK((0) == (imageManager->getTotalResourcesLoaded()));

    ImagePtr image = imageManager->load(IMAGE_FILE);
    CHECK((1) == (imageManager->getTotalResourcesLoaded()));

    ImagePtr atlas = imageManager->load(SUBIMAGE_FILE);
    CHECK((2) == (imageManager->getTotalResourcesLoaded()));

    int W = atlas->getWidth();
    int w = W / 12;
    int H = atlas->getHeight();
    int h = H / 12;
    CHECK_NE(w, 0);
    CHECK_NE(h, 0);

    ImagePtr subImage = imageManager->create();
    subImage->useSharedImage(atlas, Rect(0, 0, w, h));

    CHECK((3) == (imageManager->getTotalResourcesLoaded()));

    for (int k = 0; k < 3; k++) {
        for (int j = 0; j < 3; j++) {
            Image* r = (j == 0) ? image.get() : ((j == 1) ? atlas.get() : subImage.get());
            int h    = r->getHeight();
            int w    = r->getWidth();
            for (int i = 20; i > 0; i -= 2) {
                renderbackend.startFrame();
                r->render(Rect(i, i, w, h));
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
