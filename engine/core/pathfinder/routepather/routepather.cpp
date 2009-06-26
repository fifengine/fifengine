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
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"

#include "pathfinder/searchspace.h"

#include "routepather.h"
#include "routepathersearch.h"

namespace FIFE {
	void RoutePather::setMap(Map* map) {
		if(!map) {
			return;
		}
		m_map = map;
	}

	int RoutePather::makeSessionId() {
		return m_nextFreeSessionId++;
	}
	
	void RoutePather::makePlan(const Instance *instance, const Location& target, int session_id, int priority) {
		SearchSpace* searchspace = getSearchSpace(target.getLayer());
		if(!searchspace) {
			searchspace = new SearchSpace(target.getLayer());
			addSearchSpace(searchspace);
		}
		if(searchspace->isInSearchSpace(target)) {
			RoutePatherSearch* newSearch = new RoutePatherSearch(session_id, instance->getLocation(), target, searchspace);
			m_sessions.pushElement(SessionQueue::value_type(newSearch, priority));
			addSessionId(session_id);
			m_path_targets.insert(LocationMap::value_type(session_id,target));
		}
	}
	
	bool RoutePather::locationsEqual(const Location &a, const Location &b) {
		
		const ModelCoordinate a_coord = a.getLayerCoordinates();
		const ModelCoordinate b_coord = b.getLayerCoordinates();
		
		return a_coord == b_coord;
	}
	
	bool RoutePather::testStep(const Instance *instance, Path& path) {
		Location instanceLoc = instance->getLocation();
		if(!path.empty() && 
		   !locationsEqual(path.front(), instanceLoc) &&
		   instanceLoc.getLayer()->cellContainsBlockingInstance(path.front().getLayerCoordinates())) {
			const bool last_step = path.front() == path.back();
			path.clear();
			return last_step;
		}
		return true;
	}
	
	int RoutePather::getNextLocation(const Instance* instance, const Location& target, 
									 double distance_to_travel, Location& nextLocation,
									 Location& facingLocation, int session_id, int priority) {
		assert(instance);
		assert(instance->getLocation().getLayer() == target.getLayer());
		bool plan_needed = true;
		
		if(session_id != -1) {
			plan_needed = false;
			PathMap::iterator path_itor = m_paths.find(session_id);
			if(path_itor != m_paths.end()) {
				LocationMap::iterator location_itor = m_path_targets.find(session_id);
				assert(location_itor != m_path_targets.end());
				
				if(path_itor->second.empty()) {
					m_paths.erase(path_itor);
					m_path_targets.erase(location_itor);
					return -1;
				}
				
				if(!followPath(instance, path_itor->second, distance_to_travel, nextLocation, facingLocation) 
				   || !locationsEqual(location_itor->second, target)) {
					m_paths.erase(path_itor);
					m_path_targets.erase(location_itor);
					plan_needed = true;
				}
			} else if(!sessionIdValid(session_id)) {
				//Session id is invalid.
				return -1;
			}
		}
		if(plan_needed) {
			if(session_id == -1) {
				session_id = makeSessionId();
			}
			makePlan(instance, target, session_id, priority);
		}
		return session_id;
	}
	
	void RoutePather::update() {
		int ticksleft = m_maxticks;
		while(ticksleft >= 0) {
			if(m_sessions.empty()) {
				break;
			}
			RoutePatherSearch* priority_session = m_sessions.getPriorityElement().first;
			if(!sessionIdValid(priority_session->getSessionId())) {
				delete priority_session;
				m_sessions.popElement();
				continue;
			}
			priority_session->updateSearch();
			if(priority_session->getSearchStatus() == RoutePatherSearch::search_status_complete) {
				const int session_id = priority_session->getSessionId();
				Path newPath = priority_session->calcPath();
				newPath.erase(newPath.begin());
				m_paths.insert(PathMap::value_type(session_id, newPath));
				invalidateSessionId(session_id);
				delete priority_session;
				m_sessions.popElement();
			} else if(priority_session->getSearchStatus() == RoutePatherSearch::search_status_failed) {
				const int session_id = priority_session->getSessionId();
				invalidateSessionId(session_id);
				delete priority_session;
				m_sessions.popElement();
			}
			--ticksleft;
		}
	}
	
	bool RoutePather::followPath(const Instance* instance, Path& path, double speed, Location& nextLocation, Location& facingLocation) {
		Location instanceLoc = instance->getLocation();                      
		if(!testStep(instance, path)) {
			return false;
		}
		
		if(path.empty()) {
			return true;
		}
		
		ExactModelCoordinate instancePos = instanceLoc.getMapCoordinates();
		ExactModelCoordinate facingPos = path.front().getMapCoordinates();
		facingPos.x = facingPos.x + (facingPos.x - instancePos.x);
		facingPos.y = facingPos.y + (facingPos.y - instancePos.y);
		facingLocation = path.front();
		facingLocation.setMapCoordinates(facingPos);
		ExactModelCoordinate targetPos = path.front().getMapCoordinates();
		CellGrid* grid = instanceLoc.getLayer()->getCellGrid();
		double dx = (targetPos.x - instancePos.x) * grid->getXScale();
		double dy = (targetPos.y - instancePos.y) * grid->getYScale();
		double distance = sqrt(dx * dx + dy * dy);
		bool pop = false;
		if(speed > distance) {
			speed = distance;
			pop = true;
		}
		if(distance != 0) {
			instancePos.x += (dx / distance) * speed;
			instancePos.y += (dy / distance) * speed;
		} else {
			pop = true;
		}
		
		nextLocation.setMapCoordinates(instancePos);
		if(pop) {
			path.pop_front();
			if(!testStep(instance, path)) {
				return false;
			}
		}
		return true;
	}
	
	bool RoutePather::cancelSession(const int session_id) {
		if(session_id >= 0) {
			PathMap::iterator i = m_paths.find(session_id);
			if(i != m_paths.end()) {
				LocationMap::iterator j = m_path_targets.find(session_id);
				assert(j != m_path_targets.end());
				m_paths.erase(i);
				m_path_targets.erase(j);
				return true;
			} else {
				invalidateSessionId(session_id);
			}
		}
		return false;
	}
	
	void RoutePather::addSessionId(const int sessionId) {
		m_registeredSessionIds.push_back(sessionId);
	}
	
	bool RoutePather::sessionIdValid(const int sessionId) {
		for(SessionList::const_iterator i = m_registeredSessionIds.begin();
			i != m_registeredSessionIds.end();
			++i) {
			if((*i) == sessionId) {
				return true;
			}
		}
		return false;
	}
	
	bool RoutePather::invalidateSessionId(const int sessionId) {
		for(SessionList::iterator i = m_registeredSessionIds.begin();
			i != m_registeredSessionIds.end();
			++i) {
			if((*i) == sessionId) {
				m_registeredSessionIds.erase(i);
				return true;
			}
		}
		return false;
	}
	
	bool RoutePather::addSearchSpace(SearchSpace* search_space) {
		std::pair<SearchSpaceMap::iterator, bool> res = m_searchspaces.insert(SearchSpaceMap::value_type(search_space->getLayer(), search_space));
		
		return res.second;
	}
	
	SearchSpace* RoutePather::getSearchSpace(Layer * const layer) {
		SearchSpaceMap::iterator i = m_searchspaces.find(layer);
		if(i == m_searchspaces.end()) {
			return 0;
		}
		return i->second;
	}
}
