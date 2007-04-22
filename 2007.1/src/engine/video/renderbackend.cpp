/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "renderbackend.h"
#include "rendermanager.h"

namespace FIFE {
	
	RenderBackend::RenderBackend(const std::string& name) : m_name(name) {
	}


	RenderBackend::~RenderBackend() {
	}

	const std::string& RenderBackend::getName() const {
		return m_name;
	}
		
	Image* RenderBackend::createStaticImageFromRGBA(const uint8_t* data, unsigned int width, unsigned int height) {
		SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, width,height, 32,
		                                            0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
		SDL_LockSurface(surface);

		unsigned int size = width * height * 4;
		uint8_t* pixeldata = static_cast<uint8_t*>(surface->pixels);
		std::copy(data, data + size, pixeldata);
		SDL_UnlockSurface(surface);

		return createStaticImageFromSDL(surface);
	}

	void RenderBackend::captureScreen(const std::string& filename) {
		// Dummy Implementation.
	}
}
