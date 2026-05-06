// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "atlasbook.h"

// Standard C++ library includes
#include <algorithm>
#include <limits>
#include <utility>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"

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

    AtlasBlock* AtlasPage::getBlock(uint32_t blockWidth, uint32_t blockHeight)
    {
        if (std::cmp_greater(blockWidth * blockHeight * pixelSize, freePixels)) {
            return nullptr;
        }

        blocks.emplace_back(Rect(), 0);
        AtlasBlock* newBlock = &blocks[blocks.size() - 1];

        for (uint32_t v = 0; (v + 1) * blockHeight <= this->height; ++v) {
            newBlock->top    = v * blockHeight;
            newBlock->bottom = (v + 1) * blockHeight;

            for (uint32_t u = 0; (u + 1) * blockWidth <= this->width; ++u) {

                newBlock->left  = u * blockWidth;
                newBlock->right = (u + 1) * blockWidth;

                AtlasBlock const * intersection = intersects(newBlock);
                if (intersection == nullptr) {
                    freePixels -= blockWidth * blockHeight * pixelSize;
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
                            uint32_t const squeezedWidth = newBlock->getWidth();

                            // binary search
                            for (int i = 0, div = 2; i < 4; ++i) {
                                squeezed.left -= squeezedWidth / div;
                                squeezed.right -= squeezedWidth / div;

                                intersection = intersects(&squeezed);
                                if (intersection != nullptr) {
                                    squeezed.left += squeezedWidth / div;
                                    squeezed.right += squeezedWidth / div;
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
                            uint32_t const squeezedHeight = newBlock->getHeight();

                            // binary search
                            for (int i = 0, div = 2; i < 4; ++i) {
                                squeezed.top -= squeezedHeight / div;
                                squeezed.bottom -= squeezedHeight / div;

                                intersection = intersects(&squeezed);
                                if (intersection != nullptr) {
                                    squeezed.top += squeezedHeight / div;
                                    squeezed.bottom += squeezedHeight / div;
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
            uint32_t const bwidth  = boundaryBox.getWidth();
            uint32_t const bheight = boundaryBox.getHeight();

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
