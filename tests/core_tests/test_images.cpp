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
#include <iostream>
#include <iomanip>
#include <memory>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"
#include "util/structures/rect.h"
#include "util/time/timemanager.h"
#include "vfs/vfsdirectory.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/devicecaps.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/opengl/renderbackendopengl.h"
#include "util/base/exception.h"

using namespace FIFE;

static const std::string IMAGE_FILE = "tests/data/beach_e1.png";
static const std::string ALPHA_IMAGE_FILE = "tests/data/alpha_fidgit.png";
static const std::string SUBIMAGE_FILE = "tests/data/rpg_tiles_01.png";

// Environment
struct environment {
    std::shared_ptr<TimeManager> timemanager;
		std::shared_ptr<VFS> vfs;
		std::shared_ptr<ImageManager> imageManager;

    environment()
		: timemanager(std::make_shared<TimeManager>()),
		  vfs(std::make_shared<VFS>()),
		  imageManager(std::make_shared<ImageManager>()) {
		vfs->addSource(new VFSDirectory(vfs.get()));
            if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {
                throw SDLException(SDL_GetError());
            }
        }
};

void test_image(RenderBackend& renderbackend, const ScreenMode& mode) {
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
void test_subimage(RenderBackend& renderbackend, const ScreenMode& mode) {
	renderbackend.init("");
	renderbackend.createMainScreen(mode, "FIFE", "");

	ImagePtr img = ImageManager::instance()->load(SUBIMAGE_FILE);
	REQUIRE(img);

	int W = img->getWidth();
	int w = W / 12;
	int H = img->getHeight();
	int h = H / 12;
	std::vector<ImagePtr> subimages;

	for (int x = 0; x < (W - w); x+=w) {
		for (int y = 0; y < (H - h); y+=h) {
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

	ImagePtr img = ImageManager::instance()->load(IMAGE_FILE);
	ImagePtr alpha_img = ImageManager::instance()->load(ALPHA_IMAGE_FILE);
	REQUIRE(img);
	REQUIRE(alpha_img);

	int h0 = img->getHeight();
	int w0 = img->getWidth();

	int h1 = alpha_img->getHeight();
	int w1 = alpha_img->getWidth();
	for(int i=0; i != 200; ++i) {
		renderbackend.startFrame();
		img.get()->render(Rect(i, i, w0, h0));
		alpha_img.get()->render(Rect(i, i, w1, h1));
		alpha_img.get()->render(Rect(i, h0+i, w1, h1));
		img.get()->render(Rect(i, h0+i, w0, h0));
		renderbackend.endFrame();
	}

	CHECK(img.get()->getSurface() != nullptr);
	CHECK(alpha_img.get()->getSurface()->format->Amask != 0);
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
