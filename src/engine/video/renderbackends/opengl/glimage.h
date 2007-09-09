/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef FIFEGLIMAGE_H
#define FIFEGLIMAGE_H

#include "video/image.h"
#include <stdint.h>
#include <vector>
#include <GL/gl.h>

namespace FIFE {

	/** Implements an Image using OpenGL.
	 *
	 * This class contains a texture handle bound to the data given to the constructor;
	 * it draws a textured \i Quad to the screen.
	 *
	 * @see Image
	 * @see RenderBackendOpenGL
	 *
	 * \note Width and height are not limited to powers of two; non-power of two
	 * images will be converted internally.
	 */
	class GLImage : public Image {
		public:
			/** Construct an instance from RGBA pixel data.
			 *
			 * @param data RGBA (unsigned byte) data matching the next params
			 * @param width Width of the image
			 * @param height Height of the image
			 */
			GLImage(const uint8_t* rgbadata, unsigned int width, unsigned int height);
			/** Destructor.
			 *
			 * Deletes the GL resources allocated by this instance.
			 */
			virtual ~GLImage();

			/** Draw this image into the specified clip-rect of the given screen.
			 *
			 * @param rect Destination clip-rectangle
			 * @param screen Screen to draw to
			 * @param alpha  Alpha-value for this frame; default is fully opaque.
			 */
			virtual void render(const Rect& rect, Screen* screen, unsigned char alpha = 255);
			/** Returns the width of this image.
			 * @return Width
			 */
			virtual unsigned int getWidth() const;
			/** Returns the height of this image.
			 * @return Height of this image.
			 */
			virtual unsigned int getHeight() const;

		private:
			unsigned int m_width;
			unsigned int m_height;

			unsigned int m_rows;
			unsigned int m_cols;

			float m_tex_x;
			float m_tex_y;

			unsigned int m_last_col_width;
			unsigned int m_last_row_height;

			GLuint *m_textureid;

			void cleanup();
			void generateTexture(const uint8_t* data, unsigned int width, unsigned int height);

	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
