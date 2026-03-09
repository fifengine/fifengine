// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "util/utf8/utf8.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"

#include "subimagefont.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_GUI);

    SubImageFont::SubImageFont(const std::string& filename, const std::string& glyphs) : ImageFontBase()
    {

        FL_LOG(_log, LMsg("fifechan_image_font, loading ") << filename << " glyphs " << glyphs);

        ImagePtr img         = ImageManager::instance()->load(filename);
        SDL_Surface* surface = img->getSurface();
        m_colorkey           = RenderBackend::instance()->getColorKey();

        if (surface == nullptr) {
            throw CannotOpenFile(filename);
        }

        // Make sure we get 32bit RGB
        // and copy the Pixelbuffers surface
        SDL_Surface* tmp = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, RMASK, GMASK, BMASK, NULLMASK);

        SDL_BlitSurface(surface, nullptr, tmp, nullptr);
        surface = tmp;

        // Prepare the data for extracting the glyphs.
        uint32_t* pixels = reinterpret_cast<uint32_t*>(surface->pixels);

        int32_t x = 0;

        SDL_Rect src;

        src.h = surface->h;
        src.y = 0;

        uint32_t separator = pixels[0];
        uint32_t colorkey  = SDL_MapRGB(surface->format, m_colorkey.r, m_colorkey.g, m_colorkey.b);

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

        FL_DBG(
            _log,
            LMsg("image_font") << " glyph separator is " << pprint(reinterpret_cast<void*>(separator))
                               << " transparent color is " << pprint(reinterpret_cast<void*>(colorkey)));

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

            tmp = SDL_CreateRGBSurface(0, w, surface->h, 32, RMASK, GMASK, BMASK, NULLMASK);

            SDL_FillRect(tmp, nullptr, colorkey);
            SDL_BlitSurface(surface, &src, tmp, nullptr);

            // Disable alpha blending, so that we use colorkeying
            // SDL_SetAlpha(tmp,0,255);
            // SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,colorkey);
            SDL_SetSurfaceBlendMode(tmp, SDL_BLENDMODE_NONE);
            SDL_SetColorKey(tmp, SDL_TRUE, colorkey);

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
        SDL_FreeSurface(surface);
    }

} // namespace FIFE