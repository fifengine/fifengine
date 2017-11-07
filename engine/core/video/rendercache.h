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

#ifndef FIFE_VIDEO_RENDERCACHE_H
#define FIFE_VIDEO_RENDERCACHE_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "renderbackend.h"

namespace FIFE {

	 /** Abstract interface for all the rendercaches. */
	class RenderCache {
	public:
		/** Constructor.
		 */
		RenderCache() {};

		/** Destructor.
		 */
		virtual ~RenderCache() {};

		virtual void clear() = 0;

		/** Render 
		 */
		virtual void render() = 0;

		virtual void addLine(const Point& p1, const Point& p2, const Color& color) = 0;

		virtual void addLines(const std::vector<Point>& points, const Color& color) = 0;

		virtual void updateLines(uint32_t position, const std::vector<Point>& points, const Color& color) = 0;
	};
}

#endif
