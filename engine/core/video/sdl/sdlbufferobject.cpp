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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "sdlbufferobject.h"

namespace FIFE {
	SDLBufferLineObject::SDLBufferLineObject(const Point& p1, const Point& p2, const Color& color) {
		m_p1 = p1;
		m_p2 = p2;
		m_color = color;
	}

	SDLBufferLineObject::~SDLBufferLineObject() {
	}

	void SDLBufferLineObject::render() {
		SDL_Renderer* renderer = static_cast<RenderBackendSDL*>(RenderBackend::instance())->getRenderer();
		SDL_SetRenderDrawColor(renderer, m_color.getR(), m_color.getG(), m_color.getB(), m_color.getAlpha());
		SDL_RenderDrawLine(renderer, m_p1.x, m_p1.y, m_p2.x, m_p2.y);
	}


	SDLBufferLinesObject::SDLBufferLinesObject(const std::vector<Point>& points, const Color& color) {
		m_count = points.size();
		m_color = color;
		m_points = new SDL_Point[m_count];
		uint32_t index = 0;
		for (; index < m_count; index++) {
			m_points[index].x = points[index].x;
			m_points[index].y = points[index].y;
		}
	}

	SDLBufferLinesObject::~SDLBufferLinesObject() {
		delete[] m_points;
	}

	void SDLBufferLinesObject::add(const std::vector<Point>& points, const Color& color) {
		if (m_count != points.size()) {
			delete[] m_points;
			m_count = points.size();
			m_points = new SDL_Point[m_count];
		}
		m_color = color;
		uint32_t index = 0;
		for (; index < m_count; index++) {
			m_points[index].x = points[index].x;
			m_points[index].y = points[index].y;
		}
	}

	void SDLBufferLinesObject::render() {
		SDL_Renderer* renderer = static_cast<RenderBackendSDL*>(RenderBackend::instance())->getRenderer();
		SDL_SetRenderDrawColor(renderer, m_color.getR(), m_color.getG(), m_color.getB(), m_color.getAlpha());
		SDL_RenderDrawLines(renderer, m_points, m_count);
	}
}
