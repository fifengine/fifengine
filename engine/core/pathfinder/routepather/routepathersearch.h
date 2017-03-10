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

#ifndef FIFE_PATHFINDER_ROUTEPATHERSEARCH
#define FIFE_PATHFINDER_ROUTEPATHERSEARCH

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/priorityqueue.h"

namespace FIFE {

	class CellCache;
	class Route;

	/** RoutePatherSearch using A*
	 *
	 */
	class RoutePatherSearch {
	public:
		/** Constructor
		 *
		 * @param route A pointer to the route for which a path should be searched.
		 * @param sessionId A integer containing the session id for this search.
		 */
		RoutePatherSearch(Route* route, const int32_t sessionId);

		virtual ~RoutePatherSearch();

		/** An enumeration of the different status the search can be in.
		 *
		 */
		enum SearchStatus {
			search_status_failed,
			search_status_complete,
			search_status_incomplete
		};

		/** Updates the search.
		 *
		 * Each update checks all neighbors of the last checked coordinate and selects the most favorable.
		 */
		virtual void updateSearch() = 0;

		/** Calculates final path.
		 *
		 * If the search is successful then a path is created.
		 */
		virtual void calcPath() = 0;

		/** Retrieves the session id.
		 *
		 * @return The searches session id in the pather.
		 */
		int32_t getSessionId() const;

		/** A small function which returns the current status of the search.
		 *
		 * @return An integer value representing the status, which is enumerated by this class.
		 */
		int32_t getSearchStatus() const;

		/** Returns the associated route for this search.
		 *
		 * @return A pointer to the route.
		 */
		Route* getRoute();

	protected:
		/** Sets the current status of the search.
		 *
		 * @param status The status to set.
		 * @see SearchStatus
		 */
		void setSearchStatus(const SearchStatus status);

		//! Pointer to route
		Route* m_route;

		//! Indicates if the search should use special costs.
		bool m_specialCost;

		//! Indicates if the route is for a multi cell object.
		bool m_multicell;

		//! Indicates if dynamic blockers should be ignored.
		bool m_ignoreDynamicBlockers;

		//! Blockers from a multi cell object which should be ignored.
		std::vector<Cell*> m_ignoredBlockers;

	private:
		//! An integer containing the session id for this search.
		int32_t m_sessionId;

		//! An enumeration of the searches current status.
		SearchStatus m_status;
	};
}
#endif
