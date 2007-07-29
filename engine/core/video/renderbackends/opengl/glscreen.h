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

#ifndef FIFE_VIDEO_RENDERBACKENDS_OPENGL_GLSCREEN_H
#define FIFE_VIDEO_RENDERBACKENDS_OPENGL_GLSCREEN_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/screen.h"

namespace FIFE {

	/** OpenGL Screen surface.
	 *
	 * Implements the main render target for the OpenGL backend.
	 *
	 * @see Screen
	 * @see RenderBackendOpenGL
	 */
	class GLScreen : public Screen {
		public:
			/** Creates a new GL screen.
			 */
			GLScreen(SDL_Surface* surface);
			
			/** Destroys this GL screen.
			 */
			virtual ~GLScreen();

			/** Returns the screen width.
			 *
			 * @return Width of the screen.
			 */
			virtual unsigned int getWidth() const;
			
			/** Returns the screen height.
			 *
			 * @return Height of the screen.
			 */
			virtual unsigned int getHeight() const;

			/** Returns a pointer to the contained SDL surface.
			 *
			 * @return A pointer to the SDL surface as set in the constructor.
			 */
			virtual SDL_Surface* getSurface() const;


		private:
			// Screen.
			SDL_Surface* m_screen;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
