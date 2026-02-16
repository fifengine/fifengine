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
#include <cstdint>
#include <memory>
#include <string>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes
#include <SDL.h>
#include <fifechan.hpp>

#include "gui/fifechan/base/gui_image.h"
#include "gui/fifechan/base/gui_imageloader.h"
#include "gui/fifechan/base/opengl/opengl_gui_graphics.h"
#include "gui/fifechan/base/sdl/sdl_gui_graphics.h"
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "util/time/timemanager.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "video/devicecaps.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/sdl/renderbackendsdl.h"

using FIFE::Image;
using FIFE::ImageManager;
using FIFE::ImagePtr;
using FIFE::Rect;
using FIFE::RenderBackend;
using FIFE::RenderBackendOpenGL;
using FIFE::RenderBackendSDL;
using FIFE::ScreenMode;
using FIFE::SDLException;
using FIFE::TimeManager;
using FIFE::VFS;
using FIFE::VFSDirectory;

static const std::string IMAGE_FILE    = "tests/data/beach_e1.png";
static const std::string SUBIMAGE_FILE = "tests/data/rpg_tiles_01.png";
struct environment
{
    std::shared_ptr<TimeManager> timemanager;
    std::shared_ptr<VFS> vfs;
    std::shared_ptr<ImageManager> imageManager;

    environment() :
        timemanager(std::make_shared<TimeManager>()),
        vfs(std::make_shared<VFS>()),
        imageManager(std::make_shared<ImageManager>())
    {
        vfs->addSource(new VFSDirectory(vfs.get()));
        if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {
            throw SDLException(SDL_GetError());
        }
    }
};

void test_gui_image(RenderBackend& renderbackend, fcn::Graphics& graphics)
{
    GuiImageLoader imageloader;
    fcn::Image::setImageLoader(&imageloader);

    fcn::Container* top = new fcn::Container();
    top->setDimension(fcn::Rectangle(0, 0, 200, 300));
    fcn::Gui* gui = new fcn::Gui();
    gui->setGraphics(&graphics);
    gui->setTop(top);
    fcn::Label* label = new fcn::Label("Label");

    fcn::Image* guiimage = fcn::Image::load(IMAGE_FILE);
    fcn::Icon* icon      = new fcn::Icon(guiimage);

    top->add(label, 10, 10);
    top->add(icon, 10, 30);

    ImagePtr img = ImageManager::instance()->load(IMAGE_FILE);
    REQUIRE(img);

    int h = img->getHeight();
    int w = img->getWidth();
    for (int i = 0; i < 100; i += 2) {
        renderbackend.startFrame();
        img->render(Rect(i, i, w, h));
        gui->logic();
        gui->draw();
        renderbackend.endFrame();
    }
}

TEST_CASE("test_sdl_gui_image")
{
    environment env;
    RenderBackendSDL renderbackend(SDL_Color{0, 0, 0, 0});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_SDL), "FIFE", "");
    SdlGuiGraphics graphics;
    graphics.updateTarget();
    test_gui_image(renderbackend, graphics);
}

TEST_CASE("test_ogl_gui_image")
{
    environment env;
    RenderBackendOpenGL renderbackend(SDL_Color{0, 0, 0, 0});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_OPENGL), "FIFE", "");
    OpenGLGuiGraphics graphics;
    graphics.updateTarget();
    test_gui_image(renderbackend, graphics);
}
