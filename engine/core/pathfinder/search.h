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

#ifndef FIFE_PATHFINDER_SEARCH
#define FIFE_PATHFINDER_SEARCH

#include <vector>

#include "model/structures/location.h"

namespace FIFE {

	class AbstractPather;

	/** A base class that all searches must derive from.
	 *
	 */
	class Search {
	public:
		/** An enumeration of the different status the search can be in.
		 *
		 */
		enum SearchStatus {
			search_status_failed = -2,
			search_status_complete = -1,
			search_status_incomplete 
		};
		/** Constructor.
		 *
		 * @param session_id The id in the pather that references this session.
		 * @param from The location where the search should be started from.
		 * @param to The location where the search should finish.
		 * @param pather A pointer to the pather controlling this session.
		 */
		Search(const int session_id, const Location& from, const Location& to, AbstractPather* pather) 
			: m_sessionId(session_id), m_pather(pather), m_status(search_status_incomplete) {
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
		AbstractPather* getPather() const {
			return m_pather;
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
		virtual std::vector<Location> updateSearch() = 0;
	protected:

		/** Sets the current status of the search.
		 *
		 * @param status The status to set.
		 */
		void setSearchStatus(const SearchStatus status) {
			m_status = status;
		}

		virtual std::vector<Location> calcPath() = 0;

		//A location object representing where the search started.
		Location		m_to; 

		//A location object representing where the search ended.
		Location		m_from;

		//An integer containing the session id for this search.
		int				m_sessionId;

		//A pointer to the pather that owns this search.
		AbstractPather* m_pather;

		//An enumeration of the searches current status.
		SearchStatus	m_status;
	};

}

#endif
