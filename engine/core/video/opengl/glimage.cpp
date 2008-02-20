/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/rect.h"

#include "glimage.h"

namespace FIFE {

	GLImage::GLImage(SDL_Surface* surface):
		Image(surface) {
		resetGlimage();
	}

	GLImage::GLImage(const uint8_t* data, unsigned int width, unsigned int height):
		Image(data, width, height) {
		resetGlimage();
	}

	GLImage::~GLImage() {
		cleanup();
	}

	void GLImage::resetGlimage() {
		m_tex_x = 0;
		m_tex_y = 0;
		m_textureid = 0;
		m_rows = 0;
		m_cols = 0;
		m_last_col_width = 0;
		m_last_row_height = 0;
	}
	
	void GLImage::cleanup() {
		for (unsigned int i = 0; i < m_rows*m_cols; ++i) {
			glDeleteTextures(1, &m_textureid[i]);
		}
		delete[] m_textureid;
	}

	void GLImage::render(const Rect& rect, SDL_Surface* screen, unsigned char alpha) {
		if( m_textureid == 0 ) {
			generateTextureChunks();
		}

		if (rect.right() < 0 || rect.x > static_cast<int>(screen->w) || rect.bottom() < 0 || rect.y > static_cast<int>(screen->h)) {
			return;
		}

		if (0 == alpha) {
			return;
		}

		float tex_end_x;
		float tex_end_y;

		float scale_x = static_cast<float>(rect.w) / static_cast<float>(m_surface->w);
		float scale_y = static_cast<float>(rect.h) / static_cast<float>(m_surface->h);

		Rect target;
		/// setting transparency for whole primitive:
		glColor4ub( 255, 255, 255, alpha );

		glEnable(GL_TEXTURE_2D);
		for (unsigned int i = 0; i < m_cols; ++i) {
			for (unsigned int j = 0; j < m_rows; ++j) {
				glBindTexture(GL_TEXTURE_2D, m_textureid[j*m_cols + i]);

				if (i == m_cols-1) {
					tex_end_x = m_tex_x;
				} else {
					tex_end_x = 1.0;
				}
				if (j == m_rows-1) {
					tex_end_y = m_tex_y;
				} else {
					tex_end_y = 1.0;
				}

				target.x = rect.x + static_cast<int>(i*256*scale_x);
				target.y = rect.y + static_cast<int>(j*256*scale_y);

				if (i == m_cols-1) {
					target.w = static_cast<int>(scale_y*m_last_col_width*m_tex_x);
				} else {
					target.w = static_cast<int>(scale_y*256);
				}
				if (j == m_rows-1) {
					target.h = static_cast<int>(scale_y*m_last_row_height*m_tex_y);
				} else {
					target.h = static_cast<int>(scale_y*256);
				}

				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f);
					glVertex2i(target.x, target.y);

					glTexCoord2f(0.0f, tex_end_y);
					glVertex2i(target.x, target.y + target.h);

					glTexCoord2f(tex_end_x, tex_end_y);
					glVertex2i(target.x + target.w, target.y + target.h);

					glTexCoord2f(tex_end_x, 0.0f);
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
		m_cols = static_cast<int>(width/256);
		if (width%256) {
			++m_cols;
			while(m_last_col_width < width%256) {
				m_last_col_width <<= 1;
			}
		} else {
			m_last_col_width = 256;
		}

		m_last_row_height = 1;
		m_rows = static_cast<int>(height/256);
		if (height%256) {
			++m_rows;
			while(m_last_row_height < height%256) {
				m_last_row_height <<= 1;
			}
		} else {
			m_last_row_height = 256;
		}

		m_textureid = new GLuint[m_rows*m_cols];
		memset(m_textureid, 0x00, m_rows*m_cols*sizeof(GLuint));

		if(width%256) {
			m_tex_x = static_cast<float>(width%256) / static_cast<float>(m_last_col_width);
			m_tex_y = static_cast<float>(height%256) / static_cast<float>(m_last_row_height);
		}
		else {  // (width%256) / m_last_col_width == 0 == 256 (mod 256)
			m_tex_x = 1.0f;
			m_tex_y = 1.0f;
		}

		unsigned int chunk_width;
		unsigned int chunk_height;
		unsigned int data_chunk_height;
		unsigned int data_chunk_width;

		for (unsigned int i = 0; i < m_cols; ++i) {
			for (unsigned int j = 0; j < m_rows; ++j) {
				if (i==m_cols-1) {
					chunk_width = m_last_col_width;
					data_chunk_width = width%256;
					if(data_chunk_width == 0) {  // 0 == 256 (mod 256)
						data_chunk_width = 256;
					}
				} else {
					chunk_width = 256;
					data_chunk_width = 256;
				}
				if (j==m_rows-1) {
					chunk_height = m_last_row_height;
					data_chunk_height = height%256;
					if(data_chunk_height == 0) {  // 0 = 256 (mod 256)
						data_chunk_height = 256;
					}
				} else {
					chunk_height = 256;
					data_chunk_height = 256;
				}

				uint32_t* oglbuffer = new uint32_t[chunk_width * chunk_height];
				memset(oglbuffer, 0x00, chunk_width*chunk_height*4);

				for (unsigned int y = 0;  y < data_chunk_height; ++y) {
					for (unsigned int x = 0; x < data_chunk_width; ++x) {
						unsigned int pos = (y + j*256)*pitch + (x + i*256) * 4;

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
				glGenTextures(1, &m_textureid[j*m_cols + i]);
				// set focus on that texture
				glBindTexture(GL_TEXTURE_2D, m_textureid[j*m_cols + i]);
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
		unsigned int swidth = getWidth();
		unsigned int sheight = getHeight();

		// We need unsigned char to avoid pointer alignment issues
		uint8_t *pixels = new uint8_t[swidth * sheight * 3];

		// Read in the pixel data
		glReadPixels(0, 0, swidth, sheight, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(pixels));

		/* At this point the image has been reversed, so we need to re-reverse it so that
		it is the correct way around. We do this by copying the "image" pixels to another
		surface in reverse order */
		SDL_Surface* image = SDL_CreateRGBSurface(SDL_SWSURFACE, swidth, sheight, 24,
			255U << (0), // Blue channel
			255U << (8), // Green channel
			255U << (16), // Red channel
			0 /* no alpha! */);
		SDL_LockSurface(image);

		uint8_t *imagepixels = reinterpret_cast<uint8_t*>(image->pixels);
		// Copy the "reversed_image" memory to the "image" memory
		for (int y = (sheight - 1); y >= 0; --y) {
			uint8_t *row_begin = pixels + y * swidth * 3;
			uint8_t *row_end = row_begin + swidth * 3;

			std::copy(row_begin, row_end, imagepixels);

			// Advance a row in the output surface.
			imagepixels += image->pitch;
		}
		SDL_UnlockSurface(image);

		// Save file
		SDL_SaveBMP(image, filename.c_str());

		// Clear memory
		delete []pixels;
		SDL_FreeSurface( image );
	}
	
	void GLImage::setClipArea(const Rect& cliparea, bool clear) {
	        glScissor(cliparea.x, getHeight() - cliparea.y - cliparea.h, cliparea.w, cliparea.h);
		if (clear) {
	        	glClear(GL_COLOR_BUFFER_BIT);
		}
	}
}
