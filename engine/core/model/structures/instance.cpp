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
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/abstractpather.h"
#include "model/metamodel/action.h"
#include "model/structures/layer.h"

#include "instance.h"

namespace FIFE { namespace model {
	class ActionInfo {
	public:
		ActionInfo(AbstractPather* pather): 
			m_action(0), 
			m_nextcell(),
			m_offsetsource(),
			m_offsettarget(),
			m_offset_distance(0),
			m_target(NULL), 
			m_speed(0), 
			m_repeating(false),
			m_action_start_time(SDL_GetTicks()),
			m_pather_session_id(-1),
			m_pather(pather) {}

		~ActionInfo() {
			delete m_target;
		}

		unsigned int currentTime() {
			return SDL_GetTicks() - m_action_start_time;
		}

		Location& getNextCell(const Location& startloc) {
			assert(m_target && m_pather);
			m_pather_session_id = m_pather->getNextCell(startloc, 
			                      *m_target, m_nextcell, m_pather_session_id);
			return m_nextcell;
		}

		// Current action, owned by object
		Action* m_action;
		// Cell where instance should be moved next
		Location m_nextcell;
		// Instances move gradually from one cell to next. When this happens, instance
		// is offsetted from offset source towards offset target
		Point m_offsetsource;
		Point m_offsettarget;
		// distance of offset. Relative to layer coordinates
		double m_offset_distance;
		// target location for ongoing movement
		Location* m_target;
		// current movement speed
		float m_speed;
		// should action be repeated? used only for non-moving actions, moving ones repeat until movement is finished
		bool m_repeating;
		// In case of non-moving action, this is the direction where instance should be facing
		Point m_static_direction;
		// action start time (ticks)
		unsigned int m_action_start_time;
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

	void Instance::initalizeAction(const std::string& action_name) {
		assert(m_object);
		delete m_actioninfo;
		m_actioninfo = new ActionInfo(m_object->getPather());
		m_actioninfo->m_action = m_object->getAction(action_name);
		if (!m_actioninfo->m_action) {
			delete m_actioninfo;
			m_actioninfo = NULL;
			throw NotFound(std::string("action ") + action_name + " not found");
		}
	}

	void Instance::act(const std::string& action_name, const Location& target, const float speed) {
		initalizeAction(action_name);
		assert(m_actioninfo);
		m_actioninfo->m_target = new Location(target);
		m_actioninfo->m_speed = speed;
	}

	void Instance::act(const std::string& action_name, const Point& direction, bool repeating) {
		initalizeAction(action_name);
		m_actioninfo->m_repeating = repeating;
		m_actioninfo->m_static_direction = direction;
	}

	void Instance::update(unsigned int curticks) {
		if (!m_actioninfo) {
			return;
		}
		if (curticks == 0) {
			curticks = SDL_GetTicks();
		}
		// work in progress...
		if (m_actioninfo->m_target) {
			Location& nextcell = m_actioninfo->getNextCell(m_location);
			if ((m_location == nextcell) && (m_actioninfo->m_offset_distance == 0)) {
				finalizeAction();
			} else {
				CellGrid* cg = m_location.layer->getCellGrid();
				float speed = m_actioninfo->m_speed;
				float offset = m_actioninfo->m_offset_distance;

				// we are still moving towards current cell center point
				if (m_actioninfo->m_offsetsource != m_location.position) {
					float cur_dist = cg->getAdjacentCost(m_actioninfo->m_offsetsource, m_location.position);
					// case where we will still not reach the center point
					if ((offset + speed) < cur_dist) {
						m_actioninfo->m_offset_distance += speed;
					}
					// case where we will reach exactly the center point or go beyond
					else {
						m_actioninfo->m_offset_distance = (offset + speed) - cur_dist;
						m_actioninfo->m_offsetsource = m_location.position;
						m_actioninfo->m_offsettarget = nextcell.position;
					}
				}
				// we are moving between current cell center point towards next cell
				else {
				}
			}
		}
		else {
			if ((curticks - m_actioninfo->m_action_start_time) >= m_actioninfo->m_action->getDuration()) {
				if (m_actioninfo->m_repeating) {
					m_actioninfo->m_action_start_time = curticks;
				} else {
					finalizeAction();
				}
			}
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

	Action* Instance::getCurrentAction() {
		if (m_actioninfo) {
			return m_actioninfo->m_action;
		}
		return NULL;
	}

	float Instance::getMovementSpeed() {
		if (m_actioninfo) {
			return m_actioninfo->m_speed;
		}
		return 0;
	}

	Location* Instance::getNextCell() {
		if (m_actioninfo) {
			return &m_actioninfo->m_nextcell;
		}
		return NULL;
	}

	Point Instance::getOffsetSource() {
		if (m_actioninfo) {
			return m_actioninfo->m_offsetsource;
		}
		return m_location.position;
	}

	Point Instance::getOffsetTarget() {
		if (m_actioninfo) {
			return m_actioninfo->m_offsettarget;
		}
		return m_location.position;
	}

	double Instance::getOffsetDistance() {
		if (m_actioninfo) {
			return m_actioninfo->m_offset_distance;
		}
		return 0;
	}

	Point Instance::getStaticDirection() {
		if (m_actioninfo) {
			return m_actioninfo->m_static_direction;
		}
		return m_location.position;
	}

	int Instance::getActionRuntime() {
		if (m_actioninfo) {
			return SDL_GetTicks() - m_actioninfo->m_action_start_time;
		}
		return -1;
	}
}}
