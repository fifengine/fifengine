/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#pragma once

#include <vector>
#include <cassert>
#include <cmath>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

struct Rect
{
	uint32_t left, right, top, bottom;

	Rect(uint32_t l, uint32_t r, uint32_t t, uint32_t b)
		: left(l), right(r), top(t), bottom(b) {;}
	Rect()
		: left(0), right(0), top(0), bottom(0) {;}

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

	uint32_t getWidth() const { return right - left; }
	uint32_t getHeight() const { return bottom - top; }

	Rect intersects(Rect const& rect) const
	{
		Rect ret;

		ret.left = std::max(rect.left, left);
		ret.right = std::min(rect.right, right);
		ret.top = std::max(rect.top, top);
		ret.bottom = std::min(rect.bottom, bottom);

		if(ret.left > ret.right || ret.top > ret.bottom)
			ret.setTrivial();

		return ret;
	}

	void merge(Rect const& rect)
	{
		left = std::min(rect.left, left);
		right = std::max(rect.right, right);
		top = std::min(rect.top, top);
		bottom = std::max(rect.bottom, bottom);
	}
};

class AtlasBlock : public Rect
{
public:
	uint32_t page;

	AtlasBlock(const Rect& rect, uint32_t page)
		: Rect(rect), page(page)
	{;}
	AtlasBlock()
		: Rect(), page(0)
	{;}
};

class AtlasPage
{
public:
	uint32_t width, height;
	uint32_t pixelSize;
	uint32_t page;
	int32_t freePixels;

	typedef std::vector<AtlasBlock> Blocks;
	Blocks blocks;

	AtlasPage(uint32_t width, uint32_t height,
		uint32_t pixelSize, uint32_t page)
		: width(width), height(height), pixelSize(pixelSize),
		page(page), freePixels(width*height*pixelSize)
	{;}

	AtlasBlock* getBlock(uint32_t width, uint32_t height)
	{
		if(static_cast<int32_t>(width*height*pixelSize) > freePixels)
			return 0;

		blocks.push_back(AtlasBlock(Rect(), 0));
		AtlasBlock* newBlock = &blocks[blocks.size() - 1];

		for(uint32_t v = 0; (v+1)*height <= this->height; ++v)
		{
			newBlock->top = v * height;
			newBlock->bottom = (v+1) * height;

			for(uint32_t u = 0; (u+1)*width <= this->width; ++u)
			{

				newBlock->left = u * width;
				newBlock->right = (u+1) * width;

				AtlasBlock const* intersection = intersects(newBlock);
				if(!intersection)
				{
					freePixels -= width*height*pixelSize;
					assert(freePixels >= 0);

					// try to squeeze a little bit (horizontal)
					if(newBlock->left > 0)
					{
						AtlasBlock squeezed(*newBlock);

						--squeezed.left;
						--squeezed.right;

						if(!(intersection = intersects(&squeezed)))
						{
							++squeezed.left;
							++squeezed.right;
							int blockWidth = newBlock->getWidth();

							// binary search
							for(int i = 0, div = 2; i < 4; ++i)
							{
								squeezed.left -= blockWidth / div;
								squeezed.right -= blockWidth / div;

								if((intersection = intersects(&squeezed)))
								{
									squeezed.left += blockWidth / div;
									squeezed.right += blockWidth / div;
								}
								div <<= 1;
							}

							// linear search
							while(!(intersection = intersects(&squeezed)) && squeezed.left > 0)
							{
								--squeezed.left;
								--squeezed.right;
							}

							newBlock->left = squeezed.left + 1;
							newBlock->right = squeezed.right + 1;
						}
					}

					// try to squeeze a little bit (vertical)
					if(newBlock->top > 0)
					{
						AtlasBlock squeezed(*newBlock);

						--squeezed.top;
						--squeezed.bottom;

						if(!(intersection = intersects(&squeezed)))
						{
							++squeezed.top;
							++squeezed.bottom;
							int blockHeight = newBlock->getHeight();

							// binary search
							for(int i = 0, div = 2; i < 4; ++i)
							{
								squeezed.top -= blockHeight / div;
								squeezed.bottom -= blockHeight / div;

								if((intersection = intersects(&squeezed)))
								{
									squeezed.top += blockHeight / div;
									squeezed.bottom += blockHeight / div;
								}
								div <<= 1;
							}

							// linear search
							while(!(intersection = intersects(&squeezed)) && squeezed.top > 0)
							{
								--squeezed.top;
								--squeezed.bottom;
							}

							newBlock->top = squeezed.top + 1;
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
		return 0;
	}

	void shrink(bool pot)
	{
		Rect boundaryBox;
		for(Blocks::iterator block = blocks.begin(); block != blocks.end(); ++block)
			boundaryBox.merge(*block);

		assert(boundaryBox.left == 0);
		assert(boundaryBox.top == 0);

		if(pot)
		{
			uint32_t bwidth = boundaryBox.getWidth();
			uint32_t bheight = boundaryBox.getHeight();

			if(bwidth < width)
			{
				// look for next power of 2 for width
				uint32_t powof2 = 1;
				while(powof2 < bwidth) powof2 <<= 1;
				width = std::min(powof2, width);
			}

			if(bheight < height)
			{
				// look for next power of 2 for width
				uint32_t powof2 = 1;
				while(powof2 < bheight) powof2 <<= 1;
				height = std::min(powof2, height);
			}

		}
		else
		{
			width = boundaryBox.getWidth();
			height = boundaryBox.getHeight();
		}
	}

private:

	AtlasBlock const* intersects(AtlasBlock const* block) const
	{
		for(size_t b = 0; b < blocks.size() - 1; ++b)
		{
			if(!blocks[b].intersects(*block).isTrivial())
				return block;
		}
		// no intersection
		return 0;
	}
};

class AtlasBook
{
public:
	// How big the new atlases should be
	uint32_t pageWidth, pageHeight;
	uint32_t pixelSize;

	typedef std::vector<AtlasPage> Pages;
	Pages pages;

	AtlasBook(uint32_t pageWidth, uint32_t pageHeight,
		uint32_t pixelSize)
		: pageWidth(pageWidth), pageHeight(pageHeight),
		pixelSize(pixelSize)
	{;}

	AtlasBlock* getBlock(uint32_t width, uint32_t height)
	{
		for(Pages::iterator page = pages.begin(); page != pages.end(); ++page)
		{
			AtlasBlock* block = page->getBlock(width, height);
			if(block)
				return block;
		}
		return extendCache(width, height)->getBlock(width, height);
	}

	// add new atlas to atlas container
	AtlasPage* extendCache(uint32_t minPageWidth, uint32_t minPageHeight)
	{
		if(minPageWidth > pageWidth ||
		   minPageHeight > pageHeight)
		{
			throw std::string("Texture is too big for this atlas.");
		}

		assert(minPageWidth <= pageWidth);
		assert(minPageHeight <= pageHeight);

		pages.push_back(AtlasPage(pageWidth, pageHeight, pixelSize, pages.size()));
		return &pages[pages.size()-1];
	}

	// try to shrink every atlas page
	void shrink(bool pot)
	{
		for(Pages::iterator page = pages.begin(); page != pages.end(); ++page)
			page->shrink(pot);
	}
};
