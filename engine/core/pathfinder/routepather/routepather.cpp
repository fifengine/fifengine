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
#include <cassert>

#include "model/metamodel/grids/cellgrid.h"

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

	int RoutePather::getNextLocation(const Instance* instance, const Location& target, 
				double distance_to_travel, Location& nextLocation,
				Location& facingLocation, int session_id, int priority) {
		assert(instance);
		assert(instance->getLocation().getLayer() == target.getLayer());
		if(session_id != -1) {
			PathMap::iterator j = m_paths.find(session_id);
			if(j != m_paths.end()) {
				if(j->second.empty()) {
					m_paths.erase(j);
					return -1;
				} else {
					followPath(instance, j->second, distance_to_travel, nextLocation, facingLocation);
					return session_id;
				}
			} else {
				if(!sessionIdValid(session_id)) {
					//Session id is invalid.
					return -1;
				}
			}
		}
		if((instance->getLocation().getLayer() != target.getLayer() || RoutePatherSearch::cellBlocked(target))) {
			return -1;
		}
		SearchSpace* searchspace = getSearchSpace(target.getLayer());
		if(!searchspace) {
			searchspace = new SearchSpace(target.getLayer());
			addSearchSpace(searchspace);
		}
		if(searchspace->isInSearchSpace(target)) {
			session_id = m_nextFreeSessionId++;
			RoutePatherSearch* newSearch = new RoutePatherSearch(session_id, instance->getLocation(), target, searchspace);
			m_sessions.pushElement(SessionQueue::value_type(newSearch, priority));
			addSessionId(session_id);
		}
		return session_id;
	}

	void RoutePather::update() {
		int ticksleft = m_maxticks;
		while(ticksleft >= 0) {
			if(m_sessions.empty()) {
				break;
			}
			Search* priority_session = m_sessions.getPriorityElement().first;
			if(!sessionIdValid(priority_session->getSessionId())) {
				delete priority_session;
				m_sessions.popElement();
				continue;
			}
			priority_session->updateSearch();
			if(priority_session->getSearchStatus() == Search::search_status_complete) {
				Path newPath = priority_session->calcPath();
				m_paths.insert(PathMap::value_type(priority_session->getSessionId(), newPath));
				invalidateSessionId(priority_session->getSessionId());
				delete priority_session;
				m_sessions.popElement();
			} else if(priority_session->getSearchStatus() == Search::search_status_failed) {
				delete priority_session;
				m_sessions.popElement();
			}
			--ticksleft;
		}
	}

	void RoutePather::followPath(const Instance* instance, Path& path, double speed, Location& nextLocation, Location& facingLocation) {
		Location instanceLoc = instance->getLocation();
		ExactModelCoordinate instancePos = instanceLoc.getElevationCoordinates();
		ExactModelCoordinate facingPos = path.front().getElevationCoordinates();
		facingPos.x = facingPos.x + (facingPos.x - instancePos.x);
		facingPos.y = facingPos.y + (facingPos.y - instancePos.y);
		facingLocation = path.front();
		facingLocation.setElevationCoordinates(facingPos);
		ExactModelCoordinate targetPos = path.front().getElevationCoordinates();
		CellGrid* grid = instanceLoc.getLayer()->getCellGrid();
		double dx = (targetPos.x - instancePos.x) * grid->getXScale();
		double dy = (targetPos.y - instancePos.y) * grid->getYScale();
		double distance = sqrt(dx * dx + dy * dy);
		bool pop = false;
		if(speed > distance) {
			speed = distance;
			pop = true;
		}
		if(distance != 0)
		{
			instancePos.x += (dx / distance) * speed;
			instancePos.y += (dy / distance) * speed;
		} else {
			pop = true;
		}
		nextLocation.setElevationCoordinates(instancePos);
		if(pop)
		{
			path.pop_front();
		}
	}
	
	bool RoutePather::cancelSession(const int session_id) {
		if(session_id >= 0) {
			PathMap::iterator i = m_paths.find(session_id);
			if(i != m_paths.end()) {
				m_paths.erase(i);
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
