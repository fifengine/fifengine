// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <list>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"
#include "model/structures/layer.h"
#include "pathfinder/route.h"
#include "util/math/fife_math.h"

#include "singlelayersearch.h"

namespace FIFE
{
    namespace
    {
        [[nodiscard]] std::size_t toIndex(const int32_t value)
        {
            assert(value >= 0);
            return static_cast<std::size_t>(value);
        }

        [[nodiscard]] std::size_t toSize(const int32_t value)
        {
            assert(value >= 0);
            return static_cast<std::size_t>(value);
        }
    } // namespace

    SingleLayerSearch::SingleLayerSearch(Route* route, const int32_t sessionId) :
        RoutePatherSearch(route, sessionId),
        m_to(route->getEndNode()),
        m_from(route->getStartNode()),
        m_cellCache(m_from.getLayer()->getCellCache()),
        m_startCoordInt(m_cellCache->convertCoordToInt(m_from.getLayerCoordinates())),
        m_destCoordInt(m_cellCache->convertCoordToInt(m_to.getLayerCoordinates())),
        m_next(0)
    {

        m_sortedfrontier.pushElement(PriorityQueue<int32_t, double>::value_type(m_startCoordInt, 0.0));
        int32_t max_index = m_cellCache->getMaxIndex();
        m_spt.resize(toSize(max_index), -1);
        m_sf.resize(toSize(max_index), -1);
        m_gCosts.resize(toSize(max_index), 0.0);
    }

    SingleLayerSearch::~SingleLayerSearch() = default;

    void SingleLayerSearch::updateSearch()
    {
        if (m_sortedfrontier.empty()) {
            setSearchStatus(search_status_failed);
            m_route->setRouteStatus(ROUTE_FAILED);
            return;
        }

        PriorityQueue<int32_t, double>::value_type topvalue = m_sortedfrontier.getPriorityElement();
        m_sortedfrontier.popElement();
        m_next                      = topvalue.first;
        const std::size_t nextIndex = toIndex(m_next);
        m_spt[nextIndex]            = m_sf[nextIndex];
        // found destination
        if (m_destCoordInt == m_next) {
            setSearchStatus(search_status_complete);
            m_route->setRouteStatus(ROUTE_SEARCHED);
            return;
        }

        ModelCoordinate destCoord = m_to.getLayerCoordinates();
        ModelCoordinate nextCoord = m_cellCache->convertIntToCoord(m_next);
        CellGrid* grid            = m_cellCache->getLayer()->getCellGrid();
        Cell* nextCell            = m_cellCache->getCell(nextCoord);
        if (nextCell == nullptr) {
            return;
        }
        int32_t cellZ                       = nextCell->getLayerCoordinates().z;
        int32_t maxZ                        = m_route->getZStepRange();
        bool zLimited                       = maxZ != -1;
        uint8_t blockerThreshold            = m_ignoreDynamicBlockers ? 2 : 1;
        bool limitedArea                    = m_route->isAreaLimited();
        const std::vector<Cell*>& adjacents = nextCell->getNeighbors();
        for (auto* adjacent : adjacents) {
            if (adjacent == nullptr) {
                continue;
            }
            if (adjacent->getLayer()->getCellCache() != m_cellCache) {
                continue;
            }
            int32_t adjacentInt             = adjacent->getCellId();
            const std::size_t adjacentIndex = toIndex(adjacentInt);
            if (m_sf[adjacentIndex] != -1 && m_spt[adjacentIndex] != -1) {
                continue;
            }
            if (zLimited && std::abs(cellZ - adjacent->getLayerCoordinates().z) > maxZ) {
                continue;
            }
            bool blocker                  = adjacent->getCellType() > blockerThreshold;
            ModelCoordinate adjacentCoord = adjacent->getLayerCoordinates();
            if ((adjacentInt == m_next || blocker) && adjacentInt != m_destCoordInt) {
                if (!blocker && m_multicell) {
                    continue;
                } else if (!m_multicell) {
                    continue;
                }
            }
            // search if there are blockers which could block multicell object
            if (m_multicell) {
                blocker = false;
                Location currentLoc(nextCell->getLayer());
                currentLoc.setLayerCoordinates(nextCell->getLayerCoordinates());
                Location adjacentLoc(adjacent->getLayer());
                adjacentLoc.setLayerCoordinates(adjacent->getLayerCoordinates());

                int32_t rotation = getAngleBetween(currentLoc, adjacentLoc);
                std::vector<ModelCoordinate> coords =
                    grid->toMultiCoordinates(adjacentLoc.getLayerCoordinates(), m_route->getOccupiedCells(rotation));
                auto coord_it = coords.begin();
                for (; coord_it != coords.end(); ++coord_it) {
                    Cell* cell = m_cellCache->getCell(*coord_it);
                    if (cell != nullptr) {
                        if (cell->getCellType() > blockerThreshold) {
                            auto bc_it = std::ranges::find(m_ignoredBlockers, cell);
                            if (bc_it == m_ignoredBlockers.end()) {
                                blocker = true;
                                break;
                            }
                        }
                        if (limitedArea) {
                            // check if cell is on one of the areas
                            bool sameAreas                     = false;
                            const std::list<std::string> areas = m_route->getLimitedAreas();
                            auto area_it                       = areas.begin();
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
                bool sameAreas                     = false;
                const std::list<std::string> areas = m_route->getLimitedAreas();
                auto area_it                       = areas.begin();
                for (; area_it != areas.end(); ++area_it) {
                    if (m_cellCache->isCellInArea(*area_it, adjacent)) {
                        sameAreas = true;
                        break;
                    }
                }
                if (!sameAreas) {
                    continue;
                }
            }

            double gCost = m_gCosts[nextIndex];
            if (m_specialCost) {
                gCost += m_cellCache->getAdjacentCost(adjacentCoord, nextCoord, m_route->getCostId());
            } else {
                gCost += m_cellCache->getAdjacentCost(adjacentCoord, nextCoord);
            }
            double hCost = grid->getHeuristicCost(adjacentCoord, destCoord);
            if (m_sf[adjacentIndex] == -1) {
                m_sortedfrontier.pushElement(PriorityQueue<int32_t, double>::value_type(adjacentInt, gCost + hCost));
                m_gCosts[adjacentIndex] = gCost;
                m_sf[adjacentIndex]     = m_next;
            } else if (gCost < m_gCosts[adjacentIndex] && m_spt[adjacentIndex] == -1) {
                m_sortedfrontier.changeElementPriority(adjacentInt, gCost + hCost);
                m_gCosts[adjacentIndex] = gCost;
                m_sf[adjacentIndex]     = m_next;
            }
        }
    }

    void SingleLayerSearch::calcPath()
    {
        int32_t current = m_destCoordInt;
        int32_t end     = m_startCoordInt;
        Path path;
        Location newnode(m_cellCache->getLayer());
        // This assures that the agent always steps into the center of the cell.
        newnode.setExactLayerCoordinates(FIFE::intPt2doublePt(m_to.getLayerCoordinates()));
        path.push_back(newnode);
        while (current != end) {
            const std::size_t currentIndex = toIndex(current);
            if (m_spt[currentIndex] < 0) {
                // This is when the size of m_spt can not handle the distance of the location
                setSearchStatus(search_status_failed);
                m_route->setRouteStatus(ROUTE_FAILED);
                break;
            }
            current                      = m_spt[currentIndex];
            ModelCoordinate currentCoord = m_cellCache->convertIntToCoord(current);
            newnode.setLayerCoordinates(currentCoord);
            path.push_front(newnode);
        }
        path.front().setExactLayerCoordinates(m_from.getExactLayerCoordinatesRef());
        m_route->setPath(path);
    }
} // namespace FIFE
