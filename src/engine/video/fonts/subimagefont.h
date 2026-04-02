// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_SUBIMAGEFONT_H
#define FIFE_FONTS_SUBIMAGEFONT_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "imagefontbase.h"

namespace FIFE
{

    /** Imagefont that is able to read glyphs from single image sheet, see e.g.
     *  Fifechan imagefontcollection (google for it)
     */
    class SubImageFont : public ImageFontBase
    {
    public:
        /**
         * Constructor.
         *
         * @param filename the filename of the Image Font.
         * @param glyphs the glyphs for Fifechan format.
         */
        SubImageFont(const std::string& filename, const std::string& glyphs);
        ~SubImageFont() override = default;

    private:
        SDL_Color m_colorkey;
    };

} // namespace FIFE

#endif
