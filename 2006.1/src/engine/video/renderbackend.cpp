/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
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

#include "renderbackend.h"
#include "rendermanager.h"

namespace FIFE {

	static SDL_PixelFormat SDL_PXF;
	RenderBackend::RenderBackend(const std::string& name) : m_name(name)/*, m_gcn_graphics(0), m_gcn_imgloader(0)*/ {
		// get the pixelformat we want
		SDL_Surface* testsurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 1, 1, 32,
				0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

		SDL_PXF = *(testsurface->format);
		SDL_FreeSurface(testsurface);
	}


	RenderBackend::~RenderBackend() {
	}

	const std::string& RenderBackend::getName() const {
		return m_name;
	}

	Image* FIFE::RenderBackend::createStaticImageFromSDL(SDL_Surface* surface, bool freesurface) {
		SDL_Surface* conv = SDL_ConvertSurface(surface, &SDL_PXF, SDL_SWSURFACE | SDL_SRCALPHA);
		if (freesurface)
			SDL_FreeSurface(surface);

		SDL_LockSurface(conv);
		Image* img = createStaticImageFromRGBA(static_cast<const uint8_t*>(conv->pixels), conv->w, conv->h);
		SDL_UnlockSurface(conv);
		SDL_FreeSurface(conv);
		return img;
	}

	void RenderBackend::captureScreen(const std::string& filename) {
		// Dummy Implementation
	}
}
