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

#ifndef FIFE_VIDEO_OPENGL_BUFFEROBJECT_H
#define FIFE_VIDEO_OPENGL_BUFFEROBJECT_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/point.h"
#include "video/color.h"

#include "fife_opengl.h"

namespace FIFE {

	/** Abstract interface for all the opengl buffer objects. */
	class OpenGLBufferObject {
	public:
		/** Destructor
		 */
		virtual ~OpenGLBufferObject() {}

		/** Render
		 */
		virtual void render() = 0;

	};

	// for regular primitives with color and alpha
	struct renderDataP {
		GLfloat vertex[2];
		GLubyte color[4];
	};


	class OpenGLBufferLinesObject : public OpenGLBufferObject {
	public:
		OpenGLBufferLinesObject();
		virtual ~OpenGLBufferLinesObject();
		void add(const std::vector<Point>& points, const Color& color);
		void update(uint32_t position, const std::vector<Point>& points, const Color& color);
		void clear();
		void finalize();
		void render();
	private:
		bool m_final;
		uint32_t m_oldFinalSize;
		GLuint m_vboId;
		GLuint m_vboIndicesId;
		std::vector<renderDataP> m_data;
		std::vector<uint32_t> m_indices;
	};
}

#endif
