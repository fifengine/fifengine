/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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
#include "video/imagemanager.h"
#include "video/sdl/sdlimage.h"
#include "video/renderbackend.h"
#include "video/opengl/renderbackendopengl.h"

#include "glimage.h"

namespace FIFE {
	GLImage::GLImage(IResourceLoader* loader):
		Image(loader),
		m_compressed(false),
		m_texId(0) {

		resetGlimage();
	}

	GLImage::GLImage(const std::string& name, IResourceLoader* loader):
		Image(name, loader),
		m_compressed(false),
		m_texId(0) {

		resetGlimage();
	}

	GLImage::GLImage(SDL_Surface* surface):
		Image(surface),
		m_compressed(false),
		m_texId(0) {

		resetGlimage();
	}

	GLImage::GLImage(const std::string& name, SDL_Surface* surface):
		Image(name, surface),
		m_compressed(false),
		m_texId(0) {

		resetGlimage();
	}

	GLImage::GLImage(const uint8_t* data, uint32_t width, uint32_t height):
		Image(data, width, height),
		m_compressed(false),
		m_texId(0) {

		assert(m_surface);
		resetGlimage();
	}

	GLImage::GLImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height):
		Image(name, data, width, height),
		m_compressed(false),
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
			if(!m_shared) {
				glDeleteTextures(1, &m_texId);
			}
			m_texId = 0;
			m_compressed = false;
		}

		m_tex_coords[0] = m_tex_coords[1] = 
			m_tex_coords[2] = m_tex_coords[3] = 0.0f;
	}

	void GLImage::render(const Rect& rect, uint8_t alpha, uint8_t const* rgb) {
		// completely transparent so dont bother rendering
		if (0 == alpha) {
			return;
		}
		RenderBackend* rb = RenderBackend::instance();
		SDL_Surface* target = rb->getRenderTargetSurface();
		assert(target != m_surface); // can't draw on the source surface
		
		// not on the screen.  dont render
		if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) || 
			rect.bottom() < 0 || rect.y > static_cast<int32_t>(target->h)) {
			return;
		}
		if (!m_texId) {
			generateGLTexture();
		} else if (m_shared) {
			validateShared();
		}

		rb->addImageToArray(m_texId, rect, m_tex_coords, alpha, rgb);
	}

	void GLImage::render(const Rect& rect, const ImagePtr& overlay, uint8_t alpha, uint8_t const* rgb) {
		// completely transparent so dont bother rendering
		if (0 == alpha) {
			return;
		}
		RenderBackend* rb = RenderBackend::instance();
		SDL_Surface* target = rb->getRenderTargetSurface();
		assert(target != m_surface); // can't draw on the source surface
		
		// not on the screen.  dont render
		if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) || 
			rect.bottom() < 0 || rect.y > static_cast<int32_t>(target->h)) {
			return;
		}
		if (!m_texId) {
			generateGLTexture();
		} else if (m_shared) {
			validateShared();
		}

		GLImage* img = static_cast<GLImage*>(overlay.get());
		img->forceLoadInternal();

		static_cast<RenderBackendOpenGL*>(rb)->addImageToArray(rect, m_texId, m_tex_coords, img->getTexId(), img->getTexCoords(), alpha, rgb);
		//rb->addImageToArray(rect, m_texId, m_tex_coords, img->getTexId(), img->getTexCoords(), alpha, rgb);
	}

	void GLImage::renderZ(const Rect& rect, float vertexZ, uint8_t alpha, uint8_t const* rgb) {
		// completely transparent so dont bother rendering
		if (0 == alpha) {
			return;
		}
		RenderBackend* rb = RenderBackend::instance();
		SDL_Surface* target = rb->getRenderTargetSurface();
		assert(target != m_surface); // can't draw on the source surface
		
		// not on the screen.  dont render
		if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) || 
			rect.bottom() < 0 || rect.y > static_cast<int32_t>(target->h)) {
			return;
		}
		if (!m_texId) {
			generateGLTexture();
		} else if (m_shared) {
			validateShared();
		}
		static_cast<RenderBackendOpenGL*>(rb)->addImageToArrayZ(m_texId, rect, vertexZ, m_tex_coords, alpha, rgb);
		//rb->addImageToArray(m_texId, rect, m_tex_coords, alpha, rgb);
	}

	void GLImage::renderZ(const Rect& rect, float vertexZ, const ImagePtr& overlay, uint8_t alpha, uint8_t const* rgb) {
		// completely transparent so dont bother rendering
		if (0 == alpha) {
			return;
		}
		RenderBackend* rb = RenderBackend::instance();
		SDL_Surface* target = rb->getRenderTargetSurface();
		assert(target != m_surface); // can't draw on the source surface
		
		// not on the screen.  dont render
		if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) || 
			rect.bottom() < 0 || rect.y > static_cast<int32_t>(target->h)) {
			return;
		}
		
		if (!m_texId) {
			generateGLTexture();
		} else if (m_shared) {
			validateShared();
		}
		
		GLImage* img = static_cast<GLImage*>(overlay.get());
		img->forceLoadInternal();
		
		static_cast<RenderBackendOpenGL*>(rb)->addImageToArrayZ(rect, vertexZ, m_texId, m_tex_coords, img->getTexId(), img->getTexCoords(), alpha, rgb);
		//rb->addImageToArray(rect, m_texId, m_tex_coords, img->getTexId(), img->getTexCoords(), alpha, rgb);
	}

	void GLImage::generateGLTexture() {
		if (m_shared) {
			// First make sure we loaded big image to opengl
			validateShared();
			return;
		}
		// ultimate possibility to load the image
		// is used e.g. in case a cursor or gui image is freed even if there is a reference 
		if (!m_surface) {
			if (m_state == IResource::RES_NOT_LOADED) {
				load();
			}
		}
		const uint32_t width = m_surface->w;
		const uint32_t height = m_surface->h;

		// With OpenGL 2.0 or GL_ARB_texture_non_power_of_two we don't really need to care
		// about non power of 2 textures 
		if(GLEE_ARB_texture_non_power_of_two && RenderBackend::instance()->isNPOTEnabled()) {
			m_chunk_size_w = width;
			m_chunk_size_h = height;
		}
		else {
			//calculate the nearest larger power of 2
			m_chunk_size_w = nextPow2(width);
			m_chunk_size_h = nextPow2(height);
		}

		// used to calculate the fill ratio for given chunk
		m_tex_coords[0] = m_tex_coords[1] = 0.0f;
		m_tex_coords[2] = static_cast<float>(m_surface->w%m_chunk_size_w) / static_cast<float>(m_chunk_size_w);
		m_tex_coords[3] = static_cast<float>(m_surface->h%m_chunk_size_h) / static_cast<float>(m_chunk_size_h);

		if (m_tex_coords[2] == 0.0f){
			m_tex_coords[2] = 1.0f;
		}

		if (m_tex_coords[3] == 0.0f){
			m_tex_coords[3] = 1.0f;
		}

		uint8_t* data = static_cast<uint8_t*>(m_surface->pixels);
		int32_t pitch = m_surface->pitch;

		assert(!m_texId);

		// get texture id from opengl
		glGenTextures(1, &m_texId);
		// set focus on that texture
		static_cast<RenderBackendOpenGL*>(RenderBackend::instance())->bindTexture(m_texId);
		// set filters for texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		bool mipmapping = RenderBackend::instance()->isMipmappingEnabled();
		if (mipmapping) {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		}
		// without mipmapping, trilinear and bilinear filters are the same
		switch (RenderBackend::instance()->getTextureFiltering()) {
			case TEXTURE_FILTER_NONE:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
				break;
			case TEXTURE_FILTER_BILINEAR:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
				break;
			case TEXTURE_FILTER_TRILINEAR:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				break;
			case TEXTURE_FILTER_ANISOTROPIC:
				// currently trilinear anisotropic
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(RenderBackend::instance()->getMaxAnisotropy()));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				break;
			default:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
				break;
		}

		GLint internalFormat = GL_RGBA8;
		if(GLEE_ARB_texture_compression && RenderBackend::instance()->isImageCompressingEnabled()) {
			internalFormat = GL_COMPRESSED_RGBA;
			m_compressed = true;
		} else {
			m_compressed = false;
		}

		bool monochrome = RenderBackend::instance()->isMonochromeEnabled();
		SDL_Surface* target = RenderBackend::instance()->getRenderTargetSurface();
		int32_t bpp_target = RenderBackend::instance()->getPixelFormat().BitsPerPixel;
		int32_t bpp_source = m_surface->format->BitsPerPixel;
		// create 16 bit texture, RGBA_4444
		if (bpp_target == 16 && bpp_source == 32) {
			uint16_t* oglbuffer = new uint16_t[m_chunk_size_w * m_chunk_size_h];
			memset(oglbuffer, 0x00, m_chunk_size_w*m_chunk_size_h*sizeof(uint16_t));

			for (uint32_t y = 0;  y < height; ++y) {
				for (uint32_t x = 0; x < width; ++x) {
					uint32_t pos = (y * pitch) + (x * 4);

					uint8_t r = data[pos + 0];
					uint8_t g = data[pos + 1];
					uint8_t b = data[pos + 2];
					uint8_t a = data[pos + 3];

					if (RenderBackend::instance()->isColorKeyEnabled()) {
						// only set alpha to zero if colorkey feature is enabled
						if (r == m_colorkey.r && g == m_colorkey.g && b == m_colorkey.b) {
							a = 0;
						}
					}
					// if monochrome rendering is enabled, then the colors are converted to grayscale
					if (monochrome) {
						uint8_t lum = static_cast<uint8_t>(r*0.3 + g*0.59 + b*0.11);
						r = lum;
						g = lum;
						b = lum;
					}
					oglbuffer[(y*m_chunk_size_w) + x] = ((r >> 4) << 12) |
														((g >> 4) << 8) |
														((b >> 4) << 4) |
														((a >> 4) << 0);
				}
			}
			// in case of compression we let OpenGL handle it
			if (!m_compressed) {
				internalFormat = GL_RGBA4;
			}

			// transfer data from sdl buffer
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_chunk_size_w, m_chunk_size_h,
				0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, oglbuffer);

			delete[] oglbuffer;
			return;
		}
		
		if(GLEE_ARB_texture_non_power_of_two && RenderBackend::instance()->isNPOTEnabled()) {
			if(RenderBackend::instance()->isColorKeyEnabled()) {
				uint8_t* oglbuffer = new uint8_t[width * height * 4];
				memcpy(oglbuffer, data, width * height * 4 * sizeof(uint8_t));

				for (uint32_t y = 0;  y < height; ++y) {
					for (uint32_t x = 0; x < width * 4; x += 4) {
						uint32_t gid = x + y * pitch;

						uint8_t r = oglbuffer[gid + 0];
						uint8_t g = oglbuffer[gid + 1];
						uint8_t b = oglbuffer[gid + 2];

						// set alpha to zero
						if (r == m_colorkey.r && g == m_colorkey.g && b == m_colorkey.b) {
							oglbuffer[gid + 3] = 0;
						}
						// if monochrome rendering is enabled, then the colors are converted to grayscale
						if (monochrome) {
							uint8_t lum = static_cast<uint8_t>(r*0.3 + g*0.59 + b*0.11);
							oglbuffer[gid + 0] = lum;
							oglbuffer[gid + 1] = lum;
							oglbuffer[gid + 2] = lum;
						}
					}
				}

				// transfer data from sdl buffer
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_chunk_size_w, m_chunk_size_h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, oglbuffer);

				delete [] oglbuffer;
			} else if (monochrome) {
				uint8_t* oglbuffer = new uint8_t[width * height * 4];
				memcpy(oglbuffer, data, width * height * 4 * sizeof(uint8_t));

				for (uint32_t y = 0;  y < height; ++y) {
					for (uint32_t x = 0; x < width * 4; x += 4) {
						uint32_t gid = x + y * pitch;
						// if monochrome rendering is enabled, then the colors are converted to grayscale
						uint8_t lum = static_cast<uint8_t>(oglbuffer[gid + 0]*0.3 +	oglbuffer[gid + 1]*0.59 + oglbuffer[gid + 2]*0.11);
						oglbuffer[gid + 0] = lum;
						oglbuffer[gid + 1] = lum;
						oglbuffer[gid + 2] = lum;
					}
				}

				// transfer data from sdl buffer
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_chunk_size_w, m_chunk_size_h,
					0, GL_RGBA, GL_UNSIGNED_BYTE, oglbuffer);

				delete [] oglbuffer;
			} else {
				// transfer data directly from sdl buffer
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_chunk_size_w, m_chunk_size_h,
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
					// if monochrome rendering is enabled, then the colors are converted to grayscale
					if (monochrome) {
						uint8_t lum = static_cast<uint8_t>(r*0.3 + g*0.59 + b*0.11);
						r = lum;
						g = lum;
						b = lum;
					}

					oglbuffer[(y*m_chunk_size_w) + x] = r | (g << 8) | (b << 16) | (a<<24);
				}
			}

			// transfer data from sdl buffer
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_chunk_size_w, m_chunk_size_h,
				0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<GLvoid*>(oglbuffer));

			delete[] oglbuffer;
		}
	}

	void GLImage::generateGLSharedTexture(const GLImage* shared, const Rect& region) {
		uint32_t width = shared->getWidth();
		uint32_t height = shared->getHeight();

		if(!GLEE_ARB_texture_non_power_of_two || !RenderBackend::instance()->isNPOTEnabled()) {
			width = nextPow2(width);
			height = nextPow2(height);
		}

		if (RenderBackend::instance()->getTextureFiltering() != TEXTURE_FILTER_NONE || RenderBackend::instance()->isMipmappingEnabled()) {
			// half pixel correction
			m_tex_coords[0] = (static_cast<GLfloat>(region.x)+0.5) / static_cast<GLfloat>(width);
			m_tex_coords[1] = (static_cast<GLfloat>(region.y)+0.5) / static_cast<GLfloat>(height);
			m_tex_coords[2] = (static_cast<GLfloat>(region.x + region.w)-0.5) / static_cast<GLfloat>(width);
			m_tex_coords[3] = (static_cast<GLfloat>(region.y + region.h)-0.5) / static_cast<GLfloat>(height);
		} else {
			m_tex_coords[0] = static_cast<GLfloat>(region.x) / static_cast<GLfloat>(width);
			m_tex_coords[1] = static_cast<GLfloat>(region.y) / static_cast<GLfloat>(height);
			m_tex_coords[2] = static_cast<GLfloat>(region.x + region.w) / static_cast<GLfloat>(width);
			m_tex_coords[3] = static_cast<GLfloat>(region.y + region.h) / static_cast<GLfloat>(height);
		}
	}

	void GLImage::useSharedImage(const ImagePtr& shared, const Rect& region) {
		GLImage* img = static_cast<GLImage*>(shared.get());

		m_shared_img = img;
		m_texId = img->m_texId;
		m_shared = true;
		m_subimagerect = region;
		m_atlas_img = shared;
		m_surface = m_shared_img->m_surface;
		m_compressed = m_shared_img->m_compressed;
		m_atlas_name = m_shared_img->getName();

		if(m_texId) {
			generateGLSharedTexture(img, region);
		}

		setState(IResource::RES_LOADED);
	}

	void GLImage::forceLoadInternal() {
		if (m_texId == 0) {
			generateGLTexture();
		} else if (m_shared) {
			validateShared();
		}
	}

	void GLImage::validateShared() {
		// if image is valid we can return
		if (m_shared_img->m_texId && m_shared_img->m_texId == m_texId) {
			return;
		}

		if (m_shared_img->getState() == IResource::RES_NOT_LOADED) {
			m_shared_img->load();
			m_shared_img->generateGLTexture();
		} else if (!m_shared_img->m_texId) {
			m_shared_img->generateGLTexture();
		}

		m_texId = m_shared_img->m_texId;
		m_surface = m_shared_img->m_surface;
		m_compressed = m_shared_img->m_compressed;
		generateGLSharedTexture(m_shared_img, m_subimagerect);
	}

	void GLImage::copySubimage(uint32_t xoffset, uint32_t yoffset, const ImagePtr& img) {
		Image::copySubimage(xoffset, yoffset, img);

		if(m_texId) {
			static_cast<RenderBackendOpenGL*>(RenderBackend::instance())->bindTexture(m_texId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, img->getWidth(), img->getHeight(),
				GL_RGBA, GL_UNSIGNED_BYTE, img->getSurface()->pixels);
		}
	}

	void GLImage::load() {
		if (m_shared) {
			// check atlas image
			// if it does not exist, it is generated.
			if (!ImageManager::instance()->exists(m_atlas_name)) {
				ImagePtr newAtlas = ImageManager::instance()->create(m_atlas_name);
				GLImage* img = static_cast<GLImage*>(newAtlas.get());
				m_atlas_img = newAtlas;
				m_shared_img = img;
			}
			validateShared();
			// check if texture ids and surfaces are identical
			if (m_shared_img->m_surface != m_surface || m_texId != m_shared_img->m_texId) {
				m_texId = m_shared_img->m_texId;
				m_surface = m_shared_img->m_surface;
				m_compressed = m_shared_img->m_compressed;
				if (m_texId) {
					generateGLSharedTexture(m_shared_img, m_subimagerect);
				}
			}
			m_state = IResource::RES_LOADED;
		} else {
			Image::load();
		}
	}

	void GLImage::free() {
		// save the image offsets
		int32_t xshift = m_xshift;
		int32_t yshift = m_yshift;
		setSurface(NULL);
		m_xshift = xshift;
		m_yshift = yshift;
		m_state = IResource::RES_NOT_LOADED;
	}

	GLuint GLImage::getTexId() const {
		return m_texId;
	}

	const GLfloat* GLImage::getTexCoords() const {
		return m_tex_coords;
	}
}
