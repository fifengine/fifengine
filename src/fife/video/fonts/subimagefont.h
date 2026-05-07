// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_SUBIMAGEFONT_H
#define FIFE_FONTS_SUBIMAGEFONT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
#include "imagefontbase.h"

namespace FIFE
{

    /** Imagefont that is able to read glyphs from single image sheet, see e.g.
     *  Fifechan imagefontcollection (google for it)
     */
    class FIFE_API SubImageFont : public ImageFontBase
    {
        public:
            /**
             * Constructor.
             *
             * @param filename the filename of the Image Font.
             * @param glyphs the glyphs for Fifechan format.
             */
            SubImageFont(std::string const & filename, std::string const & glyphs);
            ~SubImageFont() override = default;

        private:
            SDL_Color m_colorkey;
    };

} // namespace FIFE

#endif
