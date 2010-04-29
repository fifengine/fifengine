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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/instance.h"
#include "model/metamodel/object.h"
#include "model/metamodel/action.h"
#include "util/resource/pool.h"

#include "visual.h"
#include "renderitem.h"

namespace FIFE {
	const int STATIC_IMAGE_NOT_INITIALIZED = -2;

	RenderItem::RenderItem():
		screenpoint(),
		dimensions(),
		image(NULL),
		m_cached_static_img_id(STATIC_IMAGE_NOT_INITIALIZED),
		m_cached_static_img_angle(0) {
	}

	int RenderItem::getStaticImageIndexByAngle(unsigned int angle, Instance* instance) {
		if (static_cast<int>(angle) != m_cached_static_img_angle) {
			m_cached_static_img_id = STATIC_IMAGE_NOT_INITIALIZED;
		}
		if (m_cached_static_img_id != STATIC_IMAGE_NOT_INITIALIZED) {
			return m_cached_static_img_id;
		}
		if(!instance->getObject()->getVisual<ObjectVisual>())
			return Pool::INVALID_ID;
		m_cached_static_img_id = instance->getObject()->getVisual<ObjectVisual>()->getStaticImageIndexByAngle(angle);
		m_cached_static_img_angle = angle;
		return m_cached_static_img_id;
	}
}
