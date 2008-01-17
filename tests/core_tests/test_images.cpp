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
#include "vfs/vfshostsystem.h"
#include "vfs/raw/rawdata.h"
#include "video/image_location.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/opengl/renderbackendopengl.h"
#include "loaders/native/video_loaders/image_provider.h"
#include "loaders/native/video_loaders/subimage_provider.h"
#include "loaders/native/video_loaders/animation_provider.h"
#include "util/exception.h"

using boost::unit_test::test_suite;
using namespace FIFE;

static const std::string IMAGE_FILE = "../data/beach_e1.png";
static const std::string ALPHA_IMAGE_FILE = "../data/alpha_fidgit.png";
static const std::string SUBIMAGE_FILE = "../data/rpg_tiles_01.png";

// Environment
struct environment {
	boost::shared_ptr<TimeManager> timemanager;
	boost::shared_ptr<VFSSourceFactory> vfssources;
	boost::shared_ptr<VFS> vfs;

	environment()
		: timemanager(new TimeManager()),
		  vfssources(new VFSSourceFactory()),
		  vfs(new VFS()) {
		VFS::instance()->addSource(new VFSHostSystem());
			if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
				throw SDLException(SDL_GetError());
			}
		}
};

void test_image(RenderBackend& renderbackend) {
	renderbackend.init();
	SDL_Surface* screen = renderbackend.createMainScreen(800, 600, 0, false);

	ImageProvider provider;
	boost::scoped_ptr<Image> img(dynamic_cast<Image*>(provider.createResource(ImageLocation(IMAGE_FILE))));
	
	int h = img->getHeight();
	int w = img->getWidth();
	for (int i = 0; i < 200; i++) {
		renderbackend.startFrame();
		img->render(Rect(i, i, w, h), screen);
		renderbackend.endFrame();
	}	
	for (int j = 0; j < 5; j++) {
		for (int i = -20; i < 20; i++) {
			renderbackend.startFrame();
			img->setXShift(i);
			img->setYShift(i);
			img->render(Rect(200, 200, w, h), screen);
			renderbackend.endFrame();
		}	
	}
}

void test_subimage(RenderBackend& renderbackend) {
	renderbackend.init();
	SDL_Surface* screen = renderbackend.createMainScreen(800, 600, 0, false);

	ImageProvider imgprovider;
	boost::scoped_ptr<Image> img(dynamic_cast<Image*>(imgprovider.createResource(ImageLocation(SUBIMAGE_FILE))));

	ImageLocation location(SUBIMAGE_FILE);
	location.setParentSource(&*img);
	int W = img->getWidth();
	int w = W / 12;
	int H = img->getHeight();
	int h = H / 12;
	location.setWidth(w);
	location.setHeight(h);
	std::vector<Image*> subimages;

	SubImageProvider subprovider;
	for (int x = 0; x < (W - w); x+=w) {
		for (int y = 0; y < (H - h); y+=h) {
			location.setXShift(x);
			location.setYShift(y);
			Image* sub = dynamic_cast<Image*>(subprovider.createResource(location));
			subimages.push_back(sub);
		}
	}
	
	for (unsigned int i = 0; i < 200; i++) {
		renderbackend.startFrame();
		subimages[i / 40]->render(Rect(200, 200, w, h), screen);
		renderbackend.endFrame();
	}
	std::vector<Image*>::iterator i = subimages.begin();
	while (i != subimages.end()) {
		delete *i;
		i++;
	}

}

void test_sdl_alphaoptimize() {
	environment env;
	RenderBackendSDL renderbackend;
	renderbackend.init();
	renderbackend.setRemoveFakeAlpha(true);
	SDL_Surface* screen = renderbackend.createMainScreen(800, 600, 0, false);
	
	ImageProvider provider;
	boost::scoped_ptr<Image> img(dynamic_cast<Image*>(provider.createResource(ImageLocation(IMAGE_FILE))));

	boost::scoped_ptr<Image> alpha_img(dynamic_cast<Image*>(provider.createResource(ImageLocation(ALPHA_IMAGE_FILE))));

	int h0 = img->getHeight();
	int w0 = img->getWidth();
	
	int h1 = alpha_img->getHeight();
	int w1 = alpha_img->getWidth();
	for(int i=0; i != 200; ++i) {
		renderbackend.startFrame();
		img->render(Rect(i, i, w0, h0), screen);
		alpha_img->render(Rect(i, i, w1, h1), screen);
		alpha_img->render(Rect(i, h0+i, w1, h1), screen);
		img->render(Rect(i, h0+i, w0, h0), screen);
		renderbackend.endFrame();
	}
	
	BOOST_CHECK(img->getSurface()->format->Amask == 0);
	BOOST_CHECK(alpha_img->getSurface()->format->Amask != 0);
}

#ifdef FIFE_BOOST_VERSION_103300
void test_sdl_image() {
#else
BOOST_AUTO_TEST_CASE( SDL_image_test ) {
#endif
	environment env;
	RenderBackendSDL renderbackend;
	test_image(renderbackend);
}

#ifdef FIFE_BOOST_VERSION_103300
void test_ogl_image() {
#else
BOOST_AUTO_TEST_CASE( OGL_image_test ) {
#endif
	environment env;
	RenderBackendOpenGL renderbackend;
	test_image(renderbackend);
}

#ifdef FIFE_BOOST_VERSION_103300
void test_sdl_subimage() {
#else
BOOST_AUTO_TEST_CASE( SDL_subimage_test ) {
#endif
	environment env;
	RenderBackendSDL renderbackend;
	test_subimage(renderbackend);
}

#ifdef FIFE_BOOST_VERSION_103300
void test_ogl_subimage() {
#else
BOOST_AUTO_TEST_CASE( OGL_subimage_test ) {
#endif
	environment env;
	RenderBackendOpenGL renderbackend;
	test_subimage(renderbackend);
}

#ifdef FIFE_BOOST_VERSION_103300
test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("Image Tests");
	test->add( BOOST_TEST_CASE( &test_sdl_subimage ),0 );
	test->add( BOOST_TEST_CASE( &test_ogl_subimage ),0 );
	test->add( BOOST_TEST_CASE( &test_sdl_image ),0 );
	test->add( BOOST_TEST_CASE( &test_ogl_image ),0 );
	test->add( BOOST_TEST_CASE( &test_sdl_alphaoptimize ),0 );

	return test;
}
#endif
