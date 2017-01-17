/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

#ifndef FIFE_VIDEO_ATLASBOOK_H
#define FIFE_VIDEO_ATLASBOOK_H

// Standard C++ library includes
#include <vector>
#include <cassert>
#include <cmath>
#include <algorithm>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"

namespace FIFE {

	class AtlasBlock {
	public:
		uint32_t page;
		uint32_t left, right, top, bottom;

		AtlasBlock(const Rect& rect, uint32_t page)
			: page(page),
			left(rect.x), right(rect.right()),
			top(rect.y), bottom(rect.bottom()){
		}

		AtlasBlock() {
		}

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

		void setTrivial() {
			left = right = top = bottom = 0;
		}

		bool isTrivial() const {
			return getWidth() == 0 || getHeight() == 0;
		}

		uint32_t getWidth() const { return right - left; }
		uint32_t getHeight() const { return bottom - top; }

		AtlasBlock intersects(AtlasBlock const& rect) const;
		void merge(AtlasBlock const& rect);
	};

	class AtlasPage {
	public:
		AtlasPage(uint32_t width, uint32_t height,
			uint32_t pixelSize, uint32_t page)
			: width(width), height(height), pixelSize(pixelSize),
			page(page), freePixels(width*height*pixelSize){
		}

		AtlasBlock* getBlock(uint32_t width, uint32_t height);
		void shrink(bool pot);

		uint32_t getWidth() const {
			return width;
		}

		uint32_t getHeight() const {
			return height;
		}

	private:
		AtlasBlock const* intersects(AtlasBlock const* block) const;

		uint32_t width, height;
		uint32_t pixelSize;
		uint32_t page;
		int32_t freePixels;

		typedef std::vector<AtlasBlock> Blocks;
		Blocks blocks;
	};

	class AtlasBook {
	public:

		AtlasBook(uint32_t pageWidth, uint32_t pageHeight,
			uint32_t pixelSize = 4)
			: pageWidth(pageWidth), pageHeight(pageHeight),
			pixelSize(pixelSize) {
		}

		AtlasBlock* getBlock(uint32_t width, uint32_t height);

		// try to shrink every atlas page
		void shrink(bool pot);

		AtlasPage& getPage(size_t index) {
			return pages[index];
		}

	private:
		// add new atlas to atlas container
		AtlasPage* extendCache(uint32_t minPageWidth, uint32_t minPageHeight);

		// How big the new atlases should be
		uint32_t pageWidth, pageHeight;
		uint32_t pixelSize;

		typedef std::vector<AtlasPage> Pages;
		Pages pages;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
