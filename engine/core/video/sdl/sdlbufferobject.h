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

#ifndef FIFE_VIDEO_SDL_BUFFEROBJECT_H
#define FIFE_VIDEO_SDL_BUFFEROBJECT_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "renderbackendsdl.h"

namespace FIFE {
	/** Abstract interface for all the sdl buffer objects. */
	class SDLBufferObject {
	public:

		/** Destructor
		 */
		virtual ~SDLBufferObject() {}

		/** Render
		 */
		virtual void render() = 0;
	};


	class SDLBufferLineObject : public SDLBufferObject {
	public:
		SDLBufferLineObject(const Point& p1, const Point& p2, const Color& color);
		virtual ~SDLBufferLineObject();
		void render();
	private:
		Point m_p1;
		Point m_p2;
		Color m_color;
	};

	class SDLBufferLinesObject : public SDLBufferObject {
	public:
		SDLBufferLinesObject(const std::vector<Point>& points, const Color& color);
		virtual ~SDLBufferLinesObject();
		void add(const std::vector<Point>& points, const Color& color);
		void render();
	private:
		SDL_Point* m_points;
		Color m_color;
		int32_t m_count;
	};
}

#endif
