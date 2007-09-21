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

#include <algorithm>
#include "realtimesearch.h"
#include "realtimepather.h"
#include "pathfinder/searchspace.h"
#include "model/structures/layer.h"
#include "model/metamodel/grids/cellgrid.h"

namespace FIFE {
	RealTimeSearch::RealTimeSearch(const int session_id, const Location& from, const Location& to, AbstractPather* pather)
	: Search(session_id, from, to, pather) {
		//Store the search space here for ease of use.
		m_searchspace = ((RealTimePather*)pather)->getSearchSpace();
	}

	//TODO: Tidy up this function.
	std::vector<Location> RealTimeSearch::updateSearch() {
		//TODO: Finish this function. The actual algorithm will go inside of this function. Needs
		//cleaning up, a bit ugly as it stands.
		if(m_sortedfrontier.empty()) {
			setSearchStatus(search_status_failed);
			return std::vector<Location>();
		}
		PriorityQueue<int, float>::value_type topvalue = m_sortedfrontier.getPriorityElement();
		m_sortedfrontier.popElement();
		int next = topvalue.first;
		m_spt[next] = m_sf[next];
		ModelCoordinate destCoord = m_to.getLayerCoordinates();
		int destcoordInt = m_searchspace->convertCoordToInt(destCoord);
		if(destcoordInt == next) {
			m_status = search_status_complete;
			return calcPath();
		}
		//use destination layer for getting the cell coordinates for now, this should be moved
		//into search space.
		ModelCoordinate nextCoord = m_searchspace->convertIntToCoord(next);
		std::vector<ModelCoordinate> adjacents;
		m_to.getLayer()->getCellGrid()->getAccessibleCoordinates(nextCoord, adjacents);
		for(std::vector<ModelCoordinate>::iterator i = adjacents.begin(); i != adjacents.end(); ++i) {
			//first determine if coordinate is in search space.
			//UGLY :(
			Location loc = m_to;
			loc.setLayerCoordinates((*i));
			if(m_searchspace->isInSearchSpace(loc)) {
				float hCost = (float)((destCoord.x - i->x) + (destCoord.y - i->y));
				float gCost = m_gCosts[next] + loc.getLayer()->getCellGrid()->getAdjacentCost(nextCoord, (*i));
				int adjacentInt = m_searchspace->convertCoordToInt((*i));
				if(m_sf[adjacentInt] == -1) {
					m_sortedfrontier.pushElement(PriorityQueue<int, float>::value_type(adjacentInt, gCost + hCost));
					m_gCosts[adjacentInt] = gCost;
					m_sf[adjacentInt] = next;
				}
				else if(gCost < m_gCosts[adjacentInt] && m_spt[adjacentInt] == -1) {
					m_sortedfrontier.changeElementPriority(adjacentInt, gCost + hCost);
					m_gCosts[adjacentInt] = gCost;
					m_sf[adjacentInt] = next;
				}
			}
		}
		return std::vector<Location>();
	}

	//TODO: This function needs cleaning up to.
	std::vector<Location> RealTimeSearch::calcPath() {
		int current = m_searchspace->convertCoordToInt(m_to.getLayerCoordinates());
		int end = m_searchspace->convertCoordToInt(m_from.getLayerCoordinates());
		std::vector<Location> path;
		path.push_back(m_to);
		while(current != end) {
			current = m_spt[current];
			Location newnode(m_to);
			newnode.setLayerCoordinates(m_searchspace->convertIntToCoord(current));
			path.push_back(newnode);
		}
		std::reverse(path.begin(), path.end());
		return path;
	}
}
