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
		SDLImage(SDL_Surface* surface);
		SDLImage(const uint8_t* data, unsigned int width, unsigned int height);
		virtual ~SDLImage();
		void invalidate() {}; //do nothing for SDL images (for now)
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
		// Call this before rendering
		void finalize();

		/** SDL Alpha Optimizer
		 * This tries to convert an image with a fake alpha channel
		 * to an RGB image when the channel can be reasonably be replaced
		 * by an colorkey.
		 */
		SDL_Surface* optimize(SDL_Surface* surface);

		void resetSdlimage();

		// SDLSurface used to create the SDLImage.
		Uint8 m_last_alpha;
		// Is the surface already optimized for rendering
		bool m_finalized;
		SDL_Color m_colorkey;
		// Surface for zoom
		SDL_Surface* m_zoom_surface;
		float m_scale_x;
		float m_scale_y;
	};

}

#endif
