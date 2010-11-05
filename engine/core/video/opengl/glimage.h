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
#include <SDL_video.h>

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
		void invalidate();
		void render(const Rect& rect, SDL_Surface* dst, unsigned char alpha = 255);
		void saveImage(const std::string& filename);
 		bool putPixel(int x, int y, int r, int g, int b, int a = 255);
		void drawLine(const Point& p1, const Point& p2, int r, int g, int b, int a = 255);
		void drawTriangle(const Point& p1, const Point& p2, const Point& p3, int r, int g, int b, int a = 255);
		void drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b, int a = 255);
		void drawVertex(const Point& p, const uint8_t size, int r, int g, int b, int a = 255);
		void drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue);

	protected:
		void setClipArea(const Rect& cliparea, bool clear);

	private:
		// texture coords to use
		float m_col_tex_coord;
		// @see m_col_tex_coord
		float m_row_tex_coord;

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

		/** Generates the GL Texture for use when rendering.
		 */
		void generateGLTexture();

		/** Original SDLImage where GLImage is created from
		 * FIXME: at the moment SDLImage is used to draw graphics (e.g. line) on screen
		 * this is clearly not optimal, but image chunking makes somewhat harder to do
		 * proper drawing of graphics (e.g. how to segment lines into correct boxes).
		 * It might be possible to use some kind of offscreen OpenGL image for this
		 * purpose
		 */
		SDLImage* m_sdlimage;

		uint32_t m_chunk_size_w;
		uint32_t m_chunk_size_h;

		SDL_Color m_colorkey;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
