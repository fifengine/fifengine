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
#include <algorithm>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/layer.h"
#include "model/structures/instancetree.h"
#include "model/metamodel/object.h"
#include "pathfinder/searchspace.h"
#include "pathfinder/heuristic.h"
#include "util/math/fife_math.h"

#include "routepathersearch.h"

namespace FIFE {
	RoutePatherSearch::RoutePatherSearch(const int session_id, const Location& from, const Location& to, SearchSpace* searchSpace)
		: m_to(to), 
		  m_from(from), 
		  m_sessionId(session_id), 
		  m_searchspace(searchSpace), 
		  m_status(search_status_incomplete), 
		  m_startCoordInt(searchSpace->convertCoordToInt(from.getLayerCoordinates())),
		  m_destCoordInt(searchSpace->convertCoordToInt(to.getLayerCoordinates())),
		  m_next(0),
		  m_heuristic(Heuristic::getHeuristic(searchSpace->getLayer()->getCellGrid()->getType())) 
	{
		m_sortedfrontier.pushElement(PriorityQueue<int, float>::value_type(m_startCoordInt, 0.0f));
		int max_index = m_searchspace->getMaxIndex();
		m_spt.resize(max_index + 1, -1);
		m_sf.resize(max_index + 1, -1);
		m_gCosts.resize(max_index + 1, 0.0f);;
	}


	void RoutePatherSearch::updateSearch() {
		if(m_sortedfrontier.empty()) {
			setSearchStatus(search_status_failed);
			return;
		}
		PriorityQueue<int, float>::value_type topvalue = m_sortedfrontier.getPriorityElement();
		m_sortedfrontier.popElement();
		m_next = topvalue.first;
		m_spt[m_next] = m_sf[m_next];
		ModelCoordinate destCoord = m_to.getLayerCoordinates();
		if(m_destCoordInt == m_next) {
			setSearchStatus(search_status_complete);
			return;
		}
		//use destination layer for getting the cell coordinates for now, this should be moved
		//into search space.
		ModelCoordinate nextCoord = m_searchspace->convertIntToCoord(m_next);
		std::vector<ModelCoordinate> adjacents;
		m_searchspace->getLayer()->getCellGrid()->getAccessibleCoordinates(nextCoord, adjacents);
		for(std::vector<ModelCoordinate>::iterator i = adjacents.begin(); i != adjacents.end(); ++i) {
			//first determine if coordinate is in search space.
			Location loc;
			loc.setLayer(m_searchspace->getLayer());
			loc.setLayerCoordinates((*i));
			int adjacentInt = m_searchspace->convertCoordToInt((*i));
			if(m_searchspace->isInSearchSpace(loc)) {
				if((adjacentInt == m_next || loc.getLayer()->cellContainsBlockingInstance(loc.getLayerCoordinates())) &&
					adjacentInt != m_destCoordInt) {
					continue;
				}
                                	float hCost = m_heuristic->calculate((*i), destCoord);
				//float hCost = Heuristic::getHeuristic(m_searchspace->getLayer()->getCellGrid()->getType())->calculate((*i), destCoord);
				float gCost = m_gCosts[m_next] + loc.getLayer()->getCellGrid()->getAdjacentCost(nextCoord, (*i));
				if(m_sf[adjacentInt] == -1) {
					m_sortedfrontier.pushElement(PriorityQueue<int, float>::value_type(adjacentInt, gCost + hCost));
					m_gCosts[adjacentInt] = gCost;
					m_sf[adjacentInt] = m_next;
				}
				else if(gCost < m_gCosts[adjacentInt] && m_spt[adjacentInt] == -1) {
					m_sortedfrontier.changeElementPriority(adjacentInt, gCost + hCost);
					m_gCosts[adjacentInt] = gCost;
					m_sf[adjacentInt] = m_next;
				}
			} 
		}
	}

	RoutePatherSearch::Path RoutePatherSearch::calcPath() {
		int current = m_destCoordInt;
		int end = m_startCoordInt;
		Path path;
		//This assures that the agent always steps into the center of the cell.
		Location to(m_to);
		to.setExactLayerCoordinates(FIFE::intPt2doublePt(to.getLayerCoordinates()));
		path.push_back(to);
		while(current != end) {
                        if(m_spt[current] < 0 ) {
                             // This is when the size of m_spt can not handle the distance of the location
                             setSearchStatus(search_status_failed);
                             break;
                        }
                        current = m_spt[current];
			Location newnode;
			newnode.setLayer(m_searchspace->getLayer());
			ModelCoordinate currentCoord = m_searchspace->convertIntToCoord(current);
			newnode.setLayerCoordinates(currentCoord);
			path.push_front(newnode);
                }
		path.front().setExactLayerCoordinates(m_from.getExactLayerCoordinates());
		return path;
	}
}
