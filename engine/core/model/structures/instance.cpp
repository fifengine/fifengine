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
#include "util/debugutils.h"
#include "util/exception.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/abstractpather.h"
#include "model/metamodel/action.h"
#include "model/structures/layer.h"

#include "instance.h"

const int STATIC_IMAGE_NOT_INITIALIZED = -2;
const int STATIC_IMAGE_NOT_FOUND = -1;

namespace FIFE {
	class ActionInfo {
	public:
		ActionInfo(AbstractPather* pather, const Location& curloc): 
			m_action(0), 
			m_nextlocations(),
			m_target(NULL), 
			m_speed(0), 
			m_repeating(false),
			m_facinglocation(curloc),
			m_action_start_time(SDL_GetTicks()),
			m_prev_call_time(m_action_start_time),
			m_pather_session_id(-1),
			m_pather(pather) {}

		~ActionInfo() {
			resetTarget();
		}

		unsigned int currentTime() {
			return SDL_GetTicks() - m_action_start_time;
		}

		std::vector<Location>& getNextLocations(const Location& startloc) {
			assert(m_target && m_pather);
			std::cout << "getting next locs from pather, loc = " << startloc << " tgt = " << *m_target << "\n";
			m_pather_session_id = m_pather->getNextLocations(startloc, 
					*m_target, m_nextlocations, m_pather_session_id);
			return m_nextlocations;
		}

		void resetTarget() {
			delete m_target;
			m_target = NULL;
		}

		// Current action, owned by object
		Action* m_action;
		// Cells where instance should be moved next
		std::vector<Location> m_nextlocations;
		// target location for ongoing movement
		Location* m_target;
		// current movement speed
		double m_speed;
		// should action be repeated? used only for non-moving actions, moving ones repeat until movement is finished
		bool m_repeating;
		// this is the direction where instance should be facing
		Location m_facinglocation;
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
		m_actioninfo(NULL),
		m_cached_static_img_id(STATIC_IMAGE_NOT_INITIALIZED),
		m_cached_static_img_angle(0),
		m_pending_actioninfo(NULL),
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

	ActionInfo* Instance::initalizeAction(const std::string& action_name) {
		assert(m_object);
		bool pend_action = (m_actioninfo && m_actioninfo->m_target);
		ActionInfo** info_to_use = &m_actioninfo;
		if (!pend_action) {
			delete m_actioninfo;
			m_actioninfo = NULL;
		} else {
			info_to_use = &m_pending_actioninfo;
		}
		delete m_pending_actioninfo;
		m_pending_actioninfo = NULL;

		(*info_to_use) = new ActionInfo(m_object->getPather(), m_location);
		(*info_to_use)->m_action = m_object->getAction(action_name);
		if (!(*info_to_use)->m_action) {
			delete (*info_to_use);
			(*info_to_use) = NULL;
			throw NotFound(std::string("action ") + action_name + " not found");
		}
		return (*info_to_use);
	}

	void Instance::act(const std::string& action_name, const Location& target, const double speed) {
		ActionInfo* a = initalizeAction(action_name);
		a->m_target = new Location(target);
		a->m_speed = speed;
		std::cout << "starting action " <<  action_name << " to " << target << " with speed " << speed <<"\n";		
		std::cout << "m_target " <<  *a->m_target << "\n";
	}

	void Instance::act(const std::string& action_name, const Location& direction, bool repeating) {
		ActionInfo* a = initalizeAction(action_name);
		a->m_repeating = repeating;
		a->m_facinglocation = direction;
	}

	bool Instance::move() {
		std::cout << "moving...\n";
		
		// in case we have a request to do something else
		if (m_pending_actioninfo) {
			std::cout << "some other action is pending\n";
			// Movement must be finalized on the center of a cell
			
			// if we are in the center of the cell, stop
			if (m_location.getCellOffsetDistance() == 0) {
				std::cout << "in the center of the cell\n";
				m_actioninfo->m_nextlocations.clear();
				m_actioninfo->resetTarget();
				return true;
			} 
			// otherwise set the movement target to be the current cell so that 
			// movement stops asap
			else {
				std::cout << "not in the center of the cell\n";
				*m_actioninfo->m_target = m_location;
				m_actioninfo->m_nextlocations.clear();
			}
		}
		// still doing the same movement, get next locations from pather
		else {
			std::cout << "still moving, getting locations from pather\n";
			m_actioninfo->getNextLocations(m_location);
		}
		// calculate next locations
		calcMovement();
		m_actioninfo->m_facinglocation = *m_actioninfo->m_target;
		return false;
	}


	void Instance::calcMovement() {
		unsigned int timedelta    = m_actioninfo->m_cur_time - m_actioninfo->m_prev_call_time;
		double distance_to_travel = (static_cast<float>(timedelta) / 1000.0) * m_actioninfo->m_speed;
		DoublePoint iter_loc      = m_location.getExactLayerCoordinates();
		double cumul_dist         = 0;
		std::cout << "calculating movement, dist to travel " << distance_to_travel << "\n";
		std::cout << "current location = " << m_location;
		
		
		// cumulate distance based on cell distances (cells returned from pathfinder)
		std::vector<Location>::iterator i = m_actioninfo->m_nextlocations.begin();
		while (i != m_actioninfo->m_nextlocations.end()) {
			std::cout << "next location from pather...\n";
			DoublePoint nextcell = (*i).getExactLayerCoordinates();
			std::cout << "next coordinates = " << nextcell << "\n";
			DoublePoint diff = nextcell - iter_loc;
			double dist_to_next_cell = diff.length();
			double dist_left = distance_to_travel - cumul_dist;
			std::cout << "diff = " << diff << ", dist_to_next = " << dist_to_next_cell << ", dist_left=" << dist_left<<"\n";
			
			// if we cannot reach the next cell...
			if (dist_left < dist_to_next_cell) {
				std::cout << "cannot reach the next cell...\n";
				// calculate next location using available distance
				iter_loc = iter_loc + (diff * (dist_left / dist_to_next_cell));
				cumul_dist += dist_left / dist_to_next_cell;
			}
			// otherwise iterate to the next cell center point
			else {
				std::cout << "cell reached, go to the center...\n";
				iter_loc = nextcell;
				cumul_dist += dist_to_next_cell;
				std::cout << "iter_loc = " << nextcell << ", cumul_dist = " << cumul_dist << "\n";
			}
			++i;
		}
		// move to point where iteration got us
		m_location.setExactLayerCoordinates(iter_loc);
		std::cout << "moving to next point : " << m_location << "\n";
		
		// if there is still "speed" left from traveling
		if ((distance_to_travel - cumul_dist) > 0) {
			std::cout << "still speed left\n";
			// if we are not in the target cell, pather cannot keep up
			if (*(m_actioninfo->m_target) != m_location) {
				std::cout << "pather cannot keep up\n";
			}
			// otherwise we are finished the movement
		}
	}

	void Instance::update(unsigned int curticks) {
		if (!m_actioninfo) {
			return;
		}
		if (curticks == 0) {
			curticks = SDL_GetTicks();
		}
		m_actioninfo->m_cur_time = curticks;
		std::cout << "updating instance, ticks = " << curticks << "\n";

		if (m_actioninfo->m_target) {
			std::cout << "action contains target for movement\n";
			bool movement_finished = move();
			if (movement_finished) {
				std::cout << "movement finished\n";
				finalizeAction();
				if (m_pending_actioninfo) {
					m_actioninfo = m_pending_actioninfo;
					m_pending_actioninfo = NULL;
				}
			}
		}
		else {
			std::cout << "action does not contain target for movement\n";
			if ((curticks - m_actioninfo->m_action_start_time) >= m_actioninfo->m_action->getDuration()) {
				if (m_actioninfo->m_repeating) {
					m_actioninfo->m_action_start_time = curticks;
				} else {
					finalizeAction();
				}
			}
		}
		if( m_actioninfo ) {
			m_actioninfo->m_prev_call_time = curticks;
		}
	}

	void Instance::finalizeAction() {
		std::cout << "finalizing action\n";
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

	std::vector<Location>* Instance::getNextLocations() const {
		if (m_actioninfo) {
			return &m_actioninfo->m_nextlocations;
		}
		return NULL;
	}

	const Location& Instance::getFacingLocation() const {
		if (m_actioninfo) {
			return m_actioninfo->m_facinglocation;
		}
		return m_location;
	}

	int Instance::getActionRuntime() const {
		if (m_actioninfo) {
			return SDL_GetTicks() - m_actioninfo->m_action_start_time;
		}
		return -1;
	}

	int Instance::getStaticImageIndexByAngle(unsigned int angle) {
		if (static_cast<int>(angle) != m_cached_static_img_angle) {
			m_cached_static_img_id = STATIC_IMAGE_NOT_INITIALIZED;
		}
		if (m_cached_static_img_id != STATIC_IMAGE_NOT_INITIALIZED) {
			return m_cached_static_img_id;
		}
		m_cached_static_img_id = m_object->getStaticImageIndexByAngle(angle);
 		m_cached_static_img_angle = angle;
		return m_cached_static_img_id;
	}
}
