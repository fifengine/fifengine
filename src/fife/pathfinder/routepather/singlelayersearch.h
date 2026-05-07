// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PATHFINDER_SINGLELAYERSEARCH
#define FIFE_PATHFINDER_SINGLELAYERSEARCH

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
#include "model/structures/location.h"
#include "routepathersearch.h"
#include "util/structures/priorityqueue.h"

namespace FIFE
{

    class CellCache;
    class Route;

    /** SingleLayerSearch using A*
     */
    class FIFE_API SingleLayerSearch : public RoutePatherSearch
    {
        public:
            /** Constructor
             *
             * @param route A pointer to the route for which a path should be searched.
             * @param sessionId A integer containing the session id for this search.
             */
            SingleLayerSearch(Route* route, int32_t sessionId);

            /** Destructor
             */
            ~SingleLayerSearch() override;

            /** Updates the search.
             *
             * Each update checks all neighbors of the last checked coordinate and selects the most favorable.
             */
            void updateSearch() override;

            /** Calculates final path.
             *
             * If the search is successful then a path is created.
             */
            void calcPath() override;

        private:
            //! A location object representing where the search started.
            Location m_to;

            //! A location object representing where the search ended.
            Location m_from;

            //! A pointer to the CellCache.
            CellCache* m_cellCache;

            //! The start coordinate as an int32_t.
            int32_t m_startCoordInt;

            //! The destination coordinate as an int32_t.
            int32_t m_destCoordInt;

            //! The next coordinate to check out.
            int32_t m_next;

            //! The shortest path tree.
            std::vector<int32_t> m_spt;

            //! The search frontier.
            std::vector<int32_t> m_sf;

            //! A table to hold the costs.
            std::vector<double> m_gCosts;

            //! Priority queue to hold nodes on the sf in order.
            PriorityQueue<int32_t, double> m_sortedfrontier;
    };
} // namespace FIFE
#endif
