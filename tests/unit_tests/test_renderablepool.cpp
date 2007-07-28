/***************************************************************************
*   Copyright (C) 2005-2007 by the FIFE Team                              *
*   fife-public@lists.sourceforge.net                                     *
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

// 3rd party library includes
#include <boost/test/unit_test.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfssourcefactory.h"
#include "vfs/vfs.h"
#include "util/rect.h"
#include "util/settingsmanager.h"
#include "util/time/timemanager.h"
#include "vfs/vfs.h"
#include "vfs/vfshostsystem.h"
#include "vfs/raw/rawdata.h"
#include "video/renderable_location.h"
#include "video/image.h"
#include "video/animation.h"
#include "video/screen.h"
#include "video/renderablepool.h"
#include "video/renderbackends/sdl/renderbackendsdl.h"
#include "video/renderbackends/opengl/renderbackendopengl.h"
#include "loaders/native/video_loaders/image_provider.h"
#include "loaders/native/video_loaders/subimage_provider.h"
#include "loaders/native/video_loaders/animation_provider.h"
#include "util/exception.h"
#include "util/log.h"

using boost::unit_test::test_suite;
using namespace FIFE;

static const std::string IMAGE_FILE = "../../content/gfx/tiles/beach/beach_e1.png";
static const std::string SUBIMAGE_FILE = "../../content/gfx/tiles/rpg_tiles_01.png";
static const std::string ANIM_FILE = "../data/crate_full_001.xml";

class AnimRoller: public AnimationListener {
	void onAnimationEnd(Animation* animation) {
		animation->setDirection(!animation->isDirectionForward());
		animation->setActive(true);
	}
};
// Environment
struct environment {
	boost::shared_ptr<SettingsManager> settings;
	boost::shared_ptr<TimeManager> timemanager;
	boost::shared_ptr<VFSSourceFactory> vfssources;
	boost::shared_ptr<VFS> vfs;

	environment()
		: settings(new SettingsManager()),
		  timemanager(new TimeManager()),
		  vfssources(new VFSSourceFactory()),
		  vfs(new VFS()) {
		Log::setLogLevel(Log::LEVEL_MAX);
		VFS::instance()->addSource(new VFSHostSystem());
			if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
				throw SDLException(SDL_GetError());
			}
		}
};

void test_renderable_pool() {
	environment env;
	RenderBackendSDL renderbackend;

	renderbackend.init();
	Screen* screen = renderbackend.createMainScreen(800, 600, 0, false);
	RenderablePool pool;
	pool.addResourceProvider(new SubImageProvider());
	pool.addResourceProvider(new ImageProvider());
	pool.addResourceProvider(new AnimationProvider());
	pool.addResourceFromLocation(new RenderableLocation(IMAGE_FILE));
	int animind = pool.addResourceFromLocation(new RenderableLocation(ANIM_FILE));
	Animation& anim = dynamic_cast<Animation&>(pool.get(animind));
	boost::scoped_ptr<AnimRoller> roller(new AnimRoller());
	anim.setAnimationListener(&*roller);
	anim.setActive(true);

	RenderableLocation* location = new RenderableLocation(SUBIMAGE_FILE);
	ImageProvider imgprovider;
	boost::scoped_ptr<Image> img(dynamic_cast<Image*>(imgprovider.createResource(RenderableLocation(SUBIMAGE_FILE))));
	location->setParentSource(&*img);
	int W = img->getWidth();
	int w = W / 12;
	int H = img->getHeight();
	int h = H / 12;
	location->setWidth(w);
	location->setHeight(h);
	std::cout << pool.addResourceFromLocation(location) << std::endl;

	for (int k = 0; k < 3; k++) {
		for (int j = 0, s = pool.getSize(); j < s; j++) {
			std::cout << j << std::endl;
			Renderable& r = dynamic_cast<Renderable&>(pool.get(j));
			int h = r.getHeight();
			int w = r.getWidth();
			for (int i = 200; i > 0; i--) {
				renderbackend.startFrame();
				r.render(Rect(i, i, w, h), screen);
				renderbackend.endFrame();
				TimeManager::instance()->update();
			}
		}
	}
}

test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("Image Tests");
	test->add( BOOST_TEST_CASE( &test_renderable_pool ),0 );
	return test;
}
