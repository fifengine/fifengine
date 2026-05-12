// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_TEST_FIXTURE_H
#define FIFE_TEST_FIXTURE_H

// Standard C++ library includes
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

// 3rd party library includes
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "util/base/exception.h"
#include "util/time/timemanager.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "video/imagemanager.h"

struct TestFixture
{
        std::shared_ptr<FIFE::TimeManager> tm;
        std::shared_ptr<FIFE::VFS> vfs;
        std::shared_ptr<FIFE::ImageManager> img;

        TestFixture()
        {
            tm  = std::make_shared<FIFE::TimeManager>();
            vfs = std::make_shared<FIFE::VFS>();
            vfs->addSource(new FIFE::VFSDirectory(vfs.get()));
            img = std::make_shared<FIFE::ImageManager>();
        }
};

struct FontTestFixture : TestFixture
{
        FontTestFixture() : TestFixture()
        {
            static bool const ttf_init = []() {
                bool sdl_ok = SDL_Init(SDL_INIT_VIDEO);
                if (!sdl_ok) {
                    throw FIFE::SDLException(std::string("SDL_Init failed: ") + SDL_GetError());
                }
                if (!TTF_Init()) {
                    std::string err = SDL_GetError();
                    if (err.empty()) {
                        err = "TTF_Init returned error with no SDL error message";
                    }
                    throw FIFE::SDLException(err);
                }
                return true;
            }();
            (void)ttf_init;
        }
};

inline void fontFileExists(std::string const & path)
{
    std::ifstream f(path);
    if (!f.good()) {
        throw std::runtime_error("Font file not found: " + path);
    }
    f.close();
}

inline void require_font_renderable(std::string const & path, int size)
{
    fontFileExists(path);
    TTF_Font* font = TTF_OpenFont(path.c_str(), static_cast<float>(size));
    if (!font) {
        throw std::runtime_error(std::string("TTF_OpenFont failed for ") + path + ": " + SDL_GetError());
    }
    SDL_Color color      = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, "Test", 4, color);
    if (!surface) {
        TTF_CloseFont(font);
        throw std::runtime_error(std::string("TTF_RenderText_Blended failed for ") + path + ": " + SDL_GetError());
    }
    SDL_DestroySurface(surface);
    TTF_CloseFont(font);
}

#endif
