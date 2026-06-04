// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_FONTS_TEXTLAYOUT_H
#define FIFE_VIDEO_FONTS_TEXTLAYOUT_H

#include <cstdint>
#include <memory>
#include <vector>

#include "fontface.h"
#include "fonttypes.h"
#include "util/structures/point.h"

namespace FIFE
{

    struct GlyphRun
    {
            std::vector<uint32_t> glyphIndices;
            std::vector<Point> positions;
            std::shared_ptr<FontFace> face;
            int startX = 0;
            int startY = 0;
    };

    struct TextLayout
    {
            std::vector<GlyphRun> runs;
            int width  = 0;
            int height = 0;

            bool isEmpty() const
            {
                return runs.empty();
            }
            void clear();
            void addRun(GlyphRun run);
    };

} // namespace FIFE

#endif
