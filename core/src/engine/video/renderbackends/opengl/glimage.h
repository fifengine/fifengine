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

#ifndef FIFE_VIDEO_RENDERBACKENDS_OPENGL_GLIMAGE_H
#define FIFE_VIDEO_RENDERBACKENDS_OPENGL_GLIMAGE_H

// Standard C++ library includes
#include <vector>

// Platform specific includes
#include "util/fife_stdint.h"
#include "util/fife_opengl.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"

namespace FIFE {

	/** Implements an Image using OpenGL.
	 *
	 * This class contains a texture handle bound to the data given to the constructor;
	 * it draws a textured @i Quad to the screen.
	 *
	 * @see Image
	 * @see RenderBackendOpenGL
	 * @note Width and height are not limited to powers of two; non-power of two
	 * images will be converted internally.
	 * @todo Check the correctness of the generateTexture function on big endian systems (ppc)
	 */
	class GLImage : public Image {
		public:
			/** Constructs an instance from a SDL Surface
			 *
			 * @note Takes ownership of the SDL Surface
			 * @param surface SDL Surface in RGBA format
			 */
			GLImage(SDL_Surface* surface);

			/** Destructor.
			 *
			 * Deletes the GL resources allocated by this instance.
			 */
			virtual ~GLImage();

			/** Draw this image into the specified clip-rect of the given screen.
			 *
			 * @param rect Destination clip-rectangle.
			 * @param screen Screen to draw to.
			 * @param alpha  Alpha-value for this frame; default is fully opaque.
			 */
			virtual void render(const Rect& rect, Screen* screen, unsigned char alpha = 255);
			/** Returns the width of this image.
			 *
			 * @return Width of the image.
			 */
			virtual unsigned int getWidth() const;
			/** Returns the height of this image.
			 *
			 * @return Height of the image.
			 */
			virtual unsigned int getHeight() const;

		private:
			// Width of the image.
			unsigned int m_width;
			// Height of the image.
			unsigned int m_height;

			unsigned int m_rows;
			unsigned int m_cols;

			float m_tex_x;
			float m_tex_y;

			unsigned int m_last_col_width;
			unsigned int m_last_row_height;

			GLuint *m_textureid;

			void cleanup();
			void generateTexture();

	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
