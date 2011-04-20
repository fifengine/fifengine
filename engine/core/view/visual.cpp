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
#include "util/log/logger.h"
#include "util/base/exception.h"

#include "model/structures/instance.h"
#include "model/metamodel/object.h"
#include "model/metamodel/action.h"

#include "visual.h"


namespace FIFE {

	static Logger _log(LM_VIEW);

	Visual2DGfx::Visual2DGfx(): m_transparency(0), m_visible(true) {
	}

	Visual2DGfx::~Visual2DGfx() {
	}

	ObjectVisual::ObjectVisual() {
	}

	ObjectVisual* ObjectVisual::create(Object* object) {
		if (object->getVisual<ObjectVisual>()) {
			throw Duplicate("Object already contains visualization");
		}
		ObjectVisual* v = new ObjectVisual();
		object->adoptVisual(v);
		return v;
	}

	ObjectVisual::~ObjectVisual() {
	}

	void ObjectVisual::addStaticImage(uint32_t angle, int32_t image_index) {
		m_angle2img[angle % 360] = image_index;
	}

	int32_t ObjectVisual::getStaticImageIndexByAngle(int32_t angle) {
		int32_t closestMatch = 0;
		return getIndexByAngle(angle, m_angle2img, closestMatch);
	}

	int32_t ObjectVisual::getClosestMatchingAngle(int32_t angle) {
		int32_t closestMatch = 0;
		getIndexByAngle(angle, m_angle2img, closestMatch);
		return closestMatch;
	}

	void ObjectVisual::getStaticImageAngles(std::vector<int32_t>& angles) {
		angles.clear();
		type_angle2id::const_iterator i(m_angle2img.begin());
		while (i != m_angle2img.end()) {
			angles.push_back(i->first);
			++i;
		}
	}

	InstanceVisual::InstanceVisual():
		m_stackposition(0) {
	}

	InstanceVisual* InstanceVisual::create(Instance* instance) {
		if (instance->getVisual<InstanceVisual>()) {
			throw Duplicate("Instance already contains visualization");
		}
		InstanceVisual* v = new InstanceVisual();
		instance->setVisual(v);
		return v;
	}

	InstanceVisual::~InstanceVisual() {
	}

	ActionVisual::ActionVisual(): m_animation_map(), m_map() {
	}

	ActionVisual* ActionVisual::create(Action* action) {
		if (action->getVisual<ActionVisual>()) {
			throw Duplicate("Action already contains visualization");
		}
		ActionVisual* v = new ActionVisual();
		action->adoptVisual(v);
		return v;
	}

	ActionVisual::~ActionVisual() {
	}

	void ActionVisual::addAnimation(uint32_t angle, AnimationPtr animationptr) {
		m_animation_map[angle % 360] = animationptr;
		m_map[angle % 360] = angle % 360;
	}

	AnimationPtr ActionVisual::getAnimationByAngle(int32_t angle) {
		int32_t closestMatch = 0;
		return m_animation_map[getIndexByAngle(angle, m_map, closestMatch)];
	}

	void ActionVisual::getActionImageAngles(std::vector<int32_t>& angles) {
		angles.clear();
		AngleAnimationMap::const_iterator i(m_animation_map.begin());
		while (i != m_animation_map.end()) {
			angles.push_back(i->first);
			++i;
		}
	}
}
