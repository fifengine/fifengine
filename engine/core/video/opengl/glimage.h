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

	/** Implements an Image using OpenGL.
	 *
	 * This class contains a texture handle bound to the data given to the constructor;
	 * it draws a textured Quad to the screen.
	 *
	 * @see Image
	 * @note Width and height are not limited to powers of two; non-power of two images will be converted internally if they are not supported by the hardware (GLEE_ARB_texture_non_power_of_two).
	 * @todo Check the correctness of the generateTexture function on big endian systems (ppc)
	 */
	class GLImage : public Image {
	public:
		GLImage(IResourceLoader* loader = 0);
		GLImage(const std::string& name, IResourceLoader* loader = 0);
		GLImage(SDL_Surface* surface);
		GLImage(const std::string& name, SDL_Surface* surface);
		GLImage(const uint8_t* data, uint32_t width, uint32_t height);
		GLImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height);

		virtual ~GLImage();
		virtual void invalidate();
		virtual void setSurface(SDL_Surface* surface);
		virtual void render(const Rect& rect, uint8_t alpha = 255, uint8_t const* rgb = 0);
		virtual void render(const Rect& rect, const ImagePtr& overlay, uint8_t alpha = 255, uint8_t const* rgb = 0);

		virtual void renderZ(const Rect& rect, float vertexZ, uint8_t alpha = 255, uint8_t const* rgb = 0);
		virtual void renderZ(const Rect& rect, float vertexZ, const ImagePtr& overlay, uint8_t alpha = 255, uint8_t const* rgb = 0);

		virtual void useSharedImage(const ImagePtr& shared, const Rect& region);
		virtual void forceLoadInternal();
		virtual void copySubimage(uint32_t xoffset, uint32_t yoffset, const ImagePtr& img);
		virtual void load();
		virtual void free();

		GLuint getTexId() const;
		const GLfloat* getTexCoords() const;
		bool isCompressed() const { return m_compressed; }
		void setCompressed(bool compressed) { m_compressed = compressed; }
	private:
		// texture coords to use
		GLfloat m_tex_coords[4];

		// Was this image compressed by OpenGL driver during loading ?
		bool m_compressed;

		//     [0]    [2]    ->(x)
		// [1]  +------+
		//      |      |
		//      |      |
		// [3]  +------+
		//
		//      |
		//      v
		//     (y)
		// To map these indices with previous one:
		// [0]:=[1]:=0.0f, [2]:=m_col_tex_coords, [3]:=m_row_tex_coords


		/** Holds texture ids that are used to access textures in GL rendering context
		 */
		GLuint m_texId;

		/** Frees allocated memory and calls resetGlImage
		 */
		void cleanup();

		/** Resets GLImage variables
		 */
		void resetGlimage();

		/** Generates the GL Texture for use when rendering.
		 */
		void generateGLTexture();
		void generateGLSharedTexture(const GLImage* shared, const Rect& region);
		void validateShared();

		uint32_t m_chunk_size_w;
		uint32_t m_chunk_size_h;

		SDL_Color m_colorkey;

		GLImage* m_shared_img;
		// Holds Atlas ImagePtr if this is a shared image
		ImagePtr m_atlas_img;
		// Holds Atlas Name if this is a shared image
		std::string m_atlas_name;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
