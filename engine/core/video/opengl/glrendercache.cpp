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
#include "renderbackendopengl.h"
#include "glbufferobject.h"
#include "glrendercache.h"

#define BUFFER_OFFSET(i) ((void*)(i))

namespace FIFE {
	GLRenderCache::GLRenderCache():
		RenderCache() {

		m_buffer = NULL;
	}

	GLRenderCache::~GLRenderCache() {
		delete m_buffer;
	}

	void GLRenderCache::clear() {
		if (m_buffer) {
			static_cast<OpenGLBufferLinesObject*>(m_buffer)->clear();
		}

	}

	void GLRenderCache::render() {
		if (m_buffer) {
			m_buffer->render();
		}
	}

	void GLRenderCache::addLine(const Point& p1, const Point& p2, const Color& color) {
	}

	void GLRenderCache::addLines(const std::vector<Point>& points, const Color& color) {
		if (points.size() < 2) {
			return;
		}
		if (!m_buffer) {
			m_buffer = new OpenGLBufferLinesObject();
		}
		static_cast<OpenGLBufferLinesObject*>(m_buffer)->add(points, color);
	}

	void GLRenderCache::updateLines(uint32_t position, const std::vector<Point>& points, const Color& color) {
		if (points.size() < 2) {
			return;
		}
		if (!m_buffer) {
			return;
		}
		static_cast<OpenGLBufferLinesObject*>(m_buffer)->update(position, points, color);
	}
}
