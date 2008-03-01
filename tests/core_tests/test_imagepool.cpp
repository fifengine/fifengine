/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes
#include <iostream>
#include <iomanip>

// Platform specific includes

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
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/raw/rawdata.h"
#include "video/image_location.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/opengl/renderbackendopengl.h"
#include "loaders/native/video_loaders/image_loader.h"
#include "loaders/native/video_loaders/subimage_loader.h"
#include "loaders/native/video_loaders/xml_animation_loader.h"
#include "util/base/exception.h"

#include "fife_unit_test.h"

using boost::unit_test::test_suite;
using namespace FIFE;

static const std::string IMAGE_FILE = "../data/beach_e1.png";
static const std::string SUBIMAGE_FILE = "../data/rpg_tiles_01.png";
static const std::string ANIM_FILE = "../data/crate_full_001.xml";

// Environment
struct environment {
	boost::shared_ptr<TimeManager> timemanager;
	boost::shared_ptr<VFS> vfs;

	environment()
		: timemanager(new TimeManager()) {
			if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
				throw SDLException(SDL_GetError());
			}
		}
};

#ifdef FIFE_BOOST_VERSION_103300
void test_image_pool() {
#else
BOOST_AUTO_TEST_CASE( ImagePool_test ) {
#endif
	environment env;
	RenderBackendSDL renderbackend;

	boost::scoped_ptr<VFS> vfs(new VFS());
	vfs->addSource(new VFSDirectory(vfs.get()));

	renderbackend.init();
	renderbackend.createMainScreen(800, 600, 0, false);
	ImagePool pool;
	pool.addResourceLoader(new SubImageLoader());
	pool.addResourceLoader(new ImageLoader(vfs.get()));
	BOOST_CHECK(pool.getResourceCount(RES_LOADED) == 0);
	BOOST_CHECK(pool.getResourceCount(RES_NON_LOADED) == 0);

	pool.addResourceFromLocation(ImageLocation(IMAGE_FILE));
	BOOST_CHECK(pool.getResourceCount(RES_LOADED) == 0);
	BOOST_CHECK(pool.getResourceCount(RES_NON_LOADED) == 1);

	ImageLocation location(SUBIMAGE_FILE);
	ImageLoader imgprovider(vfs.get());
	int fullImgInd = pool.addResourceFromLocation(ImageLocation(SUBIMAGE_FILE));
	BOOST_CHECK(pool.getResourceCount(RES_LOADED) == 0);
	BOOST_CHECK(pool.getResourceCount(RES_NON_LOADED) == 2);

	Image& img = pool.getImage(fullImgInd);
	BOOST_CHECK(pool.getResourceCount(RES_LOADED) == 1);
	BOOST_CHECK(pool.getResourceCount(RES_NON_LOADED) == 1);

	location.setParentSource(&img);
	int W = img.getWidth();
	int w = W / 12;
	int H = img.getHeight();
	int h = H / 12;
	location.setWidth(w);
	location.setHeight(h);
	pool.addResourceFromLocation(location);
	BOOST_CHECK(pool.getResourceCount(RES_LOADED) == 1);
	BOOST_CHECK(pool.getResourceCount(RES_NON_LOADED) == 2);

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
		BOOST_CHECK(pool.getResourceCount(RES_LOADED) == 3);
		BOOST_CHECK(pool.getResourceCount(RES_NON_LOADED) == 0);
	}
	pool.clear();
	BOOST_CHECK(pool.getResourceCount(RES_LOADED) == 0);
	BOOST_CHECK(pool.getResourceCount(RES_NON_LOADED) == 0);
}

#ifdef FIFE_BOOST_VERSION_103300
test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("Image Pool Tests");
	test->add( BOOST_TEST_CASE( &test_image_pool ),0 );
	return test;
}
#endif
