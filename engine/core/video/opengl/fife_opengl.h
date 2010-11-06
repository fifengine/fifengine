/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_UTIL_FIFEGL_H
#define FIFE_UTIL_FIFEGL_H

// Standard C++ library includes

// Platform specific includes
// Linux
#if defined( __unix__ )
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// Win32
#if defined( WIN32 )
// MSVC
#if defined( _MSC_VER )
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#undef DELETE
// MinGW
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif

// Macintosh
#if defined( __APPLE_CC__ )
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#endif

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	struct GLEnable {
		GLenum m_flag;
		GLboolean m_oldval;
		GLEnable(GLenum flag) : m_flag(flag) {
			glGetBooleanv(flag, &m_oldval);
			if (!m_oldval) {
				glEnable(flag);
			}
		}
		~GLEnable() {
			if (!m_oldval) {
				glDisable(m_flag);
			}
		}
	};

	struct GLDisable {
		GLenum m_flag;
		GLboolean m_oldval;
		GLDisable(GLenum flag) : m_flag(flag) {
			glGetBooleanv(flag, &m_oldval);
			if (m_oldval) {
				glDisable(flag);
			}
		}
		~GLDisable() {
			if (m_oldval) {
				glEnable(m_flag);
			}
		}
	};


} //FIFE

#endif
