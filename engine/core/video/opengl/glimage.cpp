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

// Standard C++ library includes
#include <cassert>
#include <iostream>
// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"
#include "video/sdl/sdlimage.h"
#include "video/renderbackend.h"

#include "glimage.h"

namespace FIFE {
	GLImage::GLImage(SDL_Surface* surface):
		Image(surface) {
		m_sdlimage = new SDLImage(surface);

		m_textureids = NULL;

		resetGlimage();
	}

	GLImage::GLImage(const uint8_t* data, unsigned int width, unsigned int height):
		Image(data, width, height) {
		assert(m_surface);
		m_sdlimage = new SDLImage(m_surface);

		m_textureids = NULL;

		resetGlimage();
	}

	GLImage::~GLImage() {
		// remove surface so that deletion happens correctly (by base class destructor)
		m_sdlimage->detachSurface();
		delete m_sdlimage;

		cleanup();
	}

	void GLImage::invalidate() {
		resetGlimage();
	}

	void GLImage::resetGlimage() {
		cleanup();

		m_chunk_size_w = 0;
		m_chunk_size_h = 0;

		m_colorkey = RenderBackend::instance()->getColorKey();
	}

	void GLImage::cleanup() {
		if (m_textureids) {
			glDeleteTextures(1, &m_textureids[0]);

			delete[] m_textureids;
			m_textureids = NULL;
		}

		m_col_tex_coord = 0;
		m_row_tex_coord = 0;
	}

	void GLImage::render(const Rect& rect, SDL_Surface* screen, unsigned char alpha) {
		if (!m_textureids) {
			generateGLTexture();
		}

		//not on the screen.  dont render
		if (rect.right() < 0 || rect.x > static_cast<int>(screen->w) || rect.bottom() < 0 || rect.y > static_cast<int>(screen->h)) {
			return;
		}

		//completely transparent so dont bother rendering
		if (0 == alpha) {
			return;
		}

		// the amount of "zooming" for the image
		float scale_x = static_cast<float>(rect.w) / static_cast<float>(m_surface->w);
		float scale_y = static_cast<float>(rect.h) / static_cast<float>(m_surface->h);

		// apply the scale to the width and height of the image
		uint16_t w = static_cast<int>(round(scale_x*m_surface->w));
		uint16_t h = static_cast<int>(round(scale_y*m_surface->h));

		// setting transparency for the whole primitive:
		glColor4ub( 255, 255, 255, alpha );

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_textureids[0]);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2i(rect.x, rect.y);

			glTexCoord2f(0.0f, m_row_tex_coord);
			glVertex2i(rect.x, rect.y + h);

			glTexCoord2f(m_col_tex_coord, m_row_tex_coord);
			glVertex2i(rect.x + w, rect.y + h);

			glTexCoord2f(m_col_tex_coord, 0.0f);
			glVertex2i(rect.x + w, rect.y);
		glEnd();
		glDisable(GL_TEXTURE_2D);

	}

	void GLImage::generateGLTexture() {
		const unsigned int width = m_surface->w;
		const unsigned int height = m_surface->h;

		//calculate the nearest larger power of 2
		m_chunk_size_w = nextPow2(width);
		m_chunk_size_h = nextPow2(height);

		// used to calculate the fill ratio for given chunk
		m_col_tex_coord = static_cast<float>(m_surface->w%m_chunk_size_w) / static_cast<float>(m_chunk_size_w);
		m_row_tex_coord = static_cast<float>(m_surface->h%m_chunk_size_h) / static_cast<float>(m_chunk_size_h);

		if (m_col_tex_coord == 0.0f){
			m_col_tex_coord = 1.0f;
		}

		if (m_row_tex_coord == 0.0f){
			m_row_tex_coord = 1.0f;
		}

		uint8_t* data = static_cast<uint8_t*>(m_surface->pixels);
		int pitch = m_surface->pitch;


		assert(!m_textureids);

		m_textureids = new GLuint[1];
		memset(m_textureids, 0x00, 1*sizeof(GLuint));


		uint32_t* oglbuffer = new uint32_t[m_chunk_size_w * m_chunk_size_h];
		memset(oglbuffer, 0x00, m_chunk_size_w*m_chunk_size_h*sizeof(uint32_t));

		for (unsigned int y = 0;  y < height; ++y) {
			for (unsigned int x = 0; x < width; ++x) {
				unsigned int pos = (y * pitch) + (x * 4);

				uint8_t r = data[pos + 3];
				uint8_t g = data[pos + 2];
				uint8_t b = data[pos + 1];
				uint8_t a = data[pos + 0];

				if (RenderBackend::instance()->isColorKeyEnabled()) {
					// only set alpha to zero if colorkey feature is enabled
					if (r == m_colorkey.r && g == m_colorkey.g && b == m_colorkey.b) {
						a = 0;
					}
				}

				oglbuffer[(y*m_chunk_size_w) + x] = r | (g << 8) | (b << 16) | (a<<24);
			}
		}

		// get texture id from opengl
		glGenTextures(1, &m_textureids[0]);
		// set focus on that texture
		glBindTexture(GL_TEXTURE_2D, m_textureids[0]);
		// set filters for texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// transfer data from sdl buffer
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_chunk_size_w, m_chunk_size_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<GLvoid*>(oglbuffer));

		delete[] oglbuffer;
	}

	void GLImage::saveImage(const std::string& filename) {
		const unsigned int swidth = getWidth();
		const unsigned int sheight = getHeight();
		SDL_Surface *surface = NULL;
		uint8_t *pixels;

		surface = SDL_CreateRGBSurface(SDL_SWSURFACE, swidth,
			sheight, 24,
			RMASK,GMASK,BMASK, NULLMASK);

		if(surface == NULL) {
			return;
		}

		SDL_LockSurface(surface);
		pixels = new uint8_t[swidth * sheight * 3];
		glReadPixels(0, 0, swidth, sheight, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(pixels));

		uint8_t *imagepixels = reinterpret_cast<uint8_t*>(surface->pixels);
		// Copy the "reversed_image" memory to the "image" memory
		for (int y = (sheight - 1); y >= 0; --y) {
			uint8_t *rowbegin = pixels + y * swidth * 3;
			uint8_t *rowend = rowbegin + swidth * 3;

			std::copy(rowbegin, rowend, imagepixels);

			// Advance a row in the output surface.
			imagepixels += surface->pitch;
		}

		SDL_UnlockSurface(surface);
		saveAsPng(filename, *surface);
		SDL_FreeSurface(surface);
		delete [] pixels;
	}

	void GLImage::setClipArea(const Rect& cliparea, bool clear) {
		glScissor(cliparea.x, getHeight() - cliparea.y - cliparea.h, cliparea.w, cliparea.h);

		if (clear) {
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}

	bool GLImage::putPixel(int x, int y, int r, int g, int b, int a) {
		cleanup();
		return m_sdlimage->putPixel(x, y, r, g, b, a);
	}

	void GLImage::drawLine(const Point& p1, const Point& p2, int r, int g, int b, int a) {
		cleanup();
		m_sdlimage->drawLine(p1, p2, r, g, b, a);
	}

	void GLImage::drawTriangle(const Point& p1, const Point& p2, const Point& p3, int r, int g, int b, int a) {
		cleanup();
		m_sdlimage->drawTriangle(p1, p2, p3, r, g, b, a);
	}

	void GLImage::drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		cleanup();
		m_sdlimage->drawRectangle(p, w, h, r, g, b, a);
	}

	void GLImage::fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		cleanup();
		m_sdlimage->fillRectangle(p, w, h, r, g, b, a);
	}

	void GLImage::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b, int a) {
		cleanup();
		m_sdlimage->drawQuad(p1, p2, p3, p4, r, g, b, a);
	}

	void GLImage::drawVertex(const Point& p, const uint8_t size, int r, int g, int b, int a) {
		cleanup();
		m_sdlimage->drawVertex(p, size, r, g, b, a);
	}

	void GLImage::drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue) {
		cleanup();
		m_sdlimage->drawLightPrimitive(p, intensity, radius, subdivisions, xstretch, ystretch, red, green, blue);
	}
}
