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
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"

#include "renderbackendsdl.h"
#include "sdlimage.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIDEO);

	SDLImage::SDLImage(IResourceLoader* loader):
		Image(loader) {
		resetSdlimage();
	}

	SDLImage::SDLImage(const std::string& name, IResourceLoader* loader):
		Image(name, loader) {
		resetSdlimage();
	}

	SDLImage::SDLImage(SDL_Surface* surface):
		Image(surface) {
		resetSdlimage();
	 }

	SDLImage::SDLImage(const std::string& name, SDL_Surface* surface):
		Image(name, surface) {
		resetSdlimage();
	 }

	SDLImage::SDLImage(const uint8_t* data, uint32_t width, uint32_t height):
		Image(data, width, height) {
		resetSdlimage();
	}

	SDLImage::SDLImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height):
		Image(name, data, width, height) {
		resetSdlimage();
	}

	void SDLImage::resetSdlimage() {
		m_colorkey = RenderBackend::instance()->getColorKey();
		m_texture = NULL;
	}

	SDLImage::~SDLImage() {
		invalidate();
	}

	void SDLImage::invalidate() {
		if (m_texture && !m_shared) {
			SDL_DestroyTexture(m_texture);
		}
		m_texture = NULL;
	}

	void SDLImage::setSurface(SDL_Surface* surface) {
		invalidate();
		reset(surface);
		resetSdlimage();
	}

	void SDLImage::render(const Rect& rect, uint8_t alpha, uint8_t const* rgb) {
		if (alpha == 0) {
			return;
		}

		SDL_Surface* target = RenderBackend::instance()->getRenderTargetSurface();
		assert(target != m_surface); // can't draw on the source surface

		if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) ||
			rect.bottom() < 0 || rect.y > static_cast<int32_t>(target->h)) {
			return;
		}

		SDL_Rect tarRect;
		tarRect.x = rect.x;
		tarRect.y = rect.y;
		tarRect.w = rect.w;
		tarRect.h = rect.h;

		Rect tmpRect = m_shared ? getSubImageRect() : getArea();
		SDL_Rect srcRect;
		srcRect.x = tmpRect.x;
		srcRect.y = tmpRect.y;
		srcRect.w = tmpRect.w;
		srcRect.h = tmpRect.h;

		SDL_Renderer* renderer = static_cast<RenderBackendSDL*>(RenderBackend::instance())->getRenderer();

		// create texture
		if (!m_texture) {
			if (!m_surface) {
				load();
			}
			m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
		}
		
		// set additonal color and alpha mods
		if (rgb) {
			SDL_SetTextureColorMod(m_texture, rgb[0], rgb[1], rgb[2]);
			SDL_SetTextureAlphaMod(m_texture, rgb[3]);
		} else {
			SDL_SetTextureColorMod(m_texture, 255, 255, 255);
			SDL_SetTextureAlphaMod(m_texture, 255);
		}
		// set render color
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);

		if (SDL_RenderCopy(renderer, m_texture, &srcRect, &tarRect) != 0) {
			throw SDLException(SDL_GetError());
		}
	}

	size_t SDLImage::getSize() {
		size_t size = 0;
		if (m_surface && !m_shared) {
			size += m_surface->h * m_surface->pitch;
		}

		return size;
	}

	void SDLImage::useSharedImage(const ImagePtr& shared, const Rect& region) {
		if(shared->getState() != IResource::RES_LOADED) {
			shared->load();
		}
		SDL_Surface* surface = shared->getSurface();
		if (!surface) {
			shared->load();
			surface = shared->getSurface();
		}
		SDLImage* image = static_cast<SDLImage*>(shared.get());
		m_texture = image->getTexture();
		if (!m_texture) {
			// create atlas texture
			SDL_Renderer* renderer = static_cast<RenderBackendSDL*>(RenderBackend::instance())->getRenderer();
			m_texture = SDL_CreateTextureFromSurface(renderer, surface);
			image->setTexture(m_texture);
		}
		setSurface(surface);
		m_shared = true;
		m_subimagerect = region;
		m_atlas_img = shared;
		m_atlas_name = shared->getName();
		setState(IResource::RES_LOADED);
	}

	void SDLImage::forceLoadInternal() {
		validateShared();
	}

	void SDLImage::validateShared() {
		if (m_atlas_name.empty()) {
			return;
		}

		if (m_atlas_img->getState() == IResource::RES_NOT_LOADED ||
			getState() == IResource::RES_NOT_LOADED) {
			load();
		}
	}

	void SDLImage::load() {
		if (!m_atlas_name.empty()) {
			// check atlas image
			// if it does not exist, it is created.
			if (!ImageManager::instance()->exists(m_atlas_name)) {
				ImagePtr newAtlas = ImageManager::instance()->create(m_atlas_name);
				m_atlas_img = newAtlas;
			}
			useSharedImage(m_atlas_img , m_subimagerect);
		} else {
			Image::load();
		}
	}

	void SDLImage::free() {
		// save the image offsets
		int32_t xshift = m_xshift;
		int32_t yshift = m_yshift;
		setSurface(NULL);
		m_xshift = xshift;
		m_yshift = yshift;
		m_state = IResource::RES_NOT_LOADED;
	}

	SDL_Texture* SDLImage::getTexture() {
		return m_texture;
	}
	
	void SDLImage::setTexture(SDL_Texture* texture) {
		if (m_texture == texture) {
			return;
		}
		if (m_texture && !m_shared) {
			SDL_DestroyTexture(m_texture);
		}
		m_texture = texture;
	}
}
