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
#include "model/metamodel/grids/cellgrid.h"

#include "linearpather.h"



namespace FIFE {
	static Logger _log(LM_PATHFINDER);
	
	int LinearPather::getNextLocation(const Instance* instance, const Location& target,
		                            double distance_to_travel, Location& nextLocation,
		                            Location& facingLocation, int session_id, int priority) {
		Location curloc = instance->getLocation();
		assert(curloc.getElevation() == target.getElevation());
		Layer* layer = curloc.getLayer();
		assert(layer == target.getLayer());
		CellGrid* cg = layer->getCellGrid();
		assert(layer == target.getLayer());
		
		assert(curloc.getLayer() == target.getLayer());
		m_map = curloc.getElevation()->getMap();
		
		int cur_session_id = session_id;
		if (cur_session_id < 0) {
			cur_session_id = m_session_counter++;
			
			// extrapolate facing location for this session
			ExactModelCoordinate cur_pos = curloc.getElevationCoordinates();
			ExactModelCoordinate fac_pos = target.getElevationCoordinates();
			fac_pos.x = fac_pos.x + (fac_pos.x - cur_pos.x);
			fac_pos.y = fac_pos.y + (fac_pos.y - cur_pos.y);
			facingLocation = target;
			facingLocation.setElevationCoordinates(fac_pos);
			m_session2face[cur_session_id] = facingLocation;
			FL_DBG(_log, LMsg("storing new facing loc ") <<  facingLocation);
		} else {
			FL_DBG(_log, LMsg("getting old facing loc ") <<  facingLocation);
			facingLocation = m_session2face[cur_session_id];
		}
		FL_DBG(_log, LMsg("curloc ") <<  curloc << ", target " << target << ", dist2travel " << distance_to_travel);
		ExactModelCoordinate cur_pos = curloc.getElevationCoordinates();
		ExactModelCoordinate target_pos = target.getElevationCoordinates();
		double dx = (target_pos.x - cur_pos.x) * cg->getXScale();
		double dy = (target_pos.y - cur_pos.y) * cg->getYScale();
		double dist = sqrt(dx*dx + dy*dy);
		FL_DBG(_log, LMsg("distance from cur to target = ") << dist);
		
		// calculate where current position evolves with movement
		if (distance_to_travel > dist) {
			distance_to_travel = dist;
		}
		cur_pos.x += dx * (distance_to_travel / dist);
		cur_pos.y += dy * (distance_to_travel / dist);
		nextLocation.setElevationCoordinates(cur_pos);
		FL_DBG(_log, LMsg("in case not blocking, could move to ") << nextLocation);
		
		// check if we have collisions and if we do, keep instance on current location
		ModelCoordinate nextCellcoord = nextLocation.getLayerCoordinates();
		const std::vector<Instance*>& instances = target.getLayer()->getInstances();
		std::vector<Instance*>::const_iterator instance_it = instances.begin();
		for (;instance_it != instances.end(); ++instance_it) {
			Instance* i = (*instance_it);
			if ((i == instance) || (!i->getObject()->isBlocking())) {
				continue;
			}
			ModelCoordinate c = i->getLocation().getLayerCoordinates();
			if ((c.x == nextCellcoord.x) && (c.y == nextCellcoord.y)) {
				FL_DBG(_log, LMsg("Found blocking instance from planned cell ") << nextLocation);
				nextLocation = curloc;
				break;
			}
		}
		//We're there
		if((ABS(cur_pos.x - target_pos.x) < 0.1) && (ABS(cur_pos.y - target_pos.y) < 0.1)) {
			return -1;
		}
		return cur_session_id;
	}
}
