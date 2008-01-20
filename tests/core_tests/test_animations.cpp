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

// Platform specific includes
#include "util/fife_unit_test.h"

// 3rd party library includes
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

#include "util/time/timemanager.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/raw/rawdata.h"
#include "video/image_location.h"
#include "video/animation.h"
#include "video/imagepool.h"
#include "video/animationpool.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/opengl/renderbackendopengl.h"
#include "loaders/native/video_loaders/animation_provider.h"
#include "loaders/native/video_loaders/image_provider.h"
#include "util/exception.h"

using boost::unit_test::test_suite;
using namespace FIFE;

static const std::string ANIM_FILE = "../data/crate_full_001.xml";

// Environment
struct environment {
	boost::shared_ptr<TimeManager> timemanager;
	boost::shared_ptr<VFSSourceFactory> vfssources;
	boost::shared_ptr<VFS> vfs;

	environment()
		: timemanager(new TimeManager()),
		  vfssources(new VFSSourceFactory()),
		  vfs(new VFS()) {
		VFS::instance()->addSource(new VFSDirectory());
			if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
				throw SDLException(SDL_GetError());
			}
		}
};

class PositionedAnimation {
public:
	int x;
	int y;
	Animation* anim;
	PositionedAnimation(int ax, int ay, Animation* aanim):  
		x(ax), 
		y(ay), 
		anim(aanim) {
	}
	~PositionedAnimation() { 
	}
};

void test_animation(RenderBackend& renderbackend) {
	renderbackend.init();
	const int W = 800;
	const int H = 600;
	SDL_Surface* screen = renderbackend.createMainScreen(W, H, 0, false);

	ImagePool* imagepool = new ImagePool();
	imagepool->addResourceProvider(new ImageProvider());

	AnimationPool* animpool = new AnimationPool();
	animpool->addResourceProvider(new AnimationProvider(imagepool));

	Animation& anim = animpool->getAnimation(animpool->addResourceFromFile(ANIM_FILE));

	int h = anim.getFrame(0)->getHeight();
	int w = anim.getFrame(0)->getWidth();

	std::vector<PositionedAnimation*> animations;
	for (int x = 0; x < (W - w); x+=w) {
		for (int y = 0; y < (H - h); y+=h) {
			animations.push_back(new PositionedAnimation(x, y, &anim));
		}
	}
	
	int start_ticks = SDL_GetTicks();
	for (int i = 0; i < 200; i++) {
		renderbackend.startFrame();
		std::vector<PositionedAnimation*>::iterator i = animations.begin();
		while (i != animations.end()) {
			int frametime = SDL_GetTicks() - start_ticks;
			Animation* a = (*i)->anim;
			Image* frame = a->getFrameByTimestamp(frametime);
			if (!frame) {
				start_ticks = SDL_GetTicks();
				continue;
			}
			frame->render(Rect((*i)->x, (*i)->y, w, h), screen);
			i++;
		}
		renderbackend.endFrame();
	}
	std::vector<PositionedAnimation*>::iterator i = animations.begin();
	while (i != animations.end()) {
		delete *i;
		i++;
	}
	delete animpool;
	delete imagepool;
}

#ifdef FIFE_BOOST_VERSION_103300
void test_sdl() {
#else
BOOST_AUTO_TEST_CASE( SDL_animtest ) {
#endif
	environment env;
	RenderBackendSDL renderbackend;
	test_animation(renderbackend);
}

#ifdef FIFE_BOOST_VERSION_103300
void test_ogl() {
#else
BOOST_AUTO_TEST_CASE( OGL_animtest ) {
#endif
	environment env;
	RenderBackendOpenGL renderbackend;
	test_animation(renderbackend);
}

#ifdef FIFE_BOOST_VERSION_103300
test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("Animation Tests");
	test->add( BOOST_TEST_CASE( &test_sdl ),0 );
	test->add( BOOST_TEST_CASE( &test_ogl ),0 );

	return test;
}
#endif
