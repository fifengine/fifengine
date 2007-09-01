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

const int STATIC_IMAGE_NOT_INITIALIZED = -2;
const int STATIC_IMAGE_NOT_FOUND = -1;

namespace FIFE {
	class ActionInfo {
	public:
		ActionInfo(AbstractPather* pather, const Point& curpos): 
			m_action(0), 
			m_nextcells(),
			m_offsetsource(curpos),
			m_offsettarget(curpos),
			m_offset_distance(0),
			m_target(NULL), 
			m_speed(0), 
			m_repeating(false),
			m_facingcell(curpos),
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

		std::vector<Location>& getNextCells(const Location& startloc) {
			assert(m_target && m_pather);
			m_pather_session_id = m_pather->getNextCells(startloc, 
			                      *m_target, m_nextcells, m_pather_session_id);
			return m_nextcells;
		}

		void resetTarget() {
			delete m_target;
			m_target = NULL;
		}

		// Current action, owned by object
		Action* m_action;
		// Cells where instance should be moved next
		std::vector<Location> m_nextcells;
		// Instances move gradually from one cell to next. When this happens, instance
		// is offsetted from offset source towards offset target
		Point m_offsetsource;
		Point m_offsettarget;
		// distance from m_offsetsource. Relative to layer coordinates
		double m_offset_distance;
		// current offset starting from source
		double m_cur_offset;
		// target location for ongoing movement
		Location* m_target;
		// current movement speed
		double m_speed;
		// should action be repeated? used only for non-moving actions, moving ones repeat until movement is finished
		bool m_repeating;
		// this is the direction where instance should be facing
		Point m_facingcell;
		// action start time (ticks)
		unsigned int m_action_start_time;
		// ticks since last call
		unsigned int m_prev_call_time;
		// current time for action processing (set by Instance::update)
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

		(*info_to_use) = new ActionInfo(m_object->getPather(), m_location.position);
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
	}

	void Instance::act(const std::string& action_name, const Point& direction, bool repeating) {
		ActionInfo* a = initalizeAction(action_name);
		a->m_repeating = repeating;
		a->m_facingcell = direction;
	}

	/**
	 * The following subcell movement rules apply in CellGrid space
	 * - If instance is in the cell center
	 *    - If instance is asked to move
	 *       - Move towards next cell using given speed (start subcell movement)
	 *    - Else
	 *       - Stay in the current location
	 * - Else If instance is still in the originating cell but not in its center
	 *    - If instance is asked to stop
	 *       - Move towards the center of current cell
	 *    - Else If current subcell target is the same as current movement target
	 *       - Move towards the center of target cell
	 *    - Else (current subcell target is different than current movement target)
	 *       - Move towards the center of current cell
	 * - Else (instance has arrived to the area of next cell, but not in its center yet)
	 *    - If instance is asked to stop
	 *       - Move towards the center of current cell
	 *    - Else If current subcell target is the same as current movement target
	 *       - Move towards the center of current cell
	 *    - Else If current subcell target is different than movement target
	 *       - Move towards the center of current cell
	 * Same rules should apply also in case of speeds exceeding single cell distances
	 * As seen from action, rules can be simplified but listed for completeness
	 */
	bool Instance::move() {
		bool finished = false;
		if (m_actioninfo->m_offset_distance == 0) {
			if (m_pending_actioninfo) {
				// take care of stopping
				m_actioninfo->m_offsetsource = m_location.position;
				m_actioninfo->m_offsettarget = m_location.position;
				m_actioninfo->m_cur_offset = 0;
				m_actioninfo->m_nextcells.clear();
				m_actioninfo->resetTarget();
				finished = true;
			} else {
				calcMovement();
			}
		} else if (m_location.position == m_actioninfo->m_offsetsource) {
			if (m_pending_actioninfo) {
				// switch the movement direction to return to the center point
				Point tmp = m_actioninfo->m_offsetsource;
				m_actioninfo->m_offsetsource = m_actioninfo->m_offsettarget;
				m_actioninfo->m_offsettarget = tmp;
				m_actioninfo->m_cur_offset = m_actioninfo->m_offset_distance - m_actioninfo->m_cur_offset;
				m_actioninfo->m_nextcells.clear();
				m_actioninfo->m_target->position = m_location.position;
			} 
			calcMovement();
		} else {
			calcMovement();
		}
		return finished;
	}


	void Instance::calcMovement() {
		m_actioninfo->getNextCells(m_location);

		CellGrid* cg              = m_location.layer->getCellGrid();
		unsigned int timedelta    = m_actioninfo->m_cur_time - m_actioninfo->m_prev_call_time;
		double distance_to_travel = (static_cast<float>(timedelta) / 1000.0) * m_actioninfo->m_speed;
		double dist_to_first_cell = m_actioninfo->m_offset_distance - m_actioninfo->m_cur_offset;
		double cumul_dist         = dist_to_first_cell;
		Point* prev               = &m_actioninfo->m_offsetsource;
		Point* next               = &m_actioninfo->m_offsettarget;
		double next_offset_dist   = m_actioninfo->m_offset_distance;
		bool moved                = false;

		// cumulate distance based on cell distances (cells returned from pathfinder)
		// take offset cases into consideration. There might be offsets before and
		// after centerpoints of each cell
		std::vector<Location>::iterator i = m_actioninfo->m_nextcells.begin();
		while (i != m_actioninfo->m_nextcells.end()) {
			if (cumul_dist > distance_to_travel) {
				// if cell boundary has changed
				if (prev != &m_actioninfo->m_offsetsource) {
					m_actioninfo->m_offsetsource = *prev;
					m_actioninfo->m_offsettarget = *next;
					m_actioninfo->m_offset_distance = next_offset_dist;
					double distance_to_last_cell = cumul_dist - dist_to_first_cell - distance_to_travel;
					m_actioninfo->m_cur_offset = next_offset_dist - distance_to_last_cell;
					if (m_actioninfo->m_cur_offset > (m_actioninfo->m_offset_distance / 2)) {
						m_location.position = m_actioninfo->m_offsettarget;
					} else {
						m_location.position = m_actioninfo->m_offsetsource;
					}
				}
				// still between same cells
				else {
					m_actioninfo->m_cur_offset = m_actioninfo->m_cur_offset + distance_to_travel;
				}
				moved = true;
				break;
			}
			prev = next;
			next = &(*i).position;
			next_offset_dist = cg->getAdjacentCost(*prev, *next);
			m_actioninfo->m_facingcell = *next;
			cumul_dist += next_offset_dist;
			++i;
		}
		if (!moved) {
			// two reasons to get there. 1) speed was so large that pathfinder
			// didn't have time to calculate. In that case go to last cell
			// received from pather. 2) there were no cells received from pathfinder,
			// meaning that we already are in the correct cell. Proceed towards the centerpoint
			if (m_actioninfo->m_nextcells.size() > 0) {
				m_actioninfo->m_offsetsource = m_actioninfo->m_nextcells.back().position;
				m_actioninfo->m_offsettarget = m_actioninfo->m_offsetsource;
				m_actioninfo->m_offset_distance = 0;
				m_actioninfo->m_cur_offset = 0;
				m_location.position = m_actioninfo->m_offsetsource;
			} else {
				m_actioninfo->m_cur_offset = m_actioninfo->m_cur_offset + distance_to_travel;
				if (m_actioninfo->m_offset_distance < m_actioninfo->m_cur_offset) {
					m_actioninfo->m_offsetsource = m_location.position;
					m_actioninfo->m_offsettarget = m_location.position;
					m_actioninfo->m_offset_distance = 0;
					m_actioninfo->m_cur_offset = 0;
				}
			}
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

		if (m_actioninfo->m_target) {
			bool movement_finished = move();
			if (movement_finished) {
				finalizeAction();
			}
			if (m_pending_actioninfo) {
				m_actioninfo = m_pending_actioninfo;
				m_pending_actioninfo = NULL;
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
		if( m_actioninfo ) {
			m_actioninfo->m_prev_call_time = curticks;
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

	double Instance::getMovementSpeed() {
		if (m_actioninfo) {
			return m_actioninfo->m_speed;
		}
		return 0;
	}

	std::vector<Location>* Instance::getNextCells() {
		if (m_actioninfo) {
			return &m_actioninfo->m_nextcells;
		}
		return NULL;
	}

	Point Instance::getFacingCell() {
		if (m_actioninfo) {
			return m_actioninfo->m_facingcell;
		}
		return m_location.position;
	}

	int Instance::getActionRuntime() {
		if (m_actioninfo) {
			return SDL_GetTicks() - m_actioninfo->m_action_start_time;
		}
		return -1;
	}

	int Instance::getStaticImageId() {
		if (m_cached_static_img_id != STATIC_IMAGE_NOT_INITIALIZED) {
			return m_cached_static_img_id;
		}
		Object* o = m_object;
		m_cached_static_img_id = STATIC_IMAGE_NOT_FOUND;
		while (o != NULL) {
			m_cached_static_img_id = o->getStaticImageId();
			if (m_cached_static_img_id != STATIC_IMAGE_NOT_FOUND) {
				return m_cached_static_img_id;
			}
		}
		return m_cached_static_img_id;
	}

	DoublePoint Instance::getExactPosition() {
		if (m_actioninfo) {
			if (m_actioninfo->m_offset_distance > 0) {
				assert(m_location.layer);
				CellGrid* cg = m_location.layer->getCellGrid();
				assert(cg);
				return cg->getOffset(m_actioninfo->m_offsetsource, 
						     m_actioninfo->m_offsettarget, 
						     m_actioninfo->m_offset_distance);
			} 
		}
		return intPt2doublePt(m_location.position);
	}
}
