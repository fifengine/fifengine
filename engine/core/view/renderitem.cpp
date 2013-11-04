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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/instance.h"
#include "model/metamodel/object.h"
#include "model/metamodel/action.h"

#include "visual.h"
#include "renderitem.h"

namespace FIFE {
	const int32_t STATIC_IMAGE_NOT_INITIALIZED = -1;

	RenderItem::RenderItem(Instance* parent):
		instance(parent),
		screenpoint(),
		dimensions(),
		transparency(255),
		currentFrame(-1),
		animationOverlayImages(0),
		colorOverlays(0),
		colorOverlay(0),
		m_cachedStaticImgId(STATIC_IMAGE_NOT_INITIALIZED),
		m_cachedStaticImgAngle(0) {
	}
	
	RenderItem::~RenderItem() {
		delete animationOverlayImages;
		delete colorOverlays;
		// don't delete colorOverlay here
	}

	int32_t RenderItem::getStaticImageIndexByAngle(uint32_t angle, Instance* instance) {
		ObjectVisual* objVis = instance->getObject()->getVisual<ObjectVisual>();
		if (!objVis) {
			return STATIC_IMAGE_NOT_INITIALIZED;
		}
		if (static_cast<int32_t>(angle) != m_cachedStaticImgAngle) {
			m_cachedStaticImgId = STATIC_IMAGE_NOT_INITIALIZED;
		}
		if (objVis->isColorOverlay()) {
			colorOverlay = objVis->getStaticColorOverlay(angle);
		}
		if (m_cachedStaticImgId != STATIC_IMAGE_NOT_INITIALIZED) {
			return m_cachedStaticImgId;
		}
		
		m_cachedStaticImgId = objVis->getStaticImageIndexByAngle(angle);
		m_cachedStaticImgAngle = angle;

		return m_cachedStaticImgId;
	}

	void RenderItem::reset() {
		instance = 0;
		dimensions = Rect();
		image.reset();
		transparency = 255;
		currentFrame = -1;
		m_cachedStaticImgId = STATIC_IMAGE_NOT_INITIALIZED;
		if (animationOverlayImages) {
			delete animationOverlayImages;
			animationOverlayImages = 0;
		}
		if (colorOverlays) {
			delete colorOverlays;
			colorOverlays = 0;
		}
		colorOverlay = 0;
	}
}
