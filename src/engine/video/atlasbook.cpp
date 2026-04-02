// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <limits>
#include <utility>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "atlasbook.h"

namespace FIFE
{
    AtlasBlock AtlasBlock::intersects(AtlasBlock const & rect) const
    {
        AtlasBlock ret{};

        ret.left   = std::max(rect.left, left);
        ret.right  = std::min(rect.right, right);
        ret.top    = std::max(rect.top, top);
        ret.bottom = std::min(rect.bottom, bottom);

        if (ret.left > ret.right || ret.top > ret.bottom) {
            ret.setTrivial();
        }

        return ret;
    }

    void AtlasBlock::merge(AtlasBlock const & rect)
    {
        left   = std::min(rect.left, left);
        right  = std::max(rect.right, right);
        top    = std::min(rect.top, top);
        bottom = std::max(rect.bottom, bottom);
    }

    AtlasBlock* AtlasPage::getBlock(uint32_t width, uint32_t height)
    {
        if (std::cmp_greater(width * height * pixelSize, freePixels)) {
            return nullptr;
        }

        blocks.emplace_back(Rect(), 0);
        AtlasBlock* newBlock = &blocks[blocks.size() - 1];

        for (uint32_t v = 0; (v + 1) * height <= this->height; ++v) {
            newBlock->top    = v * height;
            newBlock->bottom = (v + 1) * height;

            for (uint32_t u = 0; (u + 1) * width <= this->width; ++u) {

                newBlock->left  = u * width;
                newBlock->right = (u + 1) * width;

                AtlasBlock const * intersection = intersects(newBlock);
                if (intersection == nullptr) {
                    freePixels -= width * height * pixelSize;
                    assert(freePixels >= 0);

                    // try to squeeze a little bit (horizontal)
                    if (newBlock->left > 0) {
                        AtlasBlock squeezed(*newBlock);

                        --squeezed.left;
                        --squeezed.right;

                        intersection = intersects(&squeezed);
                        if (intersection == nullptr) {
                            ++squeezed.left;
                            ++squeezed.right;
                            const uint32_t blockWidth = newBlock->getWidth();

                            // binary search
                            for (int i = 0, div = 2; i < 4; ++i) {
                                squeezed.left -= blockWidth / div;
                                squeezed.right -= blockWidth / div;

                                intersection = intersects(&squeezed);
                                if (intersection != nullptr) {
                                    squeezed.left += blockWidth / div;
                                    squeezed.right += blockWidth / div;
                                }
                                div <<= 1;
                            }

                            // linear search
                            intersection = intersects(&squeezed);
                            while ((intersection == nullptr) && squeezed.left > 0) {
                                --squeezed.left;
                                --squeezed.right;
                            }

                            newBlock->left  = squeezed.left + 1;
                            newBlock->right = squeezed.right + 1;
                        }
                    }

                    // try to squeeze a little bit (vertical)
                    if (newBlock->top > 0) {
                        AtlasBlock squeezed(*newBlock);

                        --squeezed.top;
                        --squeezed.bottom;

                        intersection = intersects(&squeezed);
                        if (intersection == nullptr) {
                            ++squeezed.top;
                            ++squeezed.bottom;
                            const uint32_t blockHeight = newBlock->getHeight();

                            // binary search
                            for (int i = 0, div = 2; i < 4; ++i) {
                                squeezed.top -= blockHeight / div;
                                squeezed.bottom -= blockHeight / div;

                                intersection = intersects(&squeezed);
                                if (intersection != nullptr) {
                                    squeezed.top += blockHeight / div;
                                    squeezed.bottom += blockHeight / div;
                                }
                                div <<= 1;
                            }

                            // linear search
                            intersection = intersects(&squeezed);
                            while ((intersection == nullptr) && squeezed.top > 0) {
                                --squeezed.top;
                                --squeezed.bottom;
                            }

                            newBlock->top    = squeezed.top + 1;
                            newBlock->bottom = squeezed.bottom + 1;
                        }
                    }

                    newBlock->page = this->page;
                    return newBlock;
                }
            }
        }
        // couldn't find suitable place for a new block
        blocks.pop_back();
        return nullptr;
    }

    void AtlasPage::shrink(bool pot)
    {
        AtlasBlock boundaryBox(Rect(), 0);
        for (auto& block : blocks) {
            boundaryBox.merge(block);
        }

        assert(boundaryBox.left == 0);
        assert(boundaryBox.top == 0);

        if (pot) {
            const uint32_t bwidth  = boundaryBox.getWidth();
            const uint32_t bheight = boundaryBox.getHeight();

            if (bwidth < width) {
                // look for next power of 2 for width
                uint32_t powof2 = 1;
                while (powof2 < bwidth) {
                    powof2 <<= 1;
                }
                width = std::min(powof2, width);
            }

            if (bheight < height) {
                // look for next power of 2 for width
                uint32_t powof2 = 1;
                while (powof2 < bheight) {
                    powof2 <<= 1;
                }
                height = std::min(powof2, height);
            }

        } else {
            width  = boundaryBox.getWidth();
            height = boundaryBox.getHeight();
        }
    }

    AtlasBlock const * AtlasPage::intersects(AtlasBlock const * block) const
    {
        for (size_t b = 0; b < blocks.size() - 1; ++b) {
            if (!blocks[b].intersects(*block).isTrivial()) {
                return block;
            }
        }
        // no intersection
        return nullptr;
    }

    AtlasBlock* AtlasBook::getBlock(uint32_t width, uint32_t height)
    {
        for (auto& page : pages) {
            AtlasBlock* block = page.getBlock(width, height);
            if (block != nullptr) {
                return block;
            }
        }
        return extendCache(width, height)->getBlock(width, height);
    }

    AtlasPage* AtlasBook::extendCache(uint32_t minPageWidth, uint32_t minPageHeight)
    {
        if (minPageWidth > pageWidth || minPageHeight > pageHeight) {
            throw Exception("Texture is too big for this atlas.");
        }

        assert(minPageWidth <= pageWidth);
        assert(minPageHeight <= pageHeight);

        assert(pages.size() <= static_cast<size_t>(std::numeric_limits<uint32_t>::max()));
        pages.emplace_back(pageWidth, pageHeight, pixelSize, static_cast<uint32_t>(pages.size()));
        return &pages[pages.size() - 1];
    }

    void AtlasBook::shrink(bool pot)
    {
        for (auto& page : pages) {
            page.shrink(pot);
        }
    }
} // namespace FIFE
