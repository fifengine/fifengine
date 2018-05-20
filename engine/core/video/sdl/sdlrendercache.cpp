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

#include "sdlrendercache.h"

namespace FIFE {
	SDLRenderCache::SDLRenderCache() {
	}

	SDLRenderCache::~SDLRenderCache() {
		clear();
	}

	void SDLRenderCache::clear() {
		std::vector<SDLBufferObject*>::iterator it = m_objects.begin();
		for (; it != m_objects.end(); it++) {
			delete *it;
		}
		m_objects.clear();
		m_typeBuffers.clear();
	}

	void SDLRenderCache::render() {
		std::vector<SDLBufferObject*>::iterator it = m_objects.begin();
		for (; it != m_objects.end(); it++) {
			(*it)->render();
		}
	}

	void SDLRenderCache::addLine(const Point& p1, const Point& p2, const Color& color) {
		std::vector<Point> points(2);
		points[0] = p1;
		points[1] = p2;
		SDLBufferObject* buffer = m_typeBuffers[PrimitiveBufferObject];
		if (!buffer) {
			buffer = new SDLBufferPrimitiveObject(RenderType::Render_Line, points, color);
			m_typeBuffers[PrimitiveBufferObject] = buffer;
			m_objects.push_back(buffer);
		} else {
			static_cast<SDLBufferPrimitiveObject*>(buffer)->add(RenderType::Render_Line, points, color);
		}
	}

	void SDLRenderCache::addLines(const std::vector<Point>& points, const Color& color) {
		SDLBufferObject* buffer = m_typeBuffers[PrimitiveBufferObject];
		if (!buffer) {
			buffer = new SDLBufferPrimitiveObject(FIFE::RenderType::Render_Lines, points, color);
			m_typeBuffers[PrimitiveBufferObject] = buffer;
			m_objects.push_back(buffer);
		} else {
			static_cast<SDLBufferPrimitiveObject*>(buffer)->add(RenderType::Render_Lines, points, color);
		}
	}

	void SDLRenderCache::updateLines(uint32_t position, const std::vector<Point>& points, const Color& color) {
		SDLBufferObject* buffer = m_typeBuffers[PrimitiveBufferObject];
		if (!buffer) {
			return;
		}
		static_cast<SDLBufferPrimitiveObject*>(buffer)->update(position, points, color);
	}

	void SDLRenderCache::removeLines(uint32_t position, uint32_t elements) {
		SDLBufferObject* buffer = m_typeBuffers[PrimitiveBufferObject];
		if (!buffer) {
			return;
		}
		static_cast<SDLBufferPrimitiveObject*>(buffer)->remove(position, elements);
	}
}
