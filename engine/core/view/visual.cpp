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
#include "util/logger.h"
#include "util/exception.h"

#include "model/structures/instance.h"
#include "model/metamodel/object.h"
#include "model/metamodel/action.h"

#include "visual.h"


namespace FIFE {
	const int STATIC_IMAGE_NOT_INITIALIZED = -2;
	const int STATIC_IMAGE_NOT_FOUND = -1;


	static Logger _log(LM_VIEW);
	
	Visual2DGfx::Visual2DGfx(): m_transparency(0) {
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
	
	void ObjectVisual::addStaticImage(unsigned int angle, int image_index) {
		m_angle2img[angle % 360] = image_index;
	}
			
	int ObjectVisual::getStaticImageIndexByAngle(unsigned int angle) {
		return getIndexByAngle(angle, m_angle2img);
	}	
	
	InstanceVisual::InstanceVisual(): 
		m_stackposition(0),
		m_instance(NULL),
		m_cached_static_img_id(STATIC_IMAGE_NOT_INITIALIZED),
		m_cached_static_img_angle(0) {
	}
	
	InstanceVisual* InstanceVisual::create(Instance* instance) {
		if (instance->getVisual<InstanceVisual>()) {
			throw Duplicate("Instance already contains visualization");
		}
		InstanceVisual* v = new InstanceVisual();
		instance->setVisual(v);
		v->m_instance = instance;
		return v;
	}
	
	InstanceVisual::~InstanceVisual() {
	}
	
	int InstanceVisual::getStaticImageIndexByAngle(unsigned int angle) {
		if (static_cast<int>(angle) != m_cached_static_img_angle) {
			m_cached_static_img_id = STATIC_IMAGE_NOT_INITIALIZED;
		}
		if (m_cached_static_img_id != STATIC_IMAGE_NOT_INITIALIZED) {
			return m_cached_static_img_id;
		}
		m_cached_static_img_id = m_instance->getObject()->getVisual<ObjectVisual>()->getStaticImageIndexByAngle(angle);
 		m_cached_static_img_angle = angle;
		return m_cached_static_img_id;
	}	
	
	ActionVisual::ActionVisual(): m_animations() {
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

	void ActionVisual::addAnimation(unsigned int angle, int animation_index) {
		m_animations[angle % 360] = animation_index;
	}

	int ActionVisual::getAnimationIndexByAngle(int angle) {
		return getIndexByAngle(angle, m_animations);
	}
}
