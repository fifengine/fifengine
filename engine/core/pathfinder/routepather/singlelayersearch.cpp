/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
#include "model/structures/cellcache.h"
#include "model/structures/cell.h"
#include "pathfinder/route.h"
#include "util/math/fife_math.h"

#include "singlelayersearch.h"

namespace FIFE {
	SingleLayerSearch::SingleLayerSearch(Route* route, const int32_t sessionId):
		RoutePatherSearch(route, sessionId),
		m_to(route->getEndNode()),
		m_from(route->getStartNode()),
		m_cellCache(m_from.getLayer()->getCellCache()),
		m_startCoordInt(m_cellCache->convertCoordToInt(m_from.getLayerCoordinates())),
		m_destCoordInt(m_cellCache->convertCoordToInt(m_to.getLayerCoordinates())),
		m_next(0) {

		m_sortedfrontier.pushElement(PriorityQueue<int32_t, double>::value_type(m_startCoordInt, 0.0));
		int32_t max_index = m_cellCache->getMaxIndex();
		m_spt.resize(max_index, -1);
		m_sf.resize(max_index, -1);
		m_gCosts.resize(max_index, 0.0);
	}

	SingleLayerSearch::~SingleLayerSearch() {
	}

	void SingleLayerSearch::updateSearch() {
		if(m_sortedfrontier.empty()) {
			setSearchStatus(search_status_failed);
			m_route->setRouteStatus(ROUTE_FAILED);
			return;
		}

		PriorityQueue<int32_t, double>::value_type topvalue = m_sortedfrontier.getPriorityElement();
		m_sortedfrontier.popElement();
		m_next = topvalue.first;
		m_spt[m_next] = m_sf[m_next];
		// found destination
		if (m_destCoordInt == m_next) {
			setSearchStatus(search_status_complete);
			m_route->setRouteStatus(ROUTE_SEARCHED);
			return;
		}

		ModelCoordinate destCoord = m_to.getLayerCoordinates();
		ModelCoordinate nextCoord = m_cellCache->convertIntToCoord(m_next);
		CellGrid* grid = m_cellCache->getLayer()->getCellGrid();
		Cell* nextCell = m_cellCache->getCell(nextCoord);
		if (!nextCell) {
			return;
		}
		int32_t cellZ = nextCell->getLayerCoordinates().z;
		int32_t maxZ = m_route->getZStepRange();
		bool zLimited = maxZ != -1;
		uint8_t blockerThreshold = m_ignoreDynamicBlockers ? 2 : 1;
		bool limitedArea = m_route->isAreaLimited();
		const std::vector<Cell*>& adjacents = nextCell->getNeighbors();
		for (std::vector<Cell*>::const_iterator i = adjacents.begin(); i != adjacents.end(); ++i) {
			if (*i == NULL) {
				continue;
			}
			if ((*i)->getLayer()->getCellCache() != m_cellCache) {
				continue;
			}
			int32_t adjacentInt = (*i)->getCellId();
			if (m_sf[adjacentInt] != -1 && m_spt[adjacentInt] != -1) {
				continue;
			}
			if (zLimited && ABS(cellZ-(*i)->getLayerCoordinates().z) > maxZ) {
				continue;
			}
			bool blocker = (*i)->getCellType() > blockerThreshold;
			ModelCoordinate adjacentCoord = (*i)->getLayerCoordinates();
			if ((adjacentInt == m_next || blocker) && adjacentInt != m_destCoordInt) {
				if (!blocker && m_multicell) {
					continue;
				} else if (!m_multicell){
					continue;
				}
			}
			// search if there are blockers which could block multicell object
			if (m_multicell) {
				blocker = false;
				Location currentLoc(nextCell->getLayer());
				currentLoc.setLayerCoordinates(nextCell->getLayerCoordinates());
				Location adjacentLoc((*i)->getLayer());
				adjacentLoc.setLayerCoordinates((*i)->getLayerCoordinates());

				int32_t rotation = getAngleBetween(currentLoc, adjacentLoc);
				std::vector<ModelCoordinate> coords = grid->toMultiCoordinates(adjacentLoc.getLayerCoordinates(), m_route->getOccupiedCells(rotation));
				std::vector<ModelCoordinate>::iterator coord_it = coords.begin();
				for (; coord_it != coords.end(); ++coord_it) {
					Cell* cell = m_cellCache->getCell(*coord_it);
					if (cell) {
						if (cell->getCellType() > blockerThreshold) {
							std::vector<Cell*>::iterator bc_it = std::find(m_ignoredBlockers.begin(), m_ignoredBlockers.end(), cell);
							if (bc_it == m_ignoredBlockers.end()) {
								blocker = true;
								break;
							}
						}
						if (limitedArea) {
							// check if cell is on one of the areas
							bool sameAreas = false;
							const std::list<std::string> areas = m_route->getLimitedAreas();
							std::list<std::string>::const_iterator area_it = areas.begin();
							for (; area_it != areas.end(); ++area_it) {
								if (m_cellCache->isCellInArea(*area_it, cell)) {
									sameAreas = true;
									break;
								}
							}
							if (!sameAreas) {
								blocker = true;
								break;
							}
						}
					} else {
						blocker = true;
						break;
					}
				}
				if (blocker) {
					continue;
				}
			} else if (limitedArea) {
				// check if cell is on one of the areas
				bool sameAreas = false;
				const std::list<std::string> areas = m_route->getLimitedAreas();
				std::list<std::string>::const_iterator area_it = areas.begin();
				for (; area_it != areas.end(); ++area_it) {
					if (m_cellCache->isCellInArea(*area_it, *i)) {
						sameAreas = true;
						break;
					}
				}
				if (!sameAreas) {
					continue;
				}
			}

			double gCost = m_gCosts[m_next];
			if (m_specialCost) {
				gCost += m_cellCache->getAdjacentCost(adjacentCoord ,nextCoord, m_route->getCostId());
			} else {
				gCost += m_cellCache->getAdjacentCost(adjacentCoord ,nextCoord);
			}
			double hCost = grid->getHeuristicCost(adjacentCoord, destCoord);
			if (m_sf[adjacentInt] == -1) {
				m_sortedfrontier.pushElement(PriorityQueue<int32_t, double>::value_type(adjacentInt, gCost + hCost));
				m_gCosts[adjacentInt] = gCost;
				m_sf[adjacentInt] = m_next;
			} else if (gCost < m_gCosts[adjacentInt] && m_spt[adjacentInt] == -1) {
				m_sortedfrontier.changeElementPriority(adjacentInt, gCost + hCost);
				m_gCosts[adjacentInt] = gCost;
				m_sf[adjacentInt] = m_next;
			}
		}
	}

	void SingleLayerSearch::calcPath() {
		int32_t current = m_destCoordInt;
		int32_t end = m_startCoordInt;
		Path path;
		Location newnode(m_cellCache->getLayer());
		// This assures that the agent always steps into the center of the cell.
		newnode.setExactLayerCoordinates(FIFE::intPt2doublePt(m_to.getLayerCoordinates()));
		path.push_back(newnode);
		while(current != end) {
			if (m_spt[current] < 0 ) {
				// This is when the size of m_spt can not handle the distance of the location
				setSearchStatus(search_status_failed);
				m_route->setRouteStatus(ROUTE_FAILED);
				break;
			}
			current = m_spt[current];
			ModelCoordinate currentCoord = m_cellCache->convertIntToCoord(current);
			newnode.setLayerCoordinates(currentCoord);
			path.push_front(newnode);
		}
		path.front().setExactLayerCoordinates(m_from.getExactLayerCoordinatesRef());
		m_route->setPath(path);
	}
}
