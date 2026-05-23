// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "textlayout.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace FIFE
{

    void TextLayout::clear()
    {
        runs.clear();
        width  = 0;
        height = 0;
    }

    void TextLayout::addRun(GlyphRun run)
    {
        if (!run.face) {
            throw std::invalid_argument("GlyphRun face must not be null");
        }
        assert("Glyph indices and positions must match" && run.glyphIndices.size() == run.positions.size());

        int runMaxX = 0;
        int runMaxY = 0;

        for (auto const & pos : run.positions) {
            runMaxX = std::max(runMaxX, pos.x + run.startX);
            runMaxY = std::max(runMaxY, pos.y + run.startY);
        }

        width  = std::max(width, runMaxX);
        height = std::max(height, runMaxY);

        runs.push_back(std::move(run));
    }

} // namespace FIFE
