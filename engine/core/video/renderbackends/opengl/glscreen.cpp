/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "fife_opengl.h"
#include "glscreen.h"


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


	GLScreen::GLScreen(SDL_Surface* surface): m_screen(surface) {
	}

	GLScreen::~GLScreen() {
	}

	unsigned int GLScreen::getWidth() const {
		return SDL_GetVideoSurface()->w;
	}

	unsigned int GLScreen::getHeight() const {
		return SDL_GetVideoSurface()->h;
	}

	SDL_Surface* GLScreen::getSurface() const {
		return m_screen;
	}

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
