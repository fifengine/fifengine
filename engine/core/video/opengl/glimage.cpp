/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
#include "video/opengl/renderbackendopengl.h"

#include "glimage.h"

namespace FIFE {
	GLImage::GLImage(IResourceLoader* loader):
		Image(loader),
		m_texId(0){

		resetGlimage();
	}

	GLImage::GLImage(const std::string& name, IResourceLoader* loader):
		Image(name, loader),
		m_texId(0) {

		resetGlimage();
	}

	GLImage::GLImage(SDL_Surface* surface):
		Image(surface),
		m_texId(0) {

		resetGlimage();
	}

	GLImage::GLImage(const std::string& name, SDL_Surface* surface):
		Image(name, surface),
		m_texId(0) {

		resetGlimage();
	}

	GLImage::GLImage(const uint8_t* data, uint32_t width, uint32_t height):
		Image(data, width, height),
		m_texId(0) {

		assert(m_surface);
		resetGlimage();
	}

	GLImage::GLImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height):
		Image(name, data, width, height),
		m_texId(0) {

		assert(m_surface);
		resetGlimage();
	}

	GLImage::~GLImage() {
		cleanup();
	}

	void GLImage::invalidate() {
		resetGlimage();
	}

	void GLImage::setSurface(SDL_Surface* surface) {
		reset(surface);
		resetGlimage();
	}

	void GLImage::resetGlimage() {
		cleanup();

		m_chunk_size_w = 0;
		m_chunk_size_h = 0;

		m_colorkey = RenderBackend::instance()->getColorKey();
	}

	void GLImage::cleanup() {
		if (m_texId) {
			glDeleteTextures(1, &m_texId);
			m_texId = 0;
		}

		texCoords[0] = texCoords[1] = 
			texCoords[2] = texCoords[3] = 0.0f;
	}

	void GLImage::render(const Rect& rect, SDL_Surface* screen, uint8_t alpha, uint8_t const* rgb) {
		// not on the screen.  dont render
		if (rect.right() < 0 || rect.x > static_cast<int32_t>(screen->w) || rect.bottom() < 0 || rect.y > static_cast<int32_t>(screen->h)) {
			return;
		}

		// completely transparent so dont bother rendering
		if (0 == alpha) {
			return;
		}

		if (!m_texId) {
			generateGLTexture();
		}

		if(!rgb) {
			RenderBackend::instance()->addImageToArray(m_texId, rect, texCoords, alpha);
		} else {
			RenderBackend::instance()->addImageToArray2T(m_texId, rect, texCoords, alpha, rgb);
		}
	}

	void GLImage::generateGLTexture() {
		const uint32_t width = m_surface->w;
		const uint32_t height = m_surface->h;

		// With OpenGL 2.0 or GL_ARB_texture_non_power_of_two we don't really need to care
		// about non power of 2 textures 
		if(GLEE_ARB_texture_non_power_of_two) {
			m_chunk_size_w = width;
			m_chunk_size_h = height;
		}
		else {
			//calculate the nearest larger power of 2
			m_chunk_size_w = nextPow2(width);
			m_chunk_size_h = nextPow2(height);
		}

		// used to calculate the fill ratio for given chunk
		texCoords[0] = texCoords[1] = 0.0f;
		texCoords[2] = static_cast<float>(m_surface->w%m_chunk_size_w) / static_cast<float>(m_chunk_size_w);
		texCoords[3] = static_cast<float>(m_surface->h%m_chunk_size_h) / static_cast<float>(m_chunk_size_h);

		if (texCoords[2] == 0.0f){
			texCoords[2] = 1.0f;
		}

		if (texCoords[3] == 0.0f){
			texCoords[3] = 1.0f;
		}

		uint8_t* data = static_cast<uint8_t*>(m_surface->pixels);
		int32_t pitch = m_surface->pitch;

		assert(!m_texId);

		// get texture id from opengl
		glGenTextures(1, &m_texId);
		// set focus on that texture
		static_cast<RenderBackendOpenGL*>(RenderBackend::instance())->bindTexture(m_texId);
		// set filters for texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		
		if(GLEE_ARB_texture_non_power_of_two) {
			if(RenderBackend::instance()->isColorKeyEnabled()) {
				uint8_t* oglbuffer = new uint8_t[width * height * 4];
				memcpy(oglbuffer, data, width * height * 4 * sizeof(uint8_t));

				for (uint32_t y = 0;  y < height; ++y) {
					for (uint32_t x = 0; x < width * 4; x += 4) {
						uint32_t gid = x + y * width;

						uint8_t r = oglbuffer[gid + 0];
						uint8_t g = oglbuffer[gid + 1];
						uint8_t b = oglbuffer[gid + 2];
						uint8_t a = oglbuffer[gid + 3];

						// set alpha to zero
						if (r == m_colorkey.r && g == m_colorkey.g && b == m_colorkey.b) {
							oglbuffer[gid + 3] = 0;
						}
					}
				}

				// transfer data from sdl buffer
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_chunk_size_w, m_chunk_size_h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, oglbuffer);

				delete [] oglbuffer;
			} else {

				// transfer data directly from sdl buffer
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_chunk_size_w, m_chunk_size_h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
		// Non power of 2 textures are not supported, we need to pad the size of texture to nearest power of 2
		} else {
			uint32_t* oglbuffer = new uint32_t[m_chunk_size_w * m_chunk_size_h];
			memset(oglbuffer, 0x00, m_chunk_size_w*m_chunk_size_h*sizeof(uint32_t));

			for (uint32_t y = 0;  y < height; ++y) {
				for (uint32_t x = 0; x < width; ++x) {
					uint32_t pos = (y * pitch) + (x * 4);

					uint8_t a = data[pos + 3];
					uint8_t b = data[pos + 2];
					uint8_t g = data[pos + 1];
					uint8_t r = data[pos + 0];

					if (RenderBackend::instance()->isColorKeyEnabled()) {
						// only set alpha to zero if colorkey feature is enabled
						if (r == m_colorkey.r && g == m_colorkey.g && b == m_colorkey.b) {
							a = 0;
						}
					}

					oglbuffer[(y*m_chunk_size_w) + x] = r | (g << 8) | (b << 16) | (a<<24);
				}
			}

			// transfer data from sdl buffer
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_chunk_size_w, m_chunk_size_h,
				0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<GLvoid*>(oglbuffer));

			delete[] oglbuffer;
		}
	}

	void GLImage::useSharedImage(const ImagePtr& shared, const Rect& region) {
		GLImage* img = static_cast<GLImage*>(shared.get());
		setSurface(img->m_surface);

		m_texId = img->m_texId;
		m_shared = true;

		uint32_t width = shared->getWidth();
		uint32_t height = shared->getHeight();

		if(!GLEE_ARB_texture_non_power_of_two) {
			width = nextPow2(width);
			height = nextPow2(height);
		}

		texCoords[0] = static_cast<GLfloat>(region.x) / static_cast<GLfloat>(width);
		texCoords[1] = static_cast<GLfloat>(region.y) / static_cast<GLfloat>(height);
		texCoords[2] = static_cast<GLfloat>(region.x + region.w) / static_cast<GLfloat>(width);
		texCoords[3] = static_cast<GLfloat>(region.y + region.h) / static_cast<GLfloat>(height);

		m_subimagerect = region;
		setState(IResource::RES_LOADED);
	}

	void GLImage::forceLoadInternal() {
		assert(m_surface);

		if(m_shared)
			return;
		generateGLTexture();
	}
}
