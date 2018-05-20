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


namespace FIFE {

	OpenGLBufferLinesObject::OpenGLBufferLinesObject() {
		m_final = false;
		m_vboId = 0;
		m_vboIndicesId = 0;
		m_oldFinalSize = 0;
	}

	OpenGLBufferLinesObject::~OpenGLBufferLinesObject() {
		glDeleteBuffers(1, &m_vboId);
		glDeleteBuffers(1, &m_vboIndicesId);
	}

	void OpenGLBufferLinesObject::add(const std::vector<Point>& points, const Color& color) {
		if (points.size() < 2) {
			return;
		}
		m_final = false;
		std::vector<Point>::const_iterator it = points.begin();
		renderDataP rd;
		rd.vertex[0] = static_cast<float>((*it).x) + 0.375;
		rd.vertex[1] = static_cast<float>((*it).y) + 0.375;
		rd.color[0] = color.getR();
		rd.color[1] = color.getG();
		rd.color[2] = color.getB();
		rd.color[3] = color.getAlpha();
		m_data.push_back(rd);
		++it;
		uint32_t indice = m_indices.empty() ? 0 : m_indices.back() + 1;
		for (; it != points.end(); ++it) {
			rd.vertex[0] = static_cast<float>((*it).x) + 0.375;
			rd.vertex[1] = static_cast<float>((*it).y) + 0.375;
			m_data.push_back(rd);
			m_indices.push_back(indice++);
			m_indices.push_back(indice);
		}
	}

	void OpenGLBufferLinesObject::clear() {
		m_data.clear();
		m_indices.clear();
		/*if (m_vboId != 0) {
			glDeleteBuffers(1, &m_vboId);
			glDeleteBuffers(1, &m_vboIndicesId);
			m_vboId = 0;
			m_vboIndicesId = 0;
		}*/
		m_final = false;
	}

	void OpenGLBufferLinesObject::update(uint32_t position, const std::vector<Point>& points, const Color& color) {
		uint32_t elements = points.size();
		for (uint32_t i = 0; i < elements; ++i) {
			renderDataP& rd = m_data[position + i];
			rd.vertex[0] = static_cast<float>(points[i].x) + 0.375;
			rd.vertex[1] = static_cast<float>(points[i].y) + 0.375;
		}

		if (!m_final) {
			finalize();
		} else {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(renderDataP) * position, sizeof(renderDataP) * elements, &m_data[position].vertex);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	void OpenGLBufferLinesObject::finalize() {
		if (m_vboId == 0) {
			glGenBuffers(1, &m_vboId);
			glGenBuffers(1, &m_vboIndicesId);
		}

		if (m_oldFinalSize != m_indices.size()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
			glBufferData(GL_ARRAY_BUFFER, sizeof(renderDataP) * m_data.size(), &m_data[0].vertex, GL_STREAM_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndicesId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_indices.size(), &m_indices[0], GL_STREAM_DRAW);
			m_oldFinalSize = m_indices.size();
		} else {
			glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
			glBufferData(GL_ARRAY_BUFFER, sizeof(renderDataP) * m_data.size(), NULL, GL_STREAM_DRAW);
			GLvoid* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			memcpy(ptr, &m_data[0].vertex, sizeof(renderDataP) * m_data.size());
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		m_final = true;
	}

	void OpenGLBufferLinesObject::render() {
		if (m_data.empty()) {
			return;
		}
		if (!m_final) {
			finalize();
		}
		static_cast<RenderBackendOpenGL*>(RenderBackend::instance())->renderVboBuffers(m_indices.size(), m_vboId, m_vboIndicesId);
	}
}
