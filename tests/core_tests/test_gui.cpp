// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <fifechan.hpp>

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>

#include "fife_unittest.h"
#include "fixture.h"
#include "gui/fifechan/base/gui_image.h"
#include "gui/fifechan/base/gui_imageloader.h"
#include "gui/fifechan/base/opengl/opengl_gui_graphics.h"
#include "gui/fifechan/base/sdl/sdl_gui_graphics.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/sdl/renderbackendsdl.h"

using FIFE::GuiImageLoader;
using FIFE::Image;
using FIFE::ImageManager;
using FIFE::ImagePtr;
using FIFE::OpenGLGuiGraphics;
using FIFE::Rect;
using FIFE::RenderBackend;
using FIFE::RenderBackendOpenGL;
using FIFE::RenderBackendSDL;
using FIFE::ScreenMode;
using FIFE::SdlGuiGraphics;

static char const * const IMAGE_FILE    = "tests/data/beach_e1.png";
static char const * const SUBIMAGE_FILE = "tests/data/rpg_tiles_01.png";
struct environment : TestFixture
{
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

    fcn::Image const * guiimage = fcn::Image::load(IMAGE_FILE);
    fcn::Icon* icon             = new fcn::Icon(guiimage);

    top->add(label, 10, 10);
    top->add(icon, 10, 30);

    ImagePtr img = ImageManager::instance()->load(IMAGE_FILE);
    REQUIRE(img);

    int h = static_cast<int>(img->getHeight());
    int w = static_cast<int>(img->getWidth());
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
    RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 0});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_SDL), "FIFE", "");
    SdlGuiGraphics graphics;
    graphics.updateTarget();
    test_gui_image(renderbackend, graphics);
}

TEST_CASE("test_ogl_gui_image")
{
    environment env;
    RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 0});
    renderbackend.init("");
    renderbackend.createMainScreen(ScreenMode(800, 600, 32, ScreenMode::WINDOWED_OPENGL), "FIFE", "");
    OpenGLGuiGraphics graphics;
    graphics.updateTarget();
    test_gui_image(renderbackend, graphics);
}
