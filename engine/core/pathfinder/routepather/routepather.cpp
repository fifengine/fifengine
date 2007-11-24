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
				Location& facingLocation, int session_id) {
		assert(instance);
		assert(instance->getLocation().getLayer() == target.getLayer());
		if(session_id != -1) {
			//This means we're updating a session.
			SessionMap::iterator i = m_sessions.find(session_id);
			if(i != m_sessions.end()) {
				i->second->updateSearch();
				if(i->second->getSearchStatus() == Search::search_status_complete) {
					Path newPath = i->second->calcPath();
					m_paths.insert(PathMap::value_type(session_id, newPath));
					m_sessions.erase(i);
				} else if(i->second->getSearchStatus() == Search::search_status_failed) {
					m_sessions.erase(i);
					return -1;
				}
				return session_id;
			} else {
				//Check to see if this session is in the movement phase.
				PathMap::iterator j = m_paths.find(session_id);
				if(j != m_paths.end()) {
					if(j->second.empty()) {
						m_paths.erase(j);
						return -1;
					} else {
						followPath(instance, j->second, distance_to_travel, nextLocation, facingLocation);
						return session_id;
					}
				}
			}
		}
		if((instance->getLocation().getLayer() != target.getLayer()) || (instance->getLocation().getLayerCoordinates() ==
			target.getLayerCoordinates()) || RoutePatherSearch::cellBlocked(target)) {
			return -1;
		}
		SearchSpaceMap::iterator i = m_searchspaces.find(target.getLayer());
		if(i == m_searchspaces.end()) {
			SearchSpace* newSearchSpace = new SearchSpace(target.getLayer());
			i = m_searchspaces.insert(SearchSpaceMap::value_type(target.getLayer(), newSearchSpace)).first;
		}
		if(i->second->isInSearchSpace(target)) {
			session_id = m_nextFreeSessionId++;
			RoutePatherSearch* newSearch = new RoutePatherSearch(session_id, instance->getLocation(), target, i->second);
			m_sessions.insert(SessionMap::value_type(session_id, newSearch));
		}
		return session_id;
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
			SessionMap::iterator i = m_sessions.find(session_id);
			if(i != m_sessions.end()) {
				delete i->second;
				m_sessions.erase(i);
				return true;
			} else {
				PathMap::iterator i = m_paths.find(session_id);
				if(i != m_paths.end()) {
					m_paths.erase(i);
					return true;
				}
			}
		}
		return false;
	}
}
