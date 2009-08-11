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

#ifndef FIFE_PATHFINDER_SEARCH
#define FIFE_PATHFINDER_SEARCH

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"

namespace FIFE {

	class SearchSpace;

	/** A base class that all searches must derive from.
	 *
	 */
	class Search {
	public:
		typedef std::list<Location> Path;
		/** An enumeration of the different status the search can be in.
		 *
		 */
		enum SearchStatus {
			search_status_failed,
			search_status_complete,
			search_status_incomplete 
		};
		/** Constructor.
		 *
		 * @param session_id The id in the pather that references this session.
		 * @param from The location where the search should be started from.
		 * @param to The location where the search should finish.
		 * @param pather A pointer to the pather controlling this session.
		 */
		Search(const int session_id, const Location& from, const Location& to, SearchSpace* searchspace) 
			: m_to(to), m_from(from), m_sessionId(session_id), m_searchspace(searchspace),
			m_status(search_status_incomplete) {
		}

		/** Destructor.
		 *
		 */
		virtual ~Search() {}

		/** Retrieves the session id.
		 *
		 * @return The searches session id in the pather.
		 */
		int getSessionId() const {
			return m_sessionId;
		}

		/** Retrieves the pather.
		 *
		 * @return A pointer to the abstract pather which
		 */
		SearchSpace* getSearchSpace() const {
			return m_searchspace;
		}

		/** A small function which returns the current status of the search.
		 *
		 * @return An integer value representing the status, which is enumerated by this class.
		 */
		int getSearchStatus() const {
			return m_status;
		}


		/** Updates the search and returns the next part of the path or the entire path if the
		 * algorithm is offline.
		 *
		 * @return A vector of Location objects representing the path.
		 */
		virtual void updateSearch() = 0;

		/** Calculates a path generated by a search.
		 *
		 * Creates a path and returns it.
		 */
		virtual Path calcPath() = 0;
	protected:

		/** Sets the current status of the search.
		 *
		 * @param status The status to set.
		 */
		void setSearchStatus(const SearchStatus status) {
			m_status = status;
		}

		//A location object representing where the search started.
		Location		m_to; 

		//A location object representing where the search ended.
		Location		m_from;

		//An integer containing the session id for this search.
		int				m_sessionId;

		//A pointer to the pather that owns this search.
		SearchSpace*    m_searchspace;

		//An enumeration of the searches current status.
		SearchStatus	m_status;
	};

}

#endif