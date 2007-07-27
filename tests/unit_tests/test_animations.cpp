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
#include "video/animation.h"
#include "video/screen.h"
#include "video/renderbackends/sdl/renderbackendsdl.h"
#include "video/renderbackends/opengl/renderbackendopengl.h"
#include "loaders/native/video_loaders/animation_provider.h"
#include "util/exception.h"
#include "util/log.h"

using boost::unit_test::test_suite;
using namespace FIFE;

static const std::string ANIM_FILE = "../data/crate_full_001.xml";

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

class AnimRoller: public AnimationListener {
	void onAnimationEnd(Animation* animation) {
		animation->setDirection(!animation->isDirectionForward());
		animation->setActive(true);
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
		roller = new AnimRoller();
		anim->setAnimationListener(roller);
		anim->setActive(true);
	}
	~PositionedAnimation() { 
		anim->setAnimationListener(NULL);
		delete roller;
		delete anim;
	}
private:
	AnimRoller* roller;
};

void test_animation(RenderBackend& renderbackend) {
	renderbackend.init();
	const int W = 800;
	const int H = 600;
	Screen* screen = renderbackend.createMainScreen(W, H, 0, false);

	std::vector<PositionedAnimation*> animations;
	AnimationProvider provider;
	boost::scoped_ptr<Animation> anim(provider.createResource(RenderableLocation(ANIM_FILE)));
	int h = anim->getHeight();
	int w = anim->getWidth();
	for (int x = 0; x < (W - w); x+=w) {
		for (int y = 0; y < (H - h); y+=h) {
			Animation* a = provider.createResource(RenderableLocation(ANIM_FILE));
			animations.push_back(new PositionedAnimation(x, y, a));
		}
	}
	for (int i = 0; i < 400; i++) {
		renderbackend.startFrame();
		std::vector<PositionedAnimation*>::iterator i = animations.begin();
		while (i != animations.end()) {
			(*i)->anim->render(Rect((*i)->x, (*i)->y, w, h), screen);
			i++;
		}
		TimeManager::instance()->update();
		renderbackend.endFrame();
	}
	std::vector<PositionedAnimation*>::iterator i = animations.begin();
	while (i != animations.end()) {
		delete *i;
		i++;
	}
}

void test_sdl() {
	environment env;
	RenderBackendSDL renderbackend;
	test_animation(renderbackend);
}

void test_ogl() {
	environment env;
	RenderBackendOpenGL renderbackend;
	test_animation(renderbackend);
}

test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("Image Tests");
	test->add( BOOST_TEST_CASE( &test_sdl ),0 );
	test->add( BOOST_TEST_CASE( &test_ogl ),0 );

	return test;
}
