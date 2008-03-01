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

// Platform specific includes

// 3rd party library includes
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <SDL.h>
#include <guichan.hpp>

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
#include "util/base/exception.h"
#include "gui/base/opengl/opengl_gui_graphics.h"
#include "gui/base/sdl/sdl_gui_graphics.h"
#include "gui/base/gui_image.h"
#include "gui/base/gui_imageloader.h"

#include "fife_unit_test.h"

using boost::unit_test::test_suite;
using namespace FIFE;

static const std::string IMAGE_FILE = "../data/beach_e1.png";
static const std::string SUBIMAGE_FILE = "../data/rpg_tiles_01.png";

// Environment
struct environment {
	boost::shared_ptr<TimeManager> timemanager;

	environment()
		: timemanager(new TimeManager()) {
			if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
				throw SDLException(SDL_GetError());
			}
		}
};

void test_gui_image(RenderBackend& renderbackend, gcn::Graphics& graphics, ImagePool& pool) {
	boost::scoped_ptr<VFS> vfs(new VFS());
	vfs->addSource(new VFSDirectory(vfs.get()));

	pool.addResourceLoader(new SubImageLoader());
	pool.addResourceLoader(new ImageLoader(vfs.get()));

	GuiImageLoader imageloader(pool);
	gcn::Image::setImageLoader(&imageloader);	

	gcn::Container* top = new gcn::Container();
	top->setDimension(gcn::Rectangle(0, 0, 200, 300));
	gcn::Gui* gui = new gcn::Gui();
	gui->setGraphics(&graphics);
	gui->setTop(top);
	gcn::Label* label = new gcn::Label("Label");
;	gcn::Image* guiimage = gcn::Image::load(IMAGE_FILE);
	gcn::Icon* icon = new gcn::Icon(guiimage);
 
	top->add(label, 10, 10);
	top->add(icon, 10, 30);

	ImageLoader provider(vfs.get());
	boost::scoped_ptr<Image> img(provider.load(ImageLocation(IMAGE_FILE)));
	
	int h = img->getHeight();
	int w = img->getWidth();
	for (int i = 0; i < 100; i+=2) {
		renderbackend.startFrame();
		img->render(Rect(i, i, w, h));
		gui->logic();
		gui->draw();
		renderbackend.endFrame();
	}	
}

#ifdef FIFE_BOOST_VERSION_103300
void test_sdl_gui_image() {
#else
BOOST_AUTO_TEST_CASE( SDL_gui_test ) {
#endif
	environment env;
	RenderBackendSDL renderbackend;
	renderbackend.init();
	ImagePool pool;
	Image* screen = renderbackend.createMainScreen(800, 600, 0, false);
	SdlGuiGraphics graphics(pool);
	graphics.setTarget(screen->getSurface());
	test_gui_image(renderbackend, graphics, pool);
}


#ifdef FIFE_BOOST_VERSION_103300
void test_ogl_gui_image() {
#else
BOOST_AUTO_TEST_CASE( OGL_gui_test ) {
#endif
	environment env;
	RenderBackendOpenGL renderbackend;
	renderbackend.init();
	ImagePool pool;
	renderbackend.createMainScreen(800, 600, 0, false);
	OpenGLGuiGraphics graphics(pool);
	test_gui_image(renderbackend, graphics, pool);
}

#ifdef FIFE_BOOST_VERSION_103300
test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("Gui Tests");
	test->add( BOOST_TEST_CASE( &test_ogl_gui_image ),0 );
	test->add( BOOST_TEST_CASE( &test_sdl_gui_image ),0 );

	return test;
}
#endif
