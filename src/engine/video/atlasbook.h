// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_ATLASBOOK_H
#define FIFE_VIDEO_ATLASBOOK_H

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"

namespace FIFE
{

    class AtlasBlock
    {
    public:
        uint32_t page;
        uint32_t left, right, top, bottom;

        AtlasBlock(const Rect& rect, uint32_t page) :
            page(page),
            left(static_cast<uint32_t>(rect.x)),
            right(static_cast<uint32_t>(rect.right())),
            top(static_cast<uint32_t>(rect.y)),
            bottom(static_cast<uint32_t>(rect.bottom()))
        {
            assert(rect.x >= 0);
            assert(rect.y >= 0);
            assert(rect.right() >= 0);
            assert(rect.bottom() >= 0);
        }

        AtlasBlock() = default;

        //	      (0,0) [left]   [right]
        //	         +--------------------
        // 	         |
        //	 [top]   |    +---------+
        //	         |    |         |
        //	         |    |         |
        //	         |    |         |
        //	         |    |         |
        //	[bottom] |    +---------+
        //	         |

        // bottom > top
        // right > left

        void setTrivial()
        {
            left = right = top = bottom = 0;
        }
        bool isTrivial() const
        {
            return getWidth() == 0 || getHeight() == 0;
        }

        uint32_t getWidth() const
        {
            return right - left;
        }
        uint32_t getHeight() const
        {
            return bottom - top;
        }

        AtlasBlock intersects(AtlasBlock const & rect) const;
        void merge(AtlasBlock const & rect);
    };

    class AtlasPage
    {
    public:
        AtlasPage(uint32_t width, uint32_t height, uint32_t pixelSize, uint32_t page) :
            width(width),
            height(height),
            pixelSize(pixelSize),
            page(page),
            freePixels(static_cast<int32_t>(width * height * pixelSize))
        {
            assert((width * height * pixelSize) <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
        }

        AtlasBlock* getBlock(uint32_t blockWidth, uint32_t blockHeight);
        void shrink(bool pot);

        uint32_t getWidth() const
        {
            return width;
        }

        uint32_t getHeight() const
        {
            return height;
        }

    private:
        AtlasBlock const * intersects(AtlasBlock const * block) const;

        uint32_t width, height;
        uint32_t pixelSize;
        uint32_t page;
        int32_t freePixels;

        using Blocks = std::vector<AtlasBlock>;
        Blocks blocks;
    };

    class AtlasBook
    {
    public:
        AtlasBook(uint32_t pageWidth, uint32_t pageHeight, uint32_t pixelSize = 4) :
            pageWidth(pageWidth), pageHeight(pageHeight), pixelSize(pixelSize)
        {
        }

        AtlasBlock* getBlock(uint32_t width, uint32_t height);

        // try to shrink every atlas page
        void shrink(bool pot);

        AtlasPage& getPage(size_t index)
        {
            return pages[index];
        }

    private:
        // add new atlas to atlas container
        AtlasPage* extendCache(uint32_t minPageWidth, uint32_t minPageHeight);

        // How big the new atlases should be
        uint32_t pageWidth, pageHeight;
        uint32_t pixelSize;

        using Pages = std::vector<AtlasPage>;
        Pages pages;
    };
} // namespace FIFE

#endif
