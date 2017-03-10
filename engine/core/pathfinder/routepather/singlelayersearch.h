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

#ifndef FIFE_PATHFINDER_SINGLELAYERSEARCH
#define FIFE_PATHFINDER_SINGLELAYERSEARCH

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/priorityqueue.h"

#include "routepathersearch.h"

namespace FIFE {

	class CellCache;
	class Route;

	/** SingleLayerSearch using A*
	 */
	class SingleLayerSearch: public RoutePatherSearch {
	public:
		/** Constructor
		 *
		 * @param route A pointer to the route for which a path should be searched.
		 * @param sessionId A integer containing the session id for this search.
		 */
		SingleLayerSearch(Route* route, const int32_t sessionId);

		/** Destructor
		 */
		~SingleLayerSearch();

		/** Updates the search.
		 *
		 * Each update checks all neighbors of the last checked coordinate and selects the most favorable.
		 */
		void updateSearch();

		/** Calculates final path.
		 *
		 * If the search is successful then a path is created.
		 */
		void calcPath();

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
}
#endif
