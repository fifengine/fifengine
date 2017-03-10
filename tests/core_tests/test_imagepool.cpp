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
static const std::string SUBIMAGE_FILE = "tests/data/rpg_tiles_01.png";
static const std::string ANIM_FILE = "tests/data/crate_full_001.xml";

// Environment
struct environment {
	boost::shared_ptr<TimeManager> timemanager;
	boost::shared_ptr<VFS> vfs;

	environment()
		: timemanager(new TimeManager()), vfs(new VFS()) {
			if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
				throw SDLException(SDL_GetError());
			}
		}
};

TEST_FIXTURE(environment, test_image_pool)
{
	RenderBackendSDL renderbackend;

	vfs->addSource(new VFSDirectory(vfs.get()));

	renderbackend.init();
	renderbackend.createMainScreen(800, 600, 0, false, "FIFE", "");
	ImagePool pool;
	pool.addResourceLoader(new SubImageLoader());
	pool.addResourceLoader(new ImageLoader(vfs.get()));

	CHECK_EQUAL(0, pool.getResourceCount(RES_LOADED));
	CHECK_EQUAL(0, pool.getResourceCount(RES_NON_LOADED));

	ImageLocation location(IMAGE_FILE);
	pool.addResourceFromLocation(&location);
	CHECK_EQUAL(0, pool.getResourceCount(RES_LOADED));
	CHECK_EQUAL(1, pool.getResourceCount(RES_NON_LOADED));

	location = ImageLocation(SUBIMAGE_FILE);
	ImageLoader imgprovider(vfs.get());
	int fullImgInd = pool.addResourceFromLocation(&location);
	CHECK_EQUAL(0, pool.getResourceCount(RES_LOADED));
	CHECK_EQUAL(2, pool.getResourceCount(RES_NON_LOADED));
	Image& img = pool.getImage(fullImgInd);
	CHECK_EQUAL(1, pool.getResourceCount(RES_LOADED));
	CHECK_EQUAL(1, pool.getResourceCount(RES_NON_LOADED));

	location.setParentSource(&img);
	int W = img.getWidth();
	int w = W / 12;
	int H = img.getHeight();
	int h = H / 12;
	location.setWidth(w);
	location.setHeight(h);
	CHECK(w != 0 && h !=0);

	int subImgInd = pool.addResourceFromLocation(&location);
	CHECK(fullImgInd != subImgInd);

	CHECK_EQUAL(1, pool.getResourceCount(RES_LOADED));
	CHECK_EQUAL(2, pool.getResourceCount(RES_NON_LOADED));

	for (int k = 0; k < 3; k++) {
		for (int j = 0, s = pool.getResourceCount(RES_LOADED | RES_NON_LOADED); j < s; j++) {
			std::cout << j << std::endl;
			Image& r = dynamic_cast<Image&>(pool.get(j));
			int h = r.getHeight();
			int w = r.getWidth();
			for (int i = 20; i > 0; i-=2) {
				renderbackend.startFrame();
				r.render(Rect(i, i, w, h));
				renderbackend.endFrame();
				TimeManager::instance()->update();
			}
		}
		CHECK_EQUAL(3, pool.getResourceCount(RES_LOADED));
		CHECK_EQUAL(0, pool.getResourceCount(RES_NON_LOADED));
	}
	CHECK_EQUAL(3, pool.getResourceCount(RES_LOADED));
	CHECK_EQUAL(0, pool.getResourceCount(RES_NON_LOADED));

	CHECK_EQUAL(3, pool.purgeLoadedResources() );

	CHECK_EQUAL(0, pool.getResourceCount(RES_LOADED));
	CHECK_EQUAL(3, pool.getResourceCount(RES_NON_LOADED));
	pool.reset();
	CHECK_EQUAL(0, pool.getResourceCount(RES_LOADED));
	CHECK_EQUAL(0, pool.getResourceCount(RES_NON_LOADED));
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


