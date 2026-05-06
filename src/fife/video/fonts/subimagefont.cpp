// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "subimagefont.h"

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes
#include <SDL3/SDL.h>

#include <utf8cpp/utf8.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"

namespace FIFE
{

    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_GUI);

    SubImageFont::SubImageFont(std::string const & filename, std::string const & glyphs) : m_colorkey{0, 0, 0, 0}
    {

        FL_LOG(_log, LMsg("fifechan_image_font, loading ") << filename << " glyphs " << glyphs);

        ImagePtr const img   = ImageManager::instance()->load(filename);
        SDL_Surface* surface = img->getSurface();
        m_colorkey           = RenderBackend::instance()->getColorKey();

        if (surface == nullptr) {
            throw CannotOpenFile(filename);
        }

        // Make sure we get 32bit RGB
        // and copy the Pixelbuffers surface
        SDL_Surface* tmp = SDL_CreateSurface(surface->w, surface->h, SDL_PIXELFORMAT_XRGB8888);

        SDL_BlitSurface(surface, nullptr, tmp, nullptr);
        surface = tmp;

        // Prepare the data for extracting the glyphs.
        auto* pixels = static_cast<uint32_t*>(surface->pixels);

        int32_t x = 0;

        SDL_Rect src;

        src.h = surface->h;
        src.y = 0;

        uint32_t const separator               = pixels[0];
        SDL_PixelFormatDetails const * details = SDL_GetPixelFormatDetails(surface->format);
        uint32_t colorkey =
            SDL_MapRGB(details, SDL_GetSurfacePalette(surface), m_colorkey.r, m_colorkey.g, m_colorkey.b);

        // if colorkey feature is not enabled then manually find the color key in the font
        if (!RenderBackend::instance()->isColorKeyEnabled()) {
            while (x < surface->w && pixels[x] == separator) {
                ++x;
            }

            colorkey = pixels[x];
        }

        // Disable alpha blending, so that we use color keying
        // SDL_SetAlpha(surface,0,255);
        // SDL_SetColorKey(surface,SDL_SRCCOLORKEY,colorkey);

        FL_DBG(_log, LMsg("image_font") << " glyph separator is " << separator << " transparent color is " << colorkey);

        // Finally extract all glyphs
        std::string::const_iterator text_it = glyphs.begin();
        while (text_it != glyphs.end()) {
            int32_t w = 0;
            while (x < surface->w && pixels[x] == separator) {
                ++x;
            }
            if (x == surface->w) {
                break;
            }

            while (x + w < surface->w && pixels[x + w] != separator) {
                ++w;
            }

            src.x = x;
            src.w = w;

            tmp = SDL_CreateSurface(w, surface->h, SDL_PIXELFORMAT_XRGB8888);

            SDL_FillSurfaceRect(tmp, nullptr, colorkey);
            SDL_BlitSurface(surface, &src, tmp, nullptr);

            // Disable alpha blending, so that we use colorkeying
            // SDL_SetAlpha(tmp,0,255);
            // SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,colorkey);
            SDL_SetSurfaceBlendMode(tmp, SDL_BLENDMODE_NONE);
            SDL_SetSurfaceColorKey(tmp, true, colorkey);

            uint32_t codepoint          = utf8::next(text_it, glyphs.end());
            m_glyphs[codepoint].surface = tmp;

            x += w;
        }

        // Set placeholder glyph
        // This should actually work with utf8.
        if (m_glyphs.contains('?')) {
            m_placeholder = m_glyphs['?'];
        } else {
            m_placeholder.surface = nullptr;
        }

        m_height = surface->h;
        SDL_DestroySurface(surface);
    }

} // namespace FIFE
