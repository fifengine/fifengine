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
#include <boost/bind.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/animation.h"
#include "video/imagecache.h"
#include "video/renderable.h"

#include "advimage.h"

namespace gcn {
	AdvImage::AdvImage() : FIFE::GCNImage(), m_animationlistener(0) {
		cacheId = 0;
	}
	void AdvImage::loadFromCache(const size_t imageId) {
		cacheId = imageId;
	}
	void AdvImage::loadFromFile(const std::string & filename) {
		cacheId = FIFE::ImageCache::instance()->addImageFromFile(filename);
	}
	AdvImage::AdvImage(const std::string & filename): FIFE::GCNImage(), m_animationlistener(0) {
		cacheId = FIFE::ImageCache::instance()->addImageFromFile(filename);
	}

	AdvImage::~AdvImage() {
	}

	FIFE::Animation* AdvImage::getImageAsAnimation() {
		FIFE::RenderAble* ra = FIFE::ImageCache::instance()->getImage(cacheId);
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

	void AdvImage::setAnimListener(ImageAnimationListener* animationlistener) {
		m_animationlistener = animationlistener;
		FIFE::Animation* a = getImageAsAnimation();
		if (!a) {
			return;
		}
		a->setOnEndCallback(boost::bind(&ImageAnimationListener::onAnimationEnd, animationlistener));
 	}

	void AdvImage::removeAnimListener() {
		m_animationlistener = 0;
		FIFE::Animation* a = getImageAsAnimation();
		if (!a) {
			return;
		}
		a->setOnEndCallback(NULL);
	}

	bool AdvImage::isAnimation() {
		return static_cast<bool>(getImageAsAnimation());
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
