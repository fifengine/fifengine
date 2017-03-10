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
#include "model/structures/map.h"
#include "pathfinder/route.h"
#include "util/math/fife_math.h"

#include "multilayersearch.h"

namespace FIFE {
	MultiLayerSearch::MultiLayerSearch(Route* route, const int32_t sessionId):
		RoutePatherSearch(route, sessionId),
		m_to(route->getEndNode()),
		m_from(route->getStartNode()),
		m_startCache(m_from.getLayer()->getCellCache()),
		m_endCache(m_to.getLayer()->getCellCache()),
		m_currentCache(NULL),
		m_startZone(m_startCache->getCell(m_from.getLayerCoordinates())->getZone()),
		m_endZone(m_endCache->getCell(m_to.getLayerCoordinates())->getZone()),
		m_startCoordInt(m_startCache->convertCoordToInt(m_from.getLayerCoordinates())),
		m_lastStartCoordInt(m_startCoordInt),
		m_destCoordInt(m_endCache->convertCoordToInt(m_to.getLayerCoordinates())),
		m_lastDestCoordInt(-1),
		m_next(0),
		m_foundLast(true) {

		// if end zone is invalid (static blocker) then change it
		if (!m_endZone) {
			Cell* endcell = m_endCache->getCell(m_to.getLayerCoordinates());
			const std::vector<Cell*>& neighbors = endcell->getNeighbors();
			for (std::vector<Cell*>::const_iterator it = neighbors.begin();
				it != neighbors.end(); ++it) {
				Zone* tmpzone = (*it)->getZone();
				if (tmpzone) {
					m_endZone = tmpzone;
					if (tmpzone == m_startZone) {
						break;
					}
				}
			}
		}
		Cell* startCell = m_startCache->getCell(m_from.getLayerCoordinates());

		// here we hope to find between targets
		// first test if target can be reached easy
		searchBetweenTargetsNeighbor();
		// if no betweenTargets could be found we check as second the whole map
		if (m_betweenTargets.empty()) {
			searchBetweenTargetsMap();
		}
		// if it is a protected cell it can have a second startzone
		if (m_betweenTargets.empty() && startCell->isZoneProtected()) {
			const std::vector<Cell*>& neighbors = startCell->getNeighbors();
			for (std::vector<Cell*>::const_iterator it = neighbors.begin();
				it != neighbors.end(); ++it) {
				Zone* tmpzone = (*it)->getZone();
				if (tmpzone) {
					if (tmpzone != m_startZone) {
						m_startZone = tmpzone;
						break;
					}
				}
			}
			searchBetweenTargetsNeighbor();
			if (m_betweenTargets.empty()) {
				searchBetweenTargetsMap();
			}
		}
		// failed to find between targets, no Path can be created
		if (m_betweenTargets.empty()) {
			setSearchStatus(search_status_failed);
			m_route->setRouteStatus(ROUTE_FAILED);
		}
	}

	MultiLayerSearch::~MultiLayerSearch() {
	}

	void MultiLayerSearch::createSearchFrontier(int32_t startInt, CellCache* cache) {
		// reset all
		m_sortedFrontier.clear();
		m_spt.clear();
		m_sf.clear();
		m_gCosts.clear();
		// fill with defaults
		m_sortedFrontier.pushElement(PriorityQueue<int32_t, double>::value_type(startInt, 0.0));
		int32_t max_index = cache->getMaxIndex();
		m_spt.resize(max_index, -1);
		m_sf.resize(max_index, -1);
		m_gCosts.resize(max_index, 0.0);
		m_next = 0;
	}

	void MultiLayerSearch::updateSearch() {
		if (m_sortedFrontier.empty()) {
			if (!m_foundLast || m_lastDestCoordInt == m_destCoordInt || getSearchStatus() == search_status_failed) {
				setSearchStatus(search_status_failed);
				m_route->setRouteStatus(ROUTE_FAILED);
				return;
			}
			if (m_betweenTargets.empty()) {
				TransitionInfo* trans = m_currentCache->getCell(m_currentCache->convertIntToCoord(m_lastDestCoordInt))->getTransition();
				if (trans) {
					m_lastStartCoordInt = m_endCache->convertCoordToInt(trans->m_mc);
				}
				m_currentCache = m_endCache;
				m_lastDestCoordInt = m_currentCache->getCell(m_to.getLayerCoordinates())->getCellId();
			} else {
				if (m_lastDestCoordInt != -1) {
					TransitionInfo* trans = m_currentCache->getCell(m_currentCache->convertIntToCoord(m_lastDestCoordInt))->getTransition();
					if (trans) {
						m_lastStartCoordInt = trans->m_layer->getCellCache()->convertCoordToInt(trans->m_mc);
					}
				}
				m_currentCache = m_betweenTargets.front()->getLayer()->getCellCache();
				m_lastDestCoordInt = m_betweenTargets.front()->getCellId();
				m_betweenTargets.pop_front();
				m_foundLast = false;
			}
			createSearchFrontier(m_lastStartCoordInt, m_currentCache);
		}

		PriorityQueue<int32_t, double>::value_type topvalue = m_sortedFrontier.getPriorityElement();
		m_sortedFrontier.popElement();
		m_next = topvalue.first;
		m_spt[m_next] = m_sf[m_next];
		// found destination
		if (m_destCoordInt == m_next && m_betweenTargets.empty()) {
			if (m_endCache == m_currentCache) {
				setSearchStatus(search_status_complete);
				m_route->setRouteStatus(ROUTE_SEARCHED);
				return;
			}
		}

		// found between target
		if (m_lastDestCoordInt == m_next) {
			calcPathStep();
			m_sortedFrontier.clear();
			m_foundLast = true;
			return;
		}

		ModelCoordinate destCoord = m_currentCache->convertIntToCoord(m_lastDestCoordInt);
		ModelCoordinate nextCoord = m_currentCache->convertIntToCoord(m_next);
		CellGrid* grid = m_currentCache->getLayer()->getCellGrid();
		Cell* nextCell = m_currentCache->getCell(nextCoord);
		if (!nextCell) {
			return;
		}
		int32_t cellZ = nextCell->getLayerCoordinates().z;
		int32_t maxZ = m_route->getZStepRange();
		bool zLimited = maxZ != -1;
		uint8_t blockerThreshold = m_ignoreDynamicBlockers ? 2 : 1;
		bool limitedArea = m_route->isAreaLimited();
		const std::vector<Cell*>& adjacents = nextCell->getNeighbors();
		if (adjacents.empty()) {
			return;
		}
		for (std::vector<Cell*>::const_iterator i = adjacents.begin(); i != adjacents.end(); ++i) {
			if (*i == NULL) {
				continue;
			}
			if ((*i)->getLayer()->getCellCache() != m_currentCache) {
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
				if (blocker && m_multicell) {
					std::vector<Cell*>::iterator bc_it = std::find(m_ignoredBlockers.begin(), m_ignoredBlockers.end(), *i);
					if (bc_it == m_ignoredBlockers.end()) {
						continue;
					}
				} else {
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
					Cell* cell = m_currentCache->getCell(*coord_it);
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
								if (m_currentCache->isCellInArea(*area_it, cell)) {
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
					if (m_currentCache->isCellInArea(*area_it, *i)) {
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
				gCost += m_currentCache->getAdjacentCost(adjacentCoord ,nextCoord, m_route->getCostId());
			} else {
				gCost += m_currentCache->getAdjacentCost(adjacentCoord ,nextCoord);
			}
			double hCost = grid->getHeuristicCost(adjacentCoord, destCoord);
			if (m_sf[adjacentInt] == -1) {
				m_sortedFrontier.pushElement(PriorityQueue<int32_t, double>::value_type(adjacentInt, gCost + hCost));
				m_gCosts[adjacentInt] = gCost;
				m_sf[adjacentInt] = m_next;
			} else if (gCost < m_gCosts[adjacentInt] && m_spt[adjacentInt] == -1) {
				m_sortedFrontier.changeElementPriority(adjacentInt, gCost + hCost);
				m_gCosts[adjacentInt] = gCost;
				m_sf[adjacentInt] = m_next;
			}
		}
	}

	void MultiLayerSearch::calcPathStep() {
		int32_t current = m_lastDestCoordInt;
		int32_t end = m_lastStartCoordInt;
		Location newnode(m_currentCache->getLayer());
		Path path;
		// This assures that the agent always steps into the center of the target cell.
		newnode.setLayerCoordinates(m_currentCache->convertIntToCoord(current));
		path.push_back(newnode);
		while(current != end) {
			if (m_spt[current] < 0 ) {
				// This is when the size of m_spt can not handle the distance of the location
				setSearchStatus(search_status_failed);
				m_route->setRouteStatus(ROUTE_FAILED);
				break;
			}
			current = m_spt[current];
			newnode.setLayerCoordinates(m_currentCache->convertIntToCoord(current));
			path.push_front(newnode);
		}
		// set exact start coordinates
		if (m_path.empty()) {
			path.front().setExactLayerCoordinates(m_from.getExactLayerCoordinatesRef());
		}
		m_path.insert(m_path.end(), path.begin(), path.end());
	}

	void MultiLayerSearch::calcPath() {
		int32_t current = m_lastDestCoordInt;
		int32_t end = m_lastStartCoordInt;
		Location newnode(m_currentCache->getLayer());
		Path path;
		// This assures that the agent always steps into the center of the target cell.
		newnode.setLayerCoordinates(
			m_currentCache->getCell(m_currentCache->convertIntToCoord(current))->getLayerCoordinates());
		path.push_back(newnode);
		while(current != end) {
			if (m_spt[current] < 0 ) {
				// This is when the size of m_spt can not handle the distance of the location
				setSearchStatus(search_status_failed);
				m_route->setRouteStatus(ROUTE_FAILED);
				break;
			}
			current = m_spt[current];
			newnode.setLayerCoordinates(m_currentCache->convertIntToCoord(current));
			path.push_front(newnode);
		}
		m_path.insert(m_path.end(), path.begin(), path.end());
		m_route->setPath(m_path);
	}

	void MultiLayerSearch::searchBetweenTargetsNeighbor() {
		std::vector<Cell*> cells = m_startCache->getTransitionCells(m_endCache->getLayer());
		if (!cells.empty()) {
			Location loc;
			Cell* cell = NULL;
			// find nearest portal (air-line distance)
			for (std::vector<Cell*>::iterator it = cells.begin(); it != cells.end(); ++it) {
				if ((*it)->getZone() != m_startZone) {
					continue;
				}
				TransitionInfo* trans = (*it)->getTransition();
				Cell* transTarget = trans->m_layer->getCellCache()->getCell(trans->m_mc);
				if (transTarget->getZone() != m_endZone) {
					continue;
				}
				if (!cell) {
					loc.setLayer((*it)->getLayer());
					loc.setLayerCoordinates((*it)->getLayerCoordinates());
					cell = *it;
					continue;
				}
				Location temp((*it)->getLayer());
				temp.setLayerCoordinates((*it)->getLayerCoordinates());
				Location loc1(cell->getTransition()->m_layer);
				loc1.setLayerCoordinates(cell->getTransition()->m_mc);
				Location loc2((*it)->getTransition()->m_layer);
				loc2.setLayerCoordinates((*it)->getTransition()->m_mc);

				double temp_distance = temp.getLayerDistanceTo(m_from) + loc2.getLayerDistanceTo(m_to);
				double current_distance = loc.getLayerDistanceTo(m_from) + loc1.getLayerDistanceTo(m_to);
				if (current_distance > temp_distance) {
					loc = temp;
					cell = *it;
				}
			}
			if (cell) {
				m_betweenTargets.push_back(cell);
			}
		}
	}

	void MultiLayerSearch::searchBetweenTargetsMap() {
		// in case no transition is there then return
		std::vector<Cell*> endTransCells;
		std::vector<Cell*> tmpTransCells = m_endCache->getTransitionCells();
		std::vector<Cell*>::iterator tcell_it = tmpTransCells.begin();
		for (; tcell_it != tmpTransCells.end(); ++tcell_it) {
			Zone* zone = (*tcell_it)->getZone();
			if (zone == m_endZone) {
				endTransCells.push_back(*tcell_it);
			}
		}
		if (endTransCells.empty()) {
			return;
		}
		// in case no transition is there then return
		std::vector<Cell*> startTransCells;
		tmpTransCells = m_startCache->getTransitionCells();
		tcell_it = tmpTransCells.begin();
		for (; tcell_it != tmpTransCells.end(); ++tcell_it) {
			Zone* zone = (*tcell_it)->getZone();
			if (zone == m_startZone) {
				startTransCells.push_back(*tcell_it);
			}
		}
		if (startTransCells.empty()) {
			return;
		}

		// fetch zones
		std::vector<Zone*> zones;
		const std::list<Layer*>& allLayers = m_from.getLayer()->getMap()->getLayers();
		std::list<Layer*>::const_iterator lay_it = allLayers.begin();
		for (; lay_it != allLayers.end(); ++lay_it) {
			CellCache* cache = (*lay_it)->getCellCache();
			if (cache) {
				const std::vector<Zone*>& tmp_zones = cache->getZones();
				zones.insert(zones.end(), tmp_zones.begin(), tmp_zones.end());
			}
		}

		// sort zones by iterator distance
		std::map<Zone*, int32_t> zoneDistanceMap;
		std::map<int32_t, Zone*> distanceZoneMap;
		for (std::vector<Zone*>::iterator zit = zones.begin(); zit != zones.end(); ++zit) {
			// pseudo distance
			int32_t distance = std::distance(zones.begin(), zit);
			zoneDistanceMap.insert(std::pair<Zone*, int32_t>(*zit, distance));
			distanceZoneMap.insert(std::pair<int32_t, Zone*>(distance, *zit));
		}
		// start zone
		int32_t startZone = zoneDistanceMap.find(m_startZone)->second;
		// target zone
		int32_t targetZone = zoneDistanceMap.find(m_endZone)->second;
		// Priority queue to sort zones
		PriorityQueue<int32_t, double> sortedfrontier;
		// add start zone
		sortedfrontier.pushElement(PriorityQueue<int32_t, double>::value_type(startZone, 0.0));
		// max size zones
		int32_t max_index = zones.size();
		// shortest tree
		std::vector<int32_t> spt(max_index, -1);
		// search frontier
		std::vector<int32_t> sf(max_index, -1);
		// costs
		std::vector<double> costs(max_index, 0.0);
		bool found = false;
		while (!found) {
			if (sortedfrontier.empty()) {
				break;
			}
			PriorityQueue<int32_t, double>::value_type topvalue = sortedfrontier.getPriorityElement();
			sortedfrontier.popElement();
			int32_t next = topvalue.first;
			spt[next] = sf[next];
			// found destination zone
			if (targetZone == next) {
				found = true;
				break;
			}
			Zone* nextZone = distanceZoneMap.find(next)->second;
			// look in zone for tranistions
			std::vector<Cell*> transCells = nextZone->getTransitionCells();
			if (transCells.empty()) {
				continue;
			}
			std::vector<Cell*>::iterator cell_it = transCells.begin();
			for (; cell_it != transCells.end(); ++cell_it) {
				// transition info
				TransitionInfo* trans = (*cell_it)->getTransition();
				// target transition cache
				CellCache* transCache = trans->m_layer->getCellCache();
				// target transition cell
				Cell* transCelle = transCache->getCell(trans->m_mc);
				// next zone as int
				int32_t nextInt =  zoneDistanceMap.find(transCelle->getZone())->second;
				if (nextInt == next && nextInt != targetZone) {
					continue;
				}
				// iterator distance as cost
				double cost = costs[next] + static_cast<double>(ABS(nextInt-startZone));
				if (sf[nextInt] == -1) {
					sortedfrontier.pushElement(PriorityQueue<int32_t, double>::value_type(nextInt, cost));
					costs[nextInt] = cost;
					sf[nextInt] = next;
				} else if (cost < costs[nextInt] && spt[nextInt] == -1) {
					sortedfrontier.changeElementPriority(nextInt, cost);
					costs[nextInt] = cost;
					sf[nextInt] = next;
				}
			}
		}
		// startZone to endZone could be solved
		if (found) {
			// fetch sorted zones
			int32_t current = targetZone;
			int32_t end = startZone;

			std::list<Zone*> betweenZones;
			Zone* tempZone = distanceZoneMap.find(current)->second;
			betweenZones.push_back(tempZone);
			while(current != end) {
				if (spt[current] < 0 ) {
					return;
				}
				current = spt[current];
				tempZone = distanceZoneMap.find(current)->second;
				betweenZones.push_front(tempZone);
			}
			// so here we fetch the transistions
			Location lastLoc = m_from;
			for (std::list<Zone*>::iterator lit = betweenZones.begin(); lit != betweenZones.end(); ++lit) {
				++lit;
				if (lit == betweenZones.end()) {
					break;
				}
				Zone* nextZone = *lit;
				--lit;
				Zone* currentZone = *lit;
				std::vector<Cell*> tempCells = currentZone->getTransitionCells();
				if (tempCells.empty()) {
					continue;
				}

				Cell* transCell = NULL;
				double nextCost = 0.0;
				Location newLoc = lastLoc;
				for (std::vector<Cell*>::iterator cit = tempCells.begin(); cit != tempCells.end(); ++cit) {
					Cell* nextCell = *cit;
					TransitionInfo* nextTrans = nextCell->getTransition();
					Cell* transTargetCell = nextTrans->m_layer->getCellCache()->getCell(nextTrans->m_mc);
					// skip wrong transitions
					if (nextZone != transTargetCell->getZone()) {
						continue;
					}
					// nearest transistion (air-line distance)
					Location tmpLoc((*cit)->getLayer());
					tmpLoc.setLayerCoordinates((*cit)->getLayerCoordinates());
					double locCost = lastLoc.getLayerDistanceTo(tmpLoc);
					if (!transCell || locCost < nextCost) {
						nextCost = locCost;
						transCell = *cit;
						newLoc.setLayer(transTargetCell->getLayer());
						newLoc.setLayerCoordinates(transTargetCell->getLayerCoordinates());
					}
				}
				// found valid transition cell
				if (transCell) {
					m_betweenTargets.push_back(transCell);
					lastLoc = newLoc;
				}
			}
		}
	}
}
