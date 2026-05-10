// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <cstdint>
#include <memory>
#include <string>

#include "fife_unittest.h"
#include "util/base/exception.h"
#include "video/fonts/truetypefont.h"
#include "video/image.h"
#include "video/renderbackend.h"

using FIFE::Image;
using FIFE::RenderBackend;
using FIFE::SDLException;
using FIFE::TrueTypeFont;

static std::string const FONT_FILE = "demos/rio_de_hola/fonts/FreeSans.ttf";

struct FontEnvironment
{
        SDL_Surface* surface = nullptr;

        FontEnvironment()
        {
            if (!SDL_Init(SDL_INIT_VIDEO)) {
                throw SDLException(SDL_GetError());
            }
            if (TTF_Init() != 0) {
                throw SDLException(SDL_GetError());
            }
        }

        ~FontEnvironment()
        {
            TTF_Quit();
            SDL_Quit();
        }
};

TEST_CASE("TrueTypeFont::renderString sanitizes transparent pixels", "[font][ttf]")
{
    FontEnvironment env;

    std::unique_ptr<TrueTypeFont> font(new TrueTypeFont(FONT_FILE, 16));
    REQUIRE(font != nullptr);

    SDL_Surface* surface = font->renderString("Test");
    REQUIRE(surface != nullptr);
    REQUIRE(surface->w > 0);
    REQUIRE(surface->h > 0);

    SDL_LockSurface(surface);
    auto* pixels                                  = static_cast<uint32_t*>(surface->pixels);
    int32_t const pitch_px                        = surface->pitch / 4;
    SDL_PixelFormatDetails const * format_details = SDL_GetPixelFormatDetails(surface->format);

    bool found_transparent         = false;
    bool found_colored_transparent = false;

    for (int32_t y = 0; y < surface->h; ++y) {
        uint32_t* row = pixels + static_cast<size_t>(y) * static_cast<size_t>(pitch_px);
        for (int32_t x = 0; x < surface->w; ++x) {
            uint32_t p = row[x];
            uint8_t r, g, b, a;
            SDL_GetRGBA(p, format_details, nullptr, &r, &g, &b, &a);

            if (a == 0) {
                found_transparent = true;
                if (r != 0 || g != 0 || b != 0) {
                    found_colored_transparent = true;
                }
            }
        }
    }
    SDL_UnlockSurface(surface);
    SDL_DestroySurface(surface);

    CHECK(found_transparent);
    CHECK(!found_colored_transparent);
}

TEST_CASE("TrueTypeFont::renderString handles empty string", "[font][ttf]")
{
    FontEnvironment env;

    std::unique_ptr<TrueTypeFont> font(new TrueTypeFont(FONT_FILE, 16));
    REQUIRE(font != nullptr);

    SDL_Surface* surface = font->renderString("");
    REQUIRE(surface != nullptr);
    CHECK(surface->w >= 1);
    CHECK(surface->h >= 1);

    SDL_DestroySurface(surface);
}

TEST_CASE("TrueTypeFont::renderString no cyan artifacts in transparent pixels", "[font][ttf]")
{
    FontEnvironment env;

    std::unique_ptr<TrueTypeFont> font(new TrueTypeFont(FONT_FILE, 24));
    REQUIRE(font != nullptr);

    SDL_Surface* surface = font->renderString("AaBbCc");
    REQUIRE(surface != nullptr);

    SDL_LockSurface(surface);
    auto* pixels                                  = static_cast<uint32_t*>(surface->pixels);
    int32_t const pitch_px                        = surface->pitch / 4;
    SDL_PixelFormatDetails const * format_details = SDL_GetPixelFormatDetails(surface->format);

    bool found_cyan_transparent = false;

    for (int32_t y = 0; y < surface->h; ++y) {
        uint32_t* row = pixels + static_cast<size_t>(y) * static_cast<size_t>(pitch_px);
        for (int32_t x = 0; x < surface->w; ++x) {
            uint32_t p = row[x];
            uint8_t r, g, b, a;
            SDL_GetRGBA(p, format_details, nullptr, &r, &g, &b, &a);

            if (a == 0 && r == 0 && g == 255 && b == 255) {
                found_cyan_transparent = true;
            }
        }
    }
    SDL_UnlockSurface(surface);
    SDL_DestroySurface(surface);

    CHECK(!found_cyan_transparent);
}
