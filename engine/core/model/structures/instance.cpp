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
#include <iostream>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/logger.h"
#include "util/exception.h"
#include "util/fife_math.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/abstractpather.h"
#include "model/metamodel/action.h"
#include "model/structures/layer.h"

#include "instance.h"

namespace FIFE {
	static Logger _log(LM_INSTANCE);
	
	class ActionInfo {
	public:
		ActionInfo(AbstractPather* pather, const Location& curloc): 
			m_action(0), 
			m_target(NULL), 
			m_speed(0), 
			m_repeating(false),
			m_action_start_time(SDL_GetTicks()),
			m_prev_call_time(m_action_start_time),
			m_pather_session_id(-1),
			m_pather(pather) {}

		~ActionInfo() {
			if (m_pather_session_id == -1) {
				m_pather->cancelSession(m_pather_session_id);
			}
			resetTarget();
		}

		unsigned int currentTime() {
			return SDL_GetTicks() - m_action_start_time;
		}

		void resetTarget() {
			delete m_target;
			m_target = NULL;
		}

		// Current action, owned by object
		Action* m_action;
		// target location for ongoing movement
		Location* m_target;
		// current movement speed
		double m_speed;
		// should action be repeated? used only for non-moving actions, moving ones repeat until movement is finished
		bool m_repeating;
		// action start time (ticks)
		unsigned int m_action_start_time;
		// ticks since last call
		unsigned int m_prev_call_time;
		// current time for action processing (set by Instance::update), optimized to avoid multiple calls to GetTicks
		unsigned int m_cur_time;
		// session id for pather
		int m_pather_session_id;
		// pather
		AbstractPather* m_pather;
	};

	Instance::Instance(Object* object, const Location& location):
		m_object(object), 
		m_location(location),
		m_facinglocation(NULL),
		m_actioninfo(NULL),
		m_listeners(NULL),
		m_visual(NULL) {
	}

	Instance::~Instance() {
		delete m_actioninfo;
		delete m_listeners;
		delete m_visual;
		delete m_facinglocation;
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
		FL_WARN(_log, "Cannot remove unknown listener");
	}

	void Instance::initalizeAction(const std::string& action_name) {
		assert(m_object);
		if (m_actioninfo) {
			delete m_actioninfo;
			m_actioninfo = NULL;
		}
		m_actioninfo = new ActionInfo(m_object->getPather(), m_location);
		m_actioninfo->m_action = m_object->getAction(action_name);
		if (!m_actioninfo->m_action) {
			delete m_actioninfo;
			m_actioninfo = NULL;
			throw NotFound(std::string("action ") + action_name + " not found");
		}		
	}

	void Instance::act(const std::string& action_name, const Location& target, const double speed) {
		initalizeAction(action_name);
		m_actioninfo->m_target = new Location(target);
		m_actioninfo->m_speed = speed;
		setFacingLocation(target);
		FL_DBG(_log, LMsg("starting action ") <<  action_name << " from" << m_location << " to " << target << " with speed " << speed);
	}

	void Instance::act_here(const std::string& action_name, const Location& direction, bool repeating) {
		initalizeAction(action_name);
		m_actioninfo->m_repeating = repeating;
		setFacingLocation(direction);
	}

	void Instance::setFacingLocation(const Location& loc) {
		if (!m_facinglocation) {
			m_facinglocation = new Location(loc);
		} else {
			*m_facinglocation = loc;
		}
	}

	bool Instance::move() {
		FL_DBG(_log, "Moving...");
		// timeslice for this movement
		unsigned int timedelta  = m_actioninfo->m_cur_time - m_actioninfo->m_prev_call_time;
		FL_DBG(_log, LMsg("timedelta ") <<  timedelta << " prevcalltime " << m_actioninfo->m_prev_call_time);
		// how far we can travel
		double distance_to_travel = (static_cast<double>(timedelta) / 1000.0) * m_actioninfo->m_speed;
		FL_DBG(_log, LMsg("dist ") <<  distance_to_travel);
				
		Location nextLocation = m_location;
		m_actioninfo->m_pather_session_id = m_actioninfo->m_pather->getNextLocation(
			this, *m_actioninfo->m_target,
			distance_to_travel, nextLocation, *m_facinglocation,
			m_actioninfo->m_pather_session_id);
		m_location = nextLocation;
		ExactModelCoordinate a = nextLocation.getElevationCoordinates();
		ExactModelCoordinate b = m_actioninfo->m_target->getElevationCoordinates();
		// return if we are close enough to target to stop
		return ((ABS(a.x - b.x) < 0.1) && (ABS(a.y - b.y) < 0.1));
	}

	void Instance::update(unsigned int curticks) {
		if (!m_actioninfo) {
			return;
		}
		if (curticks == 0) {
			curticks = SDL_GetTicks();
		}
		m_actioninfo->m_cur_time = curticks;
		FL_DBG(_log, LMsg("updating instance, ticks = ") << curticks);

		if (m_actioninfo->m_target) {
			FL_DBG(_log, "action contains target for movement");
			bool movement_finished = move();
			if (movement_finished) {
				FL_DBG(_log, "movement finished");
				finalizeAction();
			}
		}
		else {
			FL_DBG(_log, "action does not contain target for movement");
			if ((curticks - m_actioninfo->m_action_start_time) >= m_actioninfo->m_action->getDuration()) {
				if (m_actioninfo->m_repeating) {
					m_actioninfo->m_action_start_time = curticks;
				} else {
					finalizeAction();
				}
			}
		}
		if (m_actioninfo) {
			m_actioninfo->m_prev_call_time = curticks;
		}
	}

	void Instance::finalizeAction() {
		FL_DBG(_log, "finalizing action");
		assert(m_actioninfo);

		Action* action = m_actioninfo->m_action;
		delete m_actioninfo;
		m_actioninfo = NULL;

		if (m_listeners) {
			std::vector<InstanceListener*>::iterator i = m_listeners->begin();
			while (i != m_listeners->end()) {
				(*i)->OnActionFinished(this, action);
				++i;
			}
		}
	}

	Action* Instance::getCurrentAction() const {
		if (m_actioninfo) {
			return m_actioninfo->m_action;
		}
		return NULL;
	}

	const Location& Instance::getTargetLocation() const {
		if (m_actioninfo) {
			return *m_actioninfo->m_target;
		}
		return m_location;
	}	
	
	double Instance::getMovementSpeed() const {
		if (m_actioninfo) {
			return m_actioninfo->m_speed;
		}
		return 0;
	}

	const Location& Instance::getFacingLocation() const {
		if (m_facinglocation) {
			return *m_facinglocation;
		}
		return m_location;
	}

	int Instance::getActionRuntime() const {
		if (m_actioninfo) {
			return SDL_GetTicks() - m_actioninfo->m_action_start_time;
		}
		return -1;
	}
}
