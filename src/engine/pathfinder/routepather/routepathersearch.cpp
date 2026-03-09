// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>

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

#include "routepathersearch.h"
#include <vector>

namespace FIFE
{
    RoutePatherSearch::RoutePatherSearch(Route* route, const int32_t sessionId) :
        m_route(route), m_multicell(route->isMultiCell()), m_sessionId(sessionId), m_status(search_status_incomplete)
    {

        m_route->setRouteStatus(ROUTE_SEARCHING);
        m_specialCost           = route->getCostId() != "";
        m_ignoreDynamicBlockers = route->isDynamicBlockerIgnored();
        if (m_multicell) {
            Location loc                                       = route->getStartNode();
            std::vector<ModelCoordinate> coords                = route->getOccupiedArea();
            std::vector<ModelCoordinate>::const_iterator co_it = coords.begin();
            for (; co_it != coords.end(); ++co_it) {
                Cell* cell = loc.getLayer()->getCellCache()->getCell(*co_it);
                if (cell) {
                    m_ignoredBlockers.push_back(cell);
                }
            }
        }
    }

    RoutePatherSearch::~RoutePatherSearch() { }

    int32_t RoutePatherSearch::getSessionId() const
    {
        return m_sessionId;
    }

    int32_t RoutePatherSearch::getSearchStatus() const
    {
        return m_status;
    }

    Route* RoutePatherSearch::getRoute()
    {
        return m_route;
    }

    void RoutePatherSearch::setSearchStatus(const SearchStatus status)
    {
        m_status = status;
    }
} // namespace FIFE
