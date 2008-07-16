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

#ifndef FIFE_VIDEO_RENDERBACKENDS_SDL_RENDERBACKENDSDL_H
#define FIFE_VIDEO_RENDERBACKENDS_SDL_RENDERBACKENDSDL_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"

namespace FIFE {

	/** The main class of the SDL-based renderer.
	 *
	 * @see RenderBackend
	 */
	class RenderBackendSDL : public RenderBackend {
	public:
		RenderBackendSDL();
		virtual ~RenderBackendSDL();
		const std::string& getName() const;

		void startFrame();
		void endFrame();
		void init();
		Image* createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fullscreen, const std::string& title, const std::string& icon);
		Image* createImage(const uint8_t* data, unsigned int width, unsigned int height);
		Image* createImage(SDL_Surface* surface);
 		bool putPixel(int x, int y, int r, int g, int b);
		void drawLine(const Point& p1, const Point& p2, int r, int g, int b);
		void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b);
	};

}

#endif
