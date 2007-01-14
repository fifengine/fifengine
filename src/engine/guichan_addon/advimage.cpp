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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "video/renderable.h"
#include "video/animation.h"
#include "imagecache.h"

#include "advimage.h"

namespace gcn {
#if (GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6)
	AdvImage::AdvImage(const std::string & filename) : FIFE::GCNImage() {
		cacheId = FIFE::ImageCache::instance()->addImageFromFile(filename);
#else
	AdvImage::AdvImage(const std::string& filename) : Image(filename) {
#endif
	}

	AdvImage::~AdvImage() {
	}

	FIFE::Animation* AdvImage::getImageAsAnimation() {
#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
		FIFE::RenderAble* ra = FIFE::ImageCache::instance()->getImage(cacheId);
#else
		FIFE::RenderAble* ra =  FIFE::ImageCache::instance()->getImage(
				 reinterpret_cast<size_t>(_getData()));
#endif
		if (ra->getType() == FIFE::RenderAble::RT_ANIMATION) {
			FIFE::Animation* a = dynamic_cast<FIFE::Animation*>(ra);
			return a;
		}
		return NULL;
	}


	void AdvImage::setAnimActive(bool active) {
		FIFE::Animation* a = getImageAsAnimation();
		if (!a)
			return;
		a->setIsActive(active);
	}

	void AdvImage::setAnimDirection(bool forward) {
		FIFE::Animation* a = getImageAsAnimation();
		if (!a)
			return;
		a->setDirection(forward);
	}

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
