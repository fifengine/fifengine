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
	SDLBufferPrimitiveObject::SDLBufferPrimitiveObject(RenderType type, const std::vector<Point>& points, const Color& color) {
		add(type, points, color);
	}

	SDLBufferPrimitiveObject::~SDLBufferPrimitiveObject() {

	}

	void SDLBufferPrimitiveObject::add(RenderType type, const std::vector<Point>& points, const Color& color) {
		uint32_t index = m_points.size();
		uint32_t elements = points.size();
		BufferDescriptor bc = { type, index, elements, color };

		m_points.resize(index + elements);
		for (uint32_t i = 0; i < elements; ++i) {
			m_points[index + i].x = points[i].x;
			m_points[index + i].y = points[i].y;
		}

		m_descriptors.push_back(bc);
		m_descriptorPosition[index] = m_descriptors.size() - 1;
	}

	void SDLBufferPrimitiveObject::update(uint32_t position, const std::vector<Point>& points, const Color& color) {
		BufferDescriptor& bd = m_descriptors[m_descriptorPosition[position]];
		bd.color = color;
		uint32_t index = bd.position;
		uint32_t elements = points.size();
		for (uint32_t i = 0; i < elements; ++i) {
			m_points[index + i].x = points[i].x;
			m_points[index + i].y = points[i].y;
		}
	}

	void SDLBufferPrimitiveObject::remove(uint32_t position, uint32_t elements) {
		BufferDescriptor& bd = m_descriptors[m_descriptorPosition[position]];
		bd.count = 0;
	}

	void SDLBufferPrimitiveObject::render() {
		if (m_descriptors.empty()) return;
		SDL_Point* point = NULL;
		SDL_Renderer* renderer = static_cast<RenderBackendSDL*>(RenderBackend::instance())->getRenderer();
		std::vector<BufferDescriptor>::iterator it = m_descriptors.begin();
		for (; it != m_descriptors.end(); ++it) {
			const BufferDescriptor& bd = *it;
			SDL_SetRenderDrawColor(renderer, bd.color.getR(), bd.color.getG(), bd.color.getB(), bd.color.getAlpha());
			point = &m_points[bd.position];
			switch (bd.type) {
				case Render_Point:
				case Render_Points:
					SDL_RenderDrawPoints(renderer, point, bd.count);
					break;
				case Render_Line:
				case Render_Lines:
					SDL_RenderDrawLines(renderer, point, bd.count);
					break;
			}
		}
	}
}
