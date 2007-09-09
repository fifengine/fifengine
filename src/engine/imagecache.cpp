/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include "imagecache.h"
#include "exception.h"
#include <SDL.h>
#include "graphix_primitives.h"

namespace FIFE {

	ImageCache::ImageCache() : m_images() {}

	ImageCache::~ImageCache() {
		clearCache();
	}

	ImageCache::ImageCache(const ImageCache&) : m_images() {}

	ImageCache& ImageCache::operator=(const ImageCache&) {
		return *this;
	}

	void ImageCache::clearCache() {
		type_images::const_iterator end = m_images.end();
		for (type_images::iterator i = m_images.begin(); i != end; ++i)
			delete *i;

		m_images.clear();
	}

	size_t ImageCache::addImage(SDL_Surface* surface) {
		//SDL_Surface* osurface = SDL_DisplayFormat(surface);
		//SDL_SetColorKey(osurface, SDL_SRCCOLORKEY, 0x00);
		SDL_Surface* osurface = SDL_DisplayFormatAlpha(surface);
		SDL_FreeSurface(surface);
		Image* image = new Image();
		image->setSurface(osurface);

		m_images.push_back(image);
		return m_images.size() - 1;
	}

	Image* ImageCache::getImage(size_t index) const {
		if (m_images.size() <= index)
			throw IndexOverflow(__PRETTY_FUNCTION__);

		return *(m_images.begin() + index);
	}

	size_t ImageCache::getImageCount() const {
		return m_images.size();
	}

}
;//FIFE
