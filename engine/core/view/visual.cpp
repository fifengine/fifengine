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
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEW);

	OverlayColors::OverlayColors() {
	}

	OverlayColors::OverlayColors(ImagePtr image):
		m_image(image) {
	}
	
	OverlayColors::OverlayColors(AnimationPtr animation):
		m_animation(animation) {
	}

	OverlayColors::~OverlayColors() {
	}

	void OverlayColors::setColorOverlayImage(ImagePtr image) {
		m_image = image;
	}

	ImagePtr OverlayColors::getColorOverlayImage() {
		return m_image;
	}

	void OverlayColors::setColorOverlayAnimation(AnimationPtr animation) {
		m_animation = animation;
	}

	AnimationPtr OverlayColors::getColorOverlayAnimation() {
		return m_animation;
	}

	void OverlayColors::changeColor(const Color& source, const Color& target) {
		std::pair<std::map<Color, Color>::iterator, bool> inserter = m_colorMap.insert(std::make_pair(source, target));
		if (!inserter.second) {
			Color& c = inserter.first->second;
			c.set(target.getR(), target.getG(), target.getB(), target.getAlpha());
		}
	}

	const std::map<Color, Color>& OverlayColors::getColors() {
		return m_colorMap;
	}

	void OverlayColors::resetColors() {
		m_colorMap.clear();
	}

	Visual2DGfx::Visual2DGfx() {
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

	void ObjectVisual::addStaticColorOverlay(uint32_t angle, const OverlayColors& colors) {
		OverlayColors t = colors;
		m_map[angle % 360] = angle % 360;
		std::pair<AngleColorOverlayMap::iterator, bool> inserter = m_colorOverlayMap.insert(std::make_pair(angle % 360, colors));
		if (!inserter.second) {
			OverlayColors tmp = colors;
			OverlayColors& c = inserter.first->second;
			c.setColorOverlayImage(tmp.getColorOverlayImage());
			
			const std::map<Color, Color>& colorMap = tmp.getColors();
			std::map<Color, Color>::const_iterator it = colorMap.begin();
			for (; it != colorMap.end(); ++it) {
				c.changeColor(it->first, it->second);
			}
		}
	}

	OverlayColors* ObjectVisual::getStaticColorOverlay(int32_t angle) {
		if (m_colorOverlayMap.empty()) {
			return 0;
		}
		int32_t closestMatch = 0;
		return &m_colorOverlayMap[getIndexByAngle(angle, m_map, closestMatch)];
	}

	void ObjectVisual::removeStaticColorOverlay(int32_t angle) {
		if (m_colorOverlayMap.empty()) {
			return;
		}
		int32_t closestMatch = 0;
		int32_t index = getIndexByAngle(angle, m_map, closestMatch);
		m_colorOverlayMap.erase(index);
		m_map.erase(index);
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
		m_transparency(0),
		m_visible(true),
		m_stackposition(0),
		m_instance(NULL) {
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

	void InstanceVisual::setTransparency(uint8_t transparency) {
		if (m_transparency != transparency) {
			m_transparency = transparency;
			m_instance->callOnTransparencyChange();
		}
	}

	uint8_t InstanceVisual::getTransparency() {
		return m_transparency;
	}

	void InstanceVisual::setVisible(bool visible) {
		if (m_visible != visible) {
			m_visible = visible;
			m_instance->callOnVisibleChange();
		}
	}

	bool InstanceVisual::isVisible() {
		return m_visible;
	}

	void InstanceVisual::setStackPosition(int32_t stackposition) {
		if (m_stackposition != stackposition) {
			m_stackposition = stackposition;
			m_instance->callOnStackPositionChange();
		}
	}

	int32_t InstanceVisual::getStackPosition() {
		return m_stackposition;
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

	void ActionVisual::addAnimationOverlay(uint32_t angle, int32_t order, AnimationPtr animationptr) {
		std::map<int32_t, AnimationPtr>& orderMap = m_animationOverlayMap[angle % 360];
		m_map[angle % 360] = angle % 360;
		orderMap.insert(std::pair<int32_t, AnimationPtr>(order, animationptr));
	}

	std::map<int32_t, AnimationPtr> ActionVisual::getAnimationOverlay(int32_t angle) {
		int32_t closestMatch = 0;
		return m_animationOverlayMap[getIndexByAngle(angle, m_map, closestMatch)];
	}

	void ActionVisual::removeAnimationOverlay(uint32_t angle, int32_t order) {
		if (m_animationOverlayMap.empty()) {
			return;
		}
		int32_t closestMatch = 0;
		AngleAnimationOverlayMap::iterator it = m_animationOverlayMap.find(getIndexByAngle(angle, m_map, closestMatch));
		if (it != m_animationOverlayMap.end()) {
			it->second.erase(order);
			if (it->second.empty()) {
				m_animationOverlayMap.erase(it);
			}
		}
	}
	
	void ActionVisual::addColorOverlay(uint32_t angle, const OverlayColors& colors) {
		m_map[angle % 360] = angle % 360;
		std::pair<AngleColorOverlayMap::iterator, bool> inserter = m_colorOverlayMap.insert(std::make_pair(angle % 360, colors));
		if (!inserter.second) {
			OverlayColors tmp = colors;
			OverlayColors& c = inserter.first->second;
			c.setColorOverlayAnimation(tmp.getColorOverlayAnimation());
			
			const std::map<Color, Color>& colorMap = tmp.getColors();
			std::map<Color, Color>::const_iterator it = colorMap.begin();
			for (; it != colorMap.end(); ++it) {
				c.changeColor(it->first, it->second);
			}
		}
	}

	OverlayColors* ActionVisual::getColorOverlay(int32_t angle) {
		if (m_colorOverlayMap.empty()) {
			return 0;
		}
		int32_t closestMatch = 0;
		int32_t index = getIndexByAngle(angle, m_map, closestMatch);
		if (m_colorOverlayMap.find(index) == m_colorOverlayMap.end()) {
			return 0;
		}
		return &m_colorOverlayMap[getIndexByAngle(angle, m_map, closestMatch)];
	}

	void ActionVisual::removeColorOverlay(int32_t angle) {
		if (m_colorOverlayMap.empty()) {
			return;
		}
		int32_t closestMatch = 0;
		int32_t index = getIndexByAngle(angle, m_map, closestMatch);
		m_colorOverlayMap.erase(index);
	}

	void ActionVisual::addColorOverlay(uint32_t angle, int32_t order, const OverlayColors& colors) {
		std::map<int32_t, OverlayColors>& orderMap = m_colorAnimationOverlayMap[angle % 360];
		m_map[angle % 360] = angle % 360;
		std::pair<std::map<int32_t, OverlayColors>::iterator, bool> inserter = orderMap.insert(std::make_pair(order, colors));
		if (!inserter.second) {
			OverlayColors tmp = colors;
			OverlayColors& c = inserter.first->second;
			c.setColorOverlayAnimation(tmp.getColorOverlayAnimation());
			
			const std::map<Color, Color>& colorMap = tmp.getColors();
			std::map<Color, Color>::const_iterator it = colorMap.begin();
			for (; it != colorMap.end(); ++it) {
				c.changeColor(it->first, it->second);
			}
		}
	}

	OverlayColors* ActionVisual::getColorOverlay(int32_t angle, int32_t order) {
		if (m_colorAnimationOverlayMap.empty()) {
			return 0;
		}

		int32_t closestMatch = 0;
		AngleColorAnimationOverlayMap::iterator it = m_colorAnimationOverlayMap.find(getIndexByAngle(angle, m_map, closestMatch));
		if (it != m_colorAnimationOverlayMap.end()) {
			std::map<int32_t, OverlayColors>::iterator sit = it->second.find(order);
			if (sit != it->second.end()) {
				return &it->second[order];
			}
		}
		return 0;
	}

	void ActionVisual::removeColorOverlay(int32_t angle, int32_t order) {
		if (m_colorAnimationOverlayMap.empty()) {
			return;
		}

		int32_t closestMatch = 0;
		AngleColorAnimationOverlayMap::iterator it = m_colorAnimationOverlayMap.find(getIndexByAngle(angle, m_map, closestMatch));
		if (it != m_colorAnimationOverlayMap.end()) {
			it->second.erase(order);
			if (it->second.empty()) {
				m_colorAnimationOverlayMap.erase(it);
			}
		}
	}

	void ActionVisual::getActionImageAngles(std::vector<int32_t>& angles) {
		angles.clear();
		type_angle2id::const_iterator i(m_map.begin());
		while (i != m_map.end()) {
			angles.push_back(i->first);
			++i;
		}
	}

	void ActionVisual::convertToOverlays(bool color) {
		bool colorOverlay = color && !m_colorOverlayMap.empty();
		type_angle2id::const_iterator it = m_map.begin();
		for (; it != m_map.end(); ++it) {
			addAnimationOverlay(it->first, 0, getAnimationByAngle(it->first));
			if (colorOverlay) {
				OverlayColors* oldC = getColorOverlay(it->first);
				if (oldC) {
					OverlayColors c = OverlayColors(*oldC);
					addColorOverlay(it->first, 0, c);
				}
			}
		}
	}
}
