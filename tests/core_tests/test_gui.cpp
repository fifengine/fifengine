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

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <SDL.h>
#include <fifechan.hpp>

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

using namespace FIFE;

static const std::string IMAGE_FILE = "tests/data/beach_e1.png";
static const std::string SUBIMAGE_FILE = "tests/data/rpg_tiles_01.png";
struct environment {
	boost::shared_ptr<TimeManager> timemanager;

	environment()
		: timemanager(new TimeManager()) {
			if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
				throw SDLException(SDL_GetError());
			}
		}
};

void test_gui_image(RenderBackend& renderbackend, fcn::Graphics& graphics, ImagePool& pool) {
		boost::scoped_ptr<VFS> vfs(new VFS());
		vfs->addSource(new VFSDirectory(vfs.get()));

		pool.addResourceLoader(new SubImageLoader());
		pool.addResourceLoader(new ImageLoader(vfs.get()));

		GuiImageLoader imageloader(pool);
		fcn::Image::setImageLoader(&imageloader);	


		fcn::Container* top = new fcn::Container();
		top->setDimension(fcn::Rectangle(0, 0, 200, 300));
		fcn::Gui* gui = new fcn::Gui();
		gui->setGraphics(&graphics);
		gui->setTop(top);
		fcn::Label* label = new fcn::Label("Label");

		fcn::Image* guiimage = fcn::Image::load(IMAGE_FILE);
		fcn::Icon* icon = new fcn::Icon(guiimage);
	 
		top->add(label, 10, 10);
		top->add(icon, 10, 30);

		ImageLoader provider(vfs.get());
		boost::scoped_ptr<Image> img(dynamic_cast<Image*>(provider.loadResource(ImageLocation(IMAGE_FILE))));

		int h = img->getHeight();
		int w = img->getWidth();
		for (int i = 0; i < 100; i+=2) {
			renderbackend.startFrame();
			img->render(Rect(i, i, w, h));
			gui->logic();
			gui->draw();
			renderbackend.endFrame();
		}
		delete label;
		delete icon;
		delete guiimage;
	}


TEST(test_sdl_gui_image)
{
	environment env;
	RenderBackendSDL renderbackend;
	renderbackend.init();
	ImagePool pool;
	Image* screen = renderbackend.createMainScreen(800, 600, 0, false, "FIFE", "");
	SdlGuiGraphics graphics(pool);
	graphics.setTarget(screen->getSurface());
	test_gui_image(renderbackend, graphics, pool);
}

TEST(test_ogl_gui_image)
{
	environment env;
	RenderBackendOpenGL renderbackend;
	renderbackend.init();
	ImagePool pool;
	renderbackend.createMainScreen(800, 600, 0, false, "FIFE", "");
	OpenGLGuiGraphics graphics(pool);
	test_gui_image(renderbackend, graphics, pool);
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
