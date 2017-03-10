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
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/cellcache.h"
#include "util/math/angles.h"
#include "pathfinder/route.h"

#include "routepather.h"
#include "routepathersearch.h"
#include "singlelayersearch.h"
#include "multilayersearch.h"

namespace FIFE {

	int32_t RoutePather::makeSessionId() {
		return m_nextFreeSessionId++;
	}

	bool RoutePather::locationsEqual(const Location& a, const Location& b) {
		bool sameLayer = a.getLayer() == b.getLayer();
		const ModelCoordinate a_coord = a.getLayerCoordinates();
		const ModelCoordinate b_coord = b.getLayerCoordinates();

		return (a_coord.x == b_coord.x) && (a_coord.y == b_coord.y) && sameLayer;
	}

	void RoutePather::update() {
		int32_t ticksleft = m_maxTicks;
		while (ticksleft > 0) {
			if(m_sessions.empty()) {
				break;
			}
			RoutePatherSearch* prioritySession = m_sessions.getPriorityElement().first;
			if(!sessionIdValid(prioritySession->getSessionId())) {
				delete prioritySession;
				m_sessions.popElement();
				continue;
			}
			prioritySession->updateSearch();
			if (prioritySession->getSearchStatus() == RoutePatherSearch::search_status_complete) {
				const int32_t sessionId = prioritySession->getSessionId();
				prioritySession->calcPath();
				Route* route = prioritySession->getRoute();
				if (route->getRouteStatus() == ROUTE_SOLVED) {
					invalidateSessionId(sessionId);
					delete prioritySession;
					m_sessions.popElement();
				}
			} else if (prioritySession->getSearchStatus() == RoutePatherSearch::search_status_failed) {
				const int32_t sessionId = prioritySession->getSessionId();
				invalidateSessionId(sessionId);
				delete prioritySession;
				m_sessions.popElement();
			}
			--ticksleft;
		}
	}

	bool RoutePather::cancelSession(const int32_t sessionId) {
		if (sessionId >= 0) {
			return invalidateSessionId(sessionId);
		}
		return false;
	}

	void RoutePather::addSessionId(const int32_t sessionId) {
		m_registeredSessionIds.push_back(sessionId);
	}

	bool RoutePather::sessionIdValid(const int32_t sessionId) {
		for(SessionList::const_iterator i = m_registeredSessionIds.begin();
			i != m_registeredSessionIds.end(); ++i) {
			if((*i) == sessionId) {
				return true;
			}
		}
		return false;
	}

	bool RoutePather::invalidateSessionId(const int32_t sessionId) {
		for(SessionList::iterator i = m_registeredSessionIds.begin();
			i != m_registeredSessionIds.end(); ++i) {
			if((*i) == sessionId) {
				m_registeredSessionIds.erase(i);
				return true;
			}
		}
		return false;
	}

	Route* RoutePather::createRoute(const Location& start, const Location& end, bool immediate, const std::string& costId) {
		Route* route = new Route(start, end);
		if (costId != "") {
			route->setCostId(costId);
		}
		if (immediate) {
			if (!solveRoute(route, MEDIUM_PRIORITY, true)) {
				route->setRouteStatus(ROUTE_FAILED);
			}
			return route;
 		}
		return route;
	}

	bool RoutePather::solveRoute(Route* route, int32_t priority, bool immediate) {
		if (sessionIdValid(route->getSessionId())) {
			return false;
		}

		const Location& start = route->getStartNode();
		const Location& end = route->getEndNode();

		if (locationsEqual(start, end)) {
			return false;
		}

		CellCache* startCache = start.getLayer()->getCellCache();
		CellCache* endCache = end.getLayer()->getCellCache();

		if (!startCache || !endCache) {
			return false;
		}

		if (!startCache->isInCellCache(start) || !endCache->isInCellCache(end)) {
			return false;
		}

		Cell* startCell = startCache->getCell(start.getLayerCoordinates());
		Cell* endCell = endCache->getCell(end.getLayerCoordinates());

		bool multilayer = startCache != endCache;
		if (!multilayer) {
			Zone* startZone = startCell->getZone();
			Zone* endZone = endCell->getZone();
			if (startZone != endZone) {
				// look for special cases (start is zone border or end is static blocker)
				if (!endZone || startCell->isZoneProtected()) {
					bool found = false;
					const std::vector<Cell*>& neighbors = endCell->getNeighbors();
					for (std::vector<Cell*>::const_iterator it = neighbors.begin();
						it != neighbors.end(); ++it) {
						Zone* tmpZone = (*it)->getZone();
						if (tmpZone) {
							endZone = tmpZone;
							if (tmpZone == startZone) {
								found = true;
								break;
							}
						}
					}
					if (!found && startCell->isZoneProtected()) {
						const std::vector<Cell*>& neighbors = startCell->getNeighbors();
						for (std::vector<Cell*>::const_iterator it = neighbors.begin();
							it != neighbors.end(); ++it) {
							Zone* tmpZone = (*it)->getZone();
							if (tmpZone) {
								if (tmpZone == startZone) {
									endZone = tmpZone;
									break;
								}
							}
						}
					}
				}
				// target and all neighbors are static blockers
				if (!endZone) {
					return false;
				}
				// same CellCache but different zones
				if (startZone != endZone) {
					multilayer = true;
				}
			}
		}
		
		if (route->isAreaLimited()) {
			// check if target or neighbors are on one of the areas
			bool sameAreas = false;
			const std::list<std::string> areas = route->getLimitedAreas();
			std::list<std::string>::const_iterator area_it = areas.begin();
			for (; area_it != areas.end(); ++area_it) {
				if (endCache->isCellInArea(*area_it, endCell)) {
					sameAreas = true;
					break;
				}
			}
			if (!sameAreas) {
				const std::vector<Cell*>& neighbors = endCell->getNeighbors();
				if (neighbors.empty()) {
					return false;
				}
				area_it = areas.begin();
				for (; area_it != areas.end(); ++area_it) {
					std::vector<Cell*>::const_iterator neigh_it = neighbors.begin();
					for (; neigh_it != neighbors.end(); ++neigh_it) {
						if (endCache->isCellInArea(*area_it, *neigh_it)) {
							sameAreas = true;
							break;
						}
					}
				}
			}
			if (!sameAreas) {
				return false;
			}
		}

		int32_t sessionId = route->getSessionId();
		if (sessionId == -1) {
			sessionId = makeSessionId();
			route->setSessionId(sessionId);
		}

		RoutePatherSearch* newSearch;
		if (multilayer) {
			newSearch = new MultiLayerSearch(route, sessionId);
		} else {
			newSearch = new SingleLayerSearch(route, sessionId);
		}
		if (immediate) {
			while (newSearch->getSearchStatus() != RoutePatherSearch::search_status_complete) {
				newSearch->updateSearch();
				if (newSearch->getSearchStatus() == RoutePatherSearch::search_status_failed) {
					route->setRouteStatus(ROUTE_FAILED);
					break;
				}
			}

			if (newSearch->getSearchStatus() == RoutePatherSearch::search_status_complete) {
				newSearch->calcPath();
				route->setRouteStatus(ROUTE_SOLVED);
			}
			delete newSearch;
			return true;
		}
		m_sessions.pushElement(SessionQueue::value_type(newSearch, priority));
		addSessionId(sessionId);
		return true;
	}

	bool RoutePather::followRoute(const Location& current, Route* route, double speed, Location& nextLocation) {
		Path path = route->getPath();
		if (path.empty()) {
			return false;
		}
		if (Mathd::Equal(speed, 0.0)) {
			return true;
		}
		bool nextBlocker = false;
		Location currentNode = route->getCurrentNode();
		bool multiCell = route->isMultiCell();
		if (!locationsEqual(current, currentNode)) {
			// special blocker check for multicell
			if (multiCell) {
				int32_t oldRotation = route->getRotation();
				// old coordinates
				std::vector<ModelCoordinate> oldCoords = current.getLayer()->getCellGrid()->
					toMultiCoordinates(current.getLayerCoordinates(), route->getOccupiedCells(route->getRotation()));
				oldCoords.push_back(current.getLayerCoordinates());
				route->setRotation(getAngleBetween(current, currentNode));
				// new coordinates
				std::vector<ModelCoordinate> newCoords = currentNode.getLayer()->getCellGrid()->
					toMultiCoordinates(currentNode.getLayerCoordinates(), route->getOccupiedCells(route->getRotation()));
				newCoords.push_back(currentNode.getLayerCoordinates());

				std::vector<ModelCoordinate>::const_iterator nco_it = newCoords.begin();
				for (; nco_it != newCoords.end(); ++nco_it) {
					if (currentNode.getLayer()->cellContainsBlockingInstance(*nco_it)) {
						bool found = false;
						std::vector<ModelCoordinate>::const_iterator oco_it = oldCoords.begin();
						for (; oco_it != oldCoords.end(); ++oco_it) {
							if (*oco_it == *nco_it) {
								found = true;
								break;
							}
						}
						// if we have a blocker that is not part of the object
						if (!found) {
							nextBlocker = true;
						}
					}
					if (nextBlocker) {
						route->setRotation(oldRotation);
						break;
					}
				}
			} else {
				route->setRotation(getAngleBetween(current, currentNode));
				if (currentNode.getLayer()->cellContainsBlockingInstance(currentNode.getLayerCoordinates())) {
					nextBlocker = true;
				}
			}
		}
		// set facinglocation
		ExactModelCoordinate instancePos = current.getMapCoordinates();
		// if next node is blocker
		if (nextBlocker) {
			nextLocation.setLayerCoordinates(FIFE::doublePt2intPt(current.getExactLayerCoordinates()));
			return false;
		}
		// calculate distance
		CellCache* nodeCache = currentNode.getLayer()->getCellCache();
		CellGrid* nodeGrid = currentNode.getLayer()->getCellGrid();
		ExactModelCoordinate targetPos = currentNode.getMapCoordinates();
		Cell* tmpCell = nodeCache->getCell(currentNode.getLayerCoordinates());
		if (tmpCell) {
			targetPos.z = tmpCell->getLayerCoordinates().z + nodeGrid->getZShift();
		}
		double dx = (targetPos.x - instancePos.x) * nodeGrid->getXScale();
		double dy = (targetPos.y - instancePos.y) * nodeGrid->getYScale();
		double distance = Mathd::Sqrt(dx * dx + dy * dy);
		// cell speed multi
		double multi;
		if (nodeCache->getCellSpeedMultiplier(current.getLayerCoordinates(), multi)) {
			speed *= multi;
		} else {
			speed *= nodeCache->getDefaultSpeedMultiplier();
		}
		bool pop = false;
		if (speed > distance) {
			speed = distance;
			pop = true;
		}
		if (!Mathd::Equal(distance, 0.0) && !pop) {
			Location prevNode = route->getPreviousNode();
			CellCache* prevCache = prevNode.getLayer()->getCellCache();
			CellGrid* prevGrid = prevNode.getLayer()->getCellGrid();
			ExactModelCoordinate prevPos = route->getPreviousNode().getMapCoordinates();
			tmpCell = prevCache->getCell(prevNode.getLayerCoordinates());
			if (tmpCell) {
				prevPos.z = tmpCell->getLayerCoordinates().z + prevGrid->getZShift();
			}
			double cell_dz = (targetPos.z - prevPos.z);
			if (!Mathd::Equal(cell_dz, 0.0)) {
				double cell_dx = (targetPos.x - prevPos.x);
				double cell_dy = (targetPos.y - prevPos.y);
				double cell_distance = Mathd::Sqrt(cell_dx * cell_dx + cell_dy * cell_dy);
				if (cell_dz > 0) {
					if (locationsEqual(current, currentNode)) {
						instancePos.z = targetPos.z;
					} else {
						instancePos.z = prevPos.z + cell_dz - 4*(0.5-distance/cell_distance)*(0.5-distance/cell_distance) * cell_dz;
					}
				} else if (cell_dz < 0) {
					if (locationsEqual(current, currentNode)) {
						instancePos.z = prevPos.z + 4*(0.5-distance/cell_distance)*(0.5-distance/cell_distance) * cell_dz;
					}
				}
			}
			instancePos.x += (dx / distance) * speed;
			instancePos.y += (dy / distance) * speed;
		} else {
			pop = true;
		}
		// pop to next node
		if (pop) {
			nextLocation.setMapCoordinates(targetPos);
			// if cw is false we have reached the end
			bool cw = route->walkToNextNode();
			// check transistion
			CellCache* cache = nextLocation.getLayer()->getCellCache();
			if (cache) {
				Cell* cell = cache->getCell(nextLocation.getLayerCoordinates());
				if (cell) {
					TransitionInfo* ti = cell->getTransition();
					if (ti) {
						// "beam" if it is a part of path
						if (cw &&
							!cell->getLayer()->getCellGrid()->isAccessible(nextLocation.getLayerCoordinates(),
							route->getCurrentNode().getLayerCoordinates())) {
							if (ti->m_difflayer) {
								nextLocation.setLayer(ti->m_layer);
							}
							nextLocation.setLayerCoordinates(ti->m_mc);
							return cw;
						// immediate "beam"
						} else if (ti->m_immediate) {
							if (ti->m_difflayer) {
								nextLocation.setLayer(ti->m_layer);
							}
							nextLocation.setLayerCoordinates(ti->m_mc);
							route->setEndNode(nextLocation);
							return false;
						}
					}
				}
			}
			if (cw && !multiCell &&
				currentNode.getLayer()->cellContainsBlockingInstance(route->getCurrentNode().getLayerCoordinates())) {
				//set facing to end blocker
				Location facing = route->getCurrentNode();
				route->setRotation(getAngleBetween(current, facing));

				return false;
			}
			return cw;
		}
		nextLocation.setMapCoordinates(instancePos);

		return true;
	}

	void RoutePather::setMaxTicks(int32_t ticks) {
		m_maxTicks = ticks;
	}

	int32_t RoutePather::getMaxTicks() {
		return m_maxTicks;
	}

	std::string RoutePather::getName() const {
		return "RoutePather";
	}
}
