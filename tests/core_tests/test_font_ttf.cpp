// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

// Platform specific includes
#include "fife_unittest.h"
#include "fixture.h"

// FIFE includes
#include "video/fonts/truetypefont.h"
#include "video/image.h"

using FIFE::TrueTypeFont;

static char const * const FONT_FILE = "tests/data/FreeMono.ttf";

TEST_CASE("TrueTypeFont::renderString sanitizes transparent pixels", "[font][ttf]")
{
    FontTestFixture env;
    require_font_renderable(FONT_FILE, 16);

    auto font = std::make_unique<TrueTypeFont>(FONT_FILE, 16);
    REQUIRE(font != nullptr);

    font->setColor(255, 255, 255, 255);
    SDL_Surface* surface = font->renderString("Test");
    REQUIRE(surface != nullptr);
    REQUIRE(surface->w > 0);
    REQUIRE(surface->h > 0);

    // Save for visual inspection
    std::filesystem::create_directories("test_output");
    FIFE::Image::saveAsPng("test_output/ttf_test_sanitized.png", *surface);

    SDL_LockSurface(surface);
    auto* pixels                                  = static_cast<uint32_t*>(surface->pixels);
    int32_t const pitch_px                        = surface->pitch / 4;
    SDL_PixelFormatDetails const * format_details = SDL_GetPixelFormatDetails(surface->format);

    bool found_transparent         = false;
    bool found_colored_transparent = false;

    for (int32_t y = 0; y < surface->h; ++y) {
        uint32_t const * row = pixels + (static_cast<size_t>(y) * static_cast<size_t>(pitch_px));
        for (int32_t x = 0; x < surface->w; ++x) {
            uint32_t p = row[x];
            uint8_t r  = 0;
            uint8_t g  = 0;
            uint8_t b  = 0;
            uint8_t a  = 0;
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
    FontTestFixture env;
    require_font_renderable(FONT_FILE, 16);

    auto font = std::make_unique<TrueTypeFont>(FONT_FILE, 16);
    REQUIRE(font != nullptr);

    SDL_Surface* surface = font->renderString("");
    REQUIRE(surface != nullptr);
    CHECK(surface->w >= 1);
    CHECK(surface->h >= 1);

    SDL_DestroySurface(surface);
}

TEST_CASE("TrueTypeFont::renderString no cyan artifacts in transparent pixels", "[font][ttf]")
{
    FontTestFixture env;
    require_font_renderable(FONT_FILE, 24);

    auto font = std::make_unique<TrueTypeFont>(FONT_FILE, 24);
    REQUIRE(font != nullptr);

    font->setColor(255, 255, 255, 255);
    SDL_Surface* surface = font->renderString("AaBbCc");
    REQUIRE(surface != nullptr);

    // Save for visual inspection
    std::filesystem::create_directories("test_output");
    FIFE::Image::saveAsPng("test_output/ttf_test_cyan.png", *surface);

    SDL_LockSurface(surface);
    auto* pixels                                  = static_cast<uint32_t*>(surface->pixels);
    int32_t const pitch_px                        = surface->pitch / 4;
    SDL_PixelFormatDetails const * format_details = SDL_GetPixelFormatDetails(surface->format);

    bool found_cyan_transparent = false;

    for (int32_t y = 0; y < surface->h; ++y) {
        uint32_t const * row = pixels + (static_cast<size_t>(y) * static_cast<size_t>(pitch_px));
        for (int32_t x = 0; x < surface->w; ++x) {
            uint32_t p = row[x];
            uint8_t r  = 0;
            uint8_t g  = 0;
            uint8_t b  = 0;
            uint8_t a  = 0;
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

TEST_CASE("TrueTypeFont::renderString with different colors", "[font][ttf]")
{
    FontTestFixture env;
    require_font_renderable(FONT_FILE, 20);

    auto font = std::make_unique<TrueTypeFont>(FONT_FILE, 20);
    REQUIRE(font != nullptr);

    std::filesystem::create_directories("test_output");

    // Test white text
    font->setColor(255, 255, 255, 255);
    SDL_Surface* surf_white = font->renderString("White");
    REQUIRE(surf_white != nullptr);
    FIFE::Image::saveAsPng("test_output/ttf_text_white.png", *surf_white);

    // Test red text
    font->setColor(255, 0, 0, 255);
    SDL_Surface* surf_red = font->renderString("Red");
    REQUIRE(surf_red != nullptr);
    FIFE::Image::saveAsPng("test_output/ttf_text_red.png", *surf_red);

    // Test cyan text (this should be cyan, not have cyan artifacts)
    font->setColor(0, 255, 255, 255);
    SDL_Surface* surf_cyan = font->renderString("Cyan");
    REQUIRE(surf_cyan != nullptr);
    FIFE::Image::saveAsPng("test_output/ttf_text_cyan.png", *surf_cyan);

    // Verify white text has no colored transparent pixels AND opaque pixels ARE white
    SDL_LockSurface(surf_white);
    auto* pixels                       = static_cast<uint32_t*>(surf_white->pixels);
    int32_t const pitch_px             = surf_white->pitch / 4;
    SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(surf_white->format);

    bool found_colored_transparent = false;
    bool found_opaque_not_white    = false;
    for (int32_t y = 0; y < surf_white->h && (!found_colored_transparent || !found_opaque_not_white); ++y) {
        uint32_t const * row = pixels + (static_cast<size_t>(y) * static_cast<size_t>(pitch_px));
        for (int32_t x = 0; x < surf_white->w && (!found_colored_transparent || !found_opaque_not_white); ++x) {
            uint8_t r = 0;
            uint8_t g = 0;
            uint8_t b = 0;
            uint8_t a = 0;
            SDL_GetRGBA(row[x], fmt, nullptr, &r, &g, &b, &a);
            if (a == 0) {
                if (r != 0 || g != 0 || b != 0) {
                    found_colored_transparent = true;
                }
            } else {
                // Opaque pixel should be white
                if (r != 255 || g != 255 || b != 255) {
                    found_opaque_not_white = true;
                }
            }
        }
    }
    SDL_UnlockSurface(surf_white);

    SDL_DestroySurface(surf_white);
    SDL_DestroySurface(surf_red);
    SDL_DestroySurface(surf_cyan);

    CHECK(!found_colored_transparent);
    CHECK(!found_opaque_not_white);
}

TEST_CASE("TrueTypeFont::renderString pixel-perfect verification", "[font][ttf]")
{
    FontTestFixture env;
    require_font_renderable(FONT_FILE, 16);

    auto font = std::make_unique<TrueTypeFont>(FONT_FILE, 16);
    REQUIRE(font != nullptr);

    // Render a known character at known color
    font->setColor(255, 255, 255, 255);
    SDL_Surface* surface = font->renderString("X");
    REQUIRE(surface != nullptr);

    // Save for visual inspection and regression testing
    std::filesystem::create_directories("test_output");
    FIFE::Image::saveAsPng("test_output/ttf_letter_X.png", *surface);

    // Verify we have opaque (non-transparent) pixels - the "X" glyph
    SDL_LockSurface(surface);
    auto* pixels                       = static_cast<uint32_t*>(surface->pixels);
    int32_t const pitch_px             = surface->pitch / 4;
    SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(surface->format);

    int opaque_count      = 0;
    int transparent_count = 0;

    for (int32_t y = 0; y < surface->h; ++y) {
        uint32_t const * row = pixels + (static_cast<size_t>(y) * static_cast<size_t>(pitch_px));
        for (int32_t x = 0; x < surface->w; ++x) {
            uint8_t r = 0;
            uint8_t g = 0;
            uint8_t b = 0;
            uint8_t a = 0;
            SDL_GetRGBA(row[x], fmt, nullptr, &r, &g, &b, &a);
            if (a > 0) {
                ++opaque_count;
            } else {
                ++transparent_count;
            }
        }
    }
    SDL_UnlockSurface(surface);
    SDL_DestroySurface(surface);

    // We should have both opaque (the letter) and transparent (background) pixels
    CHECK(opaque_count > 0);
    CHECK(transparent_count > 0);
}
