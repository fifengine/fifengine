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
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/exception.h"
#include "model/metamodel/abstractpather.h"

#include "instance.h"

namespace FIFE { namespace model {
	class ActionInfo {
	public:
		ActionInfo(AbstractPather* pather): 
			m_action(0), 
			m_target(NULL), 
			m_speed(0), 
			m_anim_index(-1), 
			m_frame_index(-1),
			m_action_start_time(SDL_GetTicks()),
			m_nodeoffset(0,0),
			m_nextnode(),
			m_pather_session_id(-1),
			m_pather(pather) {}

		~ActionInfo() {
			delete m_target;
		}

		unsigned int currentTime() {
			return SDL_GetTicks() - m_action_start_time;
		}

		Location& getNextNode(const Location& startloc) {
			assert(m_target && m_pather);
			m_pather_session_id = m_pather->getNextNode(startloc, 
			                      *m_target, m_nextnode, m_pather_session_id);
			// no subcell calculations / speed taken into account at this point yet...
			return m_nextnode;
		}

		// Current action, owned by object
		Action* m_action;
		// target location for ongoing movement
		Location* m_target;
		// current movement speed
		float m_speed;
		// current animation index (handle to animation pool)
		int m_anim_index;
		// current frame index in current action
		int m_frame_index;
		// action start time (ticks)
		unsigned long m_action_start_time;
		// offset caused by the movement in relation to current cell centerpoint
		Point m_nodeoffset;
		// movement node
		Location m_nextnode;
		// session id for pather
		int m_pather_session_id;
		// pather
		AbstractPather* m_pather;
	};

	Instance::Instance(Object* object, const Location& location):
		m_object(object), 
		m_location(location),
		m_static_img_ind(-1),
		m_actioninfo(NULL),
		m_listeners(NULL) {
	}

	Instance::~Instance() {
		delete m_actioninfo;
		delete m_listeners;
	}

	void Instance::addListener(InstanceListener* listener) {
		if (!m_listeners) {
			m_listeners = new std::vector<InstanceListener*>();
		}
		m_listeners->push_back(listener);
	}

	void Instance::removeListener(InstanceListener* listener) {
		if (!m_listeners) {
			return;
		}
		std::vector<InstanceListener*>::iterator i = m_listeners->begin();
		while (i != m_listeners->end()) {
			if ((*i) == listener) {
				m_listeners->erase(i);
				return;
			}
			++i;
		}
		Log("Instance") << "Cannot remove unknown listener";
	}

	void Instance::act(const std::string& action_name, const Location target, const float speed) {
		act(action_name);
		m_actioninfo->m_target = new Location(target);
		m_actioninfo->m_speed = speed;
	}

	void Instance::act(const std::string& action_name) {
		assert(m_object);
		delete m_actioninfo;
		m_actioninfo = new ActionInfo(m_object->getPather());
		m_actioninfo->m_action = m_object->getAction(action_name);
		if (!m_actioninfo->m_action) {
			throw NotFound(std::string("action ") + action_name + " not found");
		}
	}

	void Instance::update() {
		if (!m_actioninfo) {
			return;
		}
		if (m_actioninfo->m_target) {
			Location& nextnode = m_actioninfo->getNextNode(m_location);
			if (m_location == nextnode) {
				finalizeAction();
			}
			else {
				m_location = nextnode;
			}
		}
		else {
			// just for testing at this point...
			finalizeAction();
		}
	}

	void Instance::finalizeAction() {
		assert(m_actioninfo);

		Action* action = m_actioninfo->m_action;
		delete m_actioninfo;
		m_actioninfo = NULL;

		std::vector<InstanceListener*>::iterator i = m_listeners->begin();
		while (i != m_listeners->end()) {
			(*i)->OnActionFinished(this, action);
			++i;
		}
	}
}}
