/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

#ifndef FIFE_VIDEO_RENDERBACKENDS_SDL_SDLIMAGE_H
#define FIFE_VIDEO_RENDERBACKENDS_SDL_SDLIMAGE_H

// Standard C++ library includes

// 3rd party library includes
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"

namespace FIFE {

	/** The SDL implementation of the @c Image base class.
	 */
	class SDLImage : public Image {
	public:
		SDLImage(IResourceLoader* loader = 0);
		SDLImage(const std::string& name, IResourceLoader* loader = 0);
		SDLImage(SDL_Surface* surface);
		SDLImage(const std::string& name, SDL_Surface* surface);
		SDLImage(const uint8_t* data, uint32_t width, uint32_t height);
		SDLImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height);

		virtual ~SDLImage();
		virtual void invalidate() {}; //do nothing for SDL images (for now)
		virtual void setSurface(SDL_Surface* surface);
		virtual void render(const Rect& rect, uint8_t alpha = 255, uint8_t const* rgb = 0);
		virtual size_t getSize();
		virtual void useSharedImage(const ImagePtr& shared, const Rect& region);
		virtual void forceLoadInternal();
		virtual void load();
		virtual void free();

	private:
		// Call this before rendering
		void finalize();

		/** SDL Alpha Optimizer
		 * This tries to convert an image with a fake alpha channel
		 * to an RGB image when the channel can be reasonably be replaced
		 * by a colorkey.
		 */
		SDL_Surface* optimize(SDL_Surface* surface);

		void resetSdlimage();
		void validateShared();

		// SDLSurface used to create the SDLImage.
		Uint8 m_last_alpha;
		// Is the surface already optimized for rendering
		bool m_finalized;
		SDL_Color m_colorkey;
		// Surface for zoom
		SDL_Surface* m_zoom_surface;
		float m_scale_x;
		float m_scale_y;
		// Holds Atlas ImagePtr if this is a shared image
		ImagePtr m_atlas_img;
		// Holds Atlas Name if this is a shared image
		std::string m_atlas_name;
	};

}

#endif
