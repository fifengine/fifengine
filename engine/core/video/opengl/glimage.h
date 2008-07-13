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

#ifndef FIFE_VIDEO_RENDERBACKENDS_OPENGL_GLIMAGE_H
#define FIFE_VIDEO_RENDERBACKENDS_OPENGL_GLIMAGE_H

// Standard C++ library includes
#include <vector>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"

#include "fife_opengl.h"

namespace FIFE {
	// FIXME: due to image chunking issues, GLImage uses SDLImage to draw primitives on its surface
	// remember though that OpenGL backend is not separate thing of SDL; instead it sits on top of it
	class SDLImage;


	/** Implements an Image using OpenGL.
	 *
	 * This class contains a texture handle bound to the data given to the constructor;
	 * it draws a textured @i Quad to the screen.
	 *
	 * @see Image
	 * @note Width and height are not limited to powers of two; non-power of two
	 * images will be converted internally.
	 * @todo Check the correctness of the generateTexture function on big endian systems (ppc)
	 */
	class GLImage : public Image {
	public:
		GLImage(SDL_Surface* surface);
		GLImage(const uint8_t* data, unsigned int width, unsigned int height);
		virtual ~GLImage();
		void render(const Rect& rect, SDL_Surface* dst, unsigned char alpha = 255);
		void saveImage(const std::string& filename);
 		bool putPixel(int x, int y, int r, int g, int b);
		void drawLine(const Point& p1, const Point& p2, int r, int g, int b);
		void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b);

	protected:
		void setClipArea(const Rect& cliparea, bool clear);

	private:
		// number of rows into which this image is sliced, so that it "becomes power of 2 compatible"
		unsigned int m_rows;
		// see m_rows
		unsigned int m_cols;

		// ratio of texture fill in last column. E.g. in case image width = 300, chunk = 256x256,
		// last column chunk width = 64 -> ratio is (300-256) / 64 = 0.6875
		// this means that texture fills 68.75% the last column
		float m_last_col_fill_ratio;
		// @see m_last_col_fill_ratio
		float m_last_row_fill_ratio;

		/** the width of last column to render. This is also power of two 
		 * (e.g. if chunks are 256x256 and image width = 300, last column = 64
		 */
		unsigned int m_last_col_width;
		// see m_last_col_width 
		unsigned int m_last_row_height;

		/** Holds texture ids that are used to access textures in GL rendering context
		 */
		GLuint* m_textureids;

		/** Frees allocated memory and calls resetGlImage
		 */
		void cleanup();
		
		/** Resets GLImage variables
		 */
		void resetGlimage();
		
		//void saveAsPng(const std::string& filename, SDL_Surface& surface);

		/** Generates chunks for render. For reference, see
		 * http://developer.apple.com/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_texturedata/chapter_10_section_4.html
		 */
		void generateTextureChunks();
		
		/** Original SDLImage where GLImage is created from
		 * FIXME: at the moment SDLImage is used to draw graphics (e.g. line) on screen
		 * this is clearly not optimal, but image chunking makes somewhat harder to do
		 * proper drawing of graphics (e.g. how to segment lines into correct boxes).
		 * It might be possible to use some kind of offscreen OpenGL image for this
		 * purpose
		 */
		SDLImage* m_sdlimage;
		
		unsigned int m_chunk_size;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
