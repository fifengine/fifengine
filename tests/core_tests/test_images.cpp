/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"
#include "util/structures/rect.h"
#include "util/time/timemanager.h"
#include "vfs/vfsdirectory.h"
#include "vfs/raw/rawdata.h"
#include "video/image_location.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/opengl/renderbackendopengl.h"
#include "loaders/native/video_loaders/image_loader.h"
#include "loaders/native/video_loaders/subimage_loader.h"
#include "util/base/exception.h"

using namespace FIFE;

static const std::string IMAGE_FILE = "tests/data/beach_e1.png";
static const std::string ALPHA_IMAGE_FILE = "tests/data/alpha_fidgit.png";
static const std::string SUBIMAGE_FILE = "tests/data/rpg_tiles_01.png";

// Environment
struct environment {
	boost::shared_ptr<TimeManager> timemanager;
	boost::shared_ptr<VFS> vfs;

	environment()
		: timemanager(new TimeManager()),
		  vfs(new VFS()) {
		vfs->addSource(new VFSDirectory(vfs.get()));
			if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
				throw SDLException(SDL_GetError());
			}
		}
};

void test_image(VFS* vfs, RenderBackend& renderbackend) {
	renderbackend.init();
	renderbackend.createMainScreen(800, 600, 0, false, "FIFE", "");

	ImageLoader provider(vfs);
	boost::scoped_ptr<Image> img(dynamic_cast<Image*>(provider.loadResource(ImageLocation(IMAGE_FILE))));
	
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

void test_subimage(VFS* vfs, RenderBackend& renderbackend) {
	renderbackend.init();
	renderbackend.createMainScreen(800, 600, 0, false, "FIFE", "");

	ImageLoader imgprovider(vfs);
	boost::scoped_ptr<Image> img(dynamic_cast<Image*>(imgprovider.loadResource(ImageLocation(SUBIMAGE_FILE))));

	ImageLocation location(SUBIMAGE_FILE);
	location.setParentSource(&*img);
	int W = img->getWidth();
	int w = W / 12;
	int H = img->getHeight();
	int h = H / 12;
	location.setWidth(w);
	location.setHeight(h);
	std::vector<Image*> subimages;

	SubImageLoader subprovider;
	for (int x = 0; x < (W - w); x+=w) {
		for (int y = 0; y < (H - h); y+=h) {
			location.setXShift(x);
			location.setYShift(y);
			Image* sub = dynamic_cast<Image*>(subprovider.loadResource(location));
			subimages.push_back(sub);
		}
	}
	
	for (unsigned int i = 0; i < 200; i++) {
		renderbackend.startFrame();
		subimages[i / 40]->render(Rect(200, 200, w, h));
		renderbackend.endFrame();
	}
	std::vector<Image*>::iterator i = subimages.begin();
	while (i != subimages.end()) {
		delete *i;
		i++;
	}

}

TEST(test_sdl_alphaoptimize)
{
	environment env;
	RenderBackendSDL renderbackend;
	renderbackend.init();
	renderbackend.createMainScreen(800, 600, 0, false, "FIFE", "");
	renderbackend.setAlphaOptimizerEnabled(true);
		
	ImageLoader provider(env.vfs.get());
	env.vfs.get()->exists(IMAGE_FILE);
	boost::scoped_ptr<Image> img(dynamic_cast<Image*>(provider.loadResource(ImageLocation(IMAGE_FILE))));
	env.vfs.get()->exists(ALPHA_IMAGE_FILE);
	boost::scoped_ptr<Image> alpha_img(dynamic_cast<Image*>(provider.loadResource(ImageLocation(ALPHA_IMAGE_FILE))));

	int h0 = img->getHeight();
	int w0 = img->getWidth();
	
	int h1 = alpha_img->getHeight();
	int w1 = alpha_img->getWidth();
	for(int i=0; i != 200; ++i) {
		renderbackend.startFrame();
		img->render(Rect(i, i, w0, h0));
		alpha_img->render(Rect(i, i, w1, h1));
		alpha_img->render(Rect(i, h0+i, w1, h1));
		img->render(Rect(i, h0+i, w0, h0));
		renderbackend.endFrame();
	}
	
	CHECK(img->getSurface()->format->Amask == 0);
	CHECK(alpha_img->getSurface()->format->Amask != 0);
}

TEST(test_sdl_image)
{
	environment env;
	RenderBackendSDL renderbackend;
	test_image(env.vfs.get(), renderbackend);
}

TEST(test_ogl_image)
{
	environment env;
	RenderBackendOpenGL renderbackend;
	test_image(env.vfs.get(), renderbackend);
}

TEST(test_sdl_subimage)
{
	environment env;
	RenderBackendSDL renderbackend;
	CHECK(env.vfs.get()->exists(IMAGE_FILE));
	test_subimage(env.vfs.get(), renderbackend);
}

TEST(test_ogl_subimage)
{
	environment env;
	RenderBackendOpenGL renderbackend;
	test_subimage(env.vfs.get(), renderbackend);
}

// need this here because SDL redefines 
// main to SDL_main in SDL_main.h
#ifdef main
#undef main
#endif

int main()
{
	return UnitTest::RunAllTests();
}
