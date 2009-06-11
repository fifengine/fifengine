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
		resetGlimage();
	}

	GLImage::GLImage(const uint8_t* data, unsigned int width, unsigned int height):
		Image(data, width, height) {
		resetGlimage();
	}

	GLImage::~GLImage() {
		// remove surface so that deletion happens correctly (by base class destructor)
		m_sdlimage->detachSurface();
		delete m_sdlimage;
		cleanup();
	}

	void GLImage::resetGlimage() {
		m_last_col_fill_ratio = 0;
		m_last_row_fill_ratio = 0;
		m_textureids = NULL;
		m_rows = 0;
		m_cols = 0;
		m_last_col_width = 0;
		m_last_row_height = 0;
		m_chunk_size = RenderBackend::instance()->getChunkingSize();
	}
	
	void GLImage::cleanup() {
		for (unsigned int i = 0; i < m_rows*m_cols; ++i) {
			glDeleteTextures(1, &m_textureids[i]);
		}
		delete[] m_textureids;
		m_textureids = NULL;
		resetGlimage();
	}

	void GLImage::render(const Rect& rect, SDL_Surface* screen, unsigned char alpha) {
		if (!m_textureids) {
			generateTextureChunks();
		}

		if (rect.right() < 0 || rect.x > static_cast<int>(screen->w) || rect.bottom() < 0 || rect.y > static_cast<int>(screen->h)) {
			return;
		}

		if (0 == alpha) {
			return;
		}

		// used to calculate the fill ratio for given chunk
		float col_fill_ratio;
		float row_fill_ratio;

		// the amount of "zooming" for the image
		float scale_x = static_cast<float>(rect.w) / static_cast<float>(m_surface->w);
		float scale_y = static_cast<float>(rect.h) / static_cast<float>(m_surface->h);
		
		// rectangle used for drawing
		Rect target;
		// zooming causes scaling sometimes to round pixels incorrectly. Instead of 
		//  recalculating it all, store the values from previous round and calculate
		//  new x & y
		Rect prev;
		
		/// setting transparency for the whole primitive:
		glColor4ub( 255, 255, 255, alpha );

		glEnable(GL_TEXTURE_2D);
		for (unsigned int i = 0; i < m_cols; ++i) {
			if (i == m_cols-1) {
				col_fill_ratio = m_last_col_fill_ratio;
				target.w = static_cast<int>(round(scale_y*m_last_col_width*m_last_col_fill_ratio));
			} else {
				col_fill_ratio = 1.0;
				target.w = static_cast<int>(round(scale_y*m_chunk_size));
			}
			if (i > 0) {
				target.x = prev.x + prev.w;
			} else {
				target.x = rect.x;
			}
			
			for (unsigned int j = 0; j < m_rows; ++j) {
				if (j == m_rows-1) {
					row_fill_ratio = m_last_row_fill_ratio;
					target.h = static_cast<int>(round(scale_y*m_last_row_height*m_last_row_fill_ratio));
				} else {
					row_fill_ratio = 1.0;
					target.h = static_cast<int>(round(scale_y*m_chunk_size));
				}
				if (j > 0) {
					target.y = prev.y + prev.h;
				} else {
					target.y = rect.y;
				}
				prev = target;
				
				glBindTexture(GL_TEXTURE_2D, m_textureids[j*m_cols + i]);
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f);
					glVertex2i(target.x, target.y);

					glTexCoord2f(0.0f, row_fill_ratio);
					glVertex2i(target.x, target.y + target.h);

					glTexCoord2f(col_fill_ratio, row_fill_ratio);
					glVertex2i(target.x + target.w, target.y + target.h);

					glTexCoord2f(col_fill_ratio, 0.0f);
					glVertex2i(target.x + target.w, target.y);
				glEnd();
			}
		}
		glDisable(GL_TEXTURE_2D);
	}

	void GLImage::generateTextureChunks() {
		const unsigned int width = m_surface->w;
		const unsigned int height = m_surface->h;
		uint8_t* data = static_cast<uint8_t*>(m_surface->pixels);
		int pitch = m_surface->pitch;

		m_last_col_width = 1;
		m_cols = static_cast<int>(width/m_chunk_size);
		if (width%m_chunk_size) {
			++m_cols;
			while(m_last_col_width < width%m_chunk_size) {
				m_last_col_width <<= 1;
			}
		} else {
			m_last_col_width = m_chunk_size;
		}

		m_last_row_height = 1;
		m_rows = static_cast<int>(height/m_chunk_size);
		if (height%m_chunk_size) {
			++m_rows;
			while(m_last_row_height < height%m_chunk_size) {
				m_last_row_height <<= 1;
			}
		} else {
			m_last_row_height = m_chunk_size;
		}

		m_textureids = new GLuint[m_rows*m_cols];
		memset(m_textureids, 0x00, m_rows*m_cols*sizeof(GLuint));

		if(width%m_chunk_size) {
			m_last_col_fill_ratio = static_cast<float>(width%m_chunk_size) / static_cast<float>(m_last_col_width);
		} else {  // (width%m_chunk_size) / m_last_col_width == 0 == m_chunk_size (mod m_chunk_size)
			m_last_col_fill_ratio = 1.0f;
		}

		if (height%m_chunk_size) {
			m_last_row_fill_ratio = static_cast<float>(height%m_chunk_size) / static_cast<float>(m_last_row_height);
		} else {
			m_last_row_fill_ratio = 1.0f;
		}

		unsigned int chunk_width;
		unsigned int chunk_height;
		unsigned int data_chunk_height;
		unsigned int data_chunk_width;

		for (unsigned int i = 0; i < m_cols; ++i) {
			for (unsigned int j = 0; j < m_rows; ++j) {
				if (i==m_cols-1) {
					chunk_width = m_last_col_width;
					data_chunk_width = width%m_chunk_size;
					if(data_chunk_width == 0) {  // 0 == m_chunk_size (mod m_chunk_size)
						data_chunk_width = m_chunk_size;
					}
				} else {
					chunk_width = m_chunk_size;
					data_chunk_width = m_chunk_size;
				}
				if (j==m_rows-1) {
					chunk_height = m_last_row_height;
					data_chunk_height = height%m_chunk_size;
					if(data_chunk_height == 0) {  // 0 = m_chunk_size (mod m_chunk_size)
						data_chunk_height = m_chunk_size;
					}
				} else {
					chunk_height = m_chunk_size;
					data_chunk_height = m_chunk_size;
				}

				uint32_t* oglbuffer = new uint32_t[chunk_width * chunk_height];
				memset(oglbuffer, 0x00, chunk_width*chunk_height*4);

				for (unsigned int y = 0;  y < data_chunk_height; ++y) {
					for (unsigned int x = 0; x < data_chunk_width; ++x) {
						unsigned int pos = (y + j*m_chunk_size)*pitch + (x + i*m_chunk_size) * 4;

						// FIXME
						// The following code might not be endianness correct

						uint8_t r = data[pos + 0];
						uint8_t g = data[pos + 1];
						uint8_t b = data[pos + 2];
						uint8_t a = data[pos + 3];

						oglbuffer[(y*chunk_width) + x] = (r << 24) | (g << 16) | (b << 8) | a;
					}
				}

				// get texture id from opengl
				glGenTextures(1, &m_textureids[j*m_cols + i]);
				// set focus on that texture
				glBindTexture(GL_TEXTURE_2D, m_textureids[j*m_cols + i]);
				// set filters for texture
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				// transfer data from sdl buffer
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, chunk_width, chunk_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<GLvoid*>(oglbuffer));

				delete[] oglbuffer;
			}
		}
	}
	
	void GLImage::saveImage(const std::string& filename) {
		const unsigned int swidth = getWidth();
		const unsigned int sheight = getHeight();
		Uint32 rmask, gmask, bmask, amask;
		SDL_Surface *surface = NULL;
		uint8_t *pixels; 
		
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
		#else
		rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
		#endif

		surface = SDL_CreateRGBSurface(SDL_SWSURFACE, swidth, 
			sheight, 24, 
			rmask, gmask, bmask, 0);
		
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
	
	bool GLImage::putPixel(int x, int y, int r, int g, int b) {
		cleanup();
		return m_sdlimage->putPixel(x, y, r, g, b);
	}
	
	void GLImage::drawLine(const Point& p1, const Point& p2, int r, int g, int b) {
		cleanup();
		m_sdlimage->drawLine(p1, p2, r, g, b);
	}	
	
	void GLImage::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b) {
		cleanup();
		m_sdlimage->drawQuad(p1, p2, p3, p4, r, g, b);
	}
}
