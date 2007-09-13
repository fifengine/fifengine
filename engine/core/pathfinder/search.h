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
		/** Constructor.
		 *
		 */
		Search(int session_id, AbstractPather* pather) : m_sessionId(session_id), m_pather(pather) {
		}

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

		/** Updates the search and returns the next part of the path or the entire path if the
		 * algorithm is offline.
		 *
		 * @return A vector of Location objects representing the path.
		 */
		virtual std::vector<Location> updateSearch() = 0;
	private:
		//An integer containing the session id for this search.
		int m_sessionId;

		//A pointer to the pather that owns this search.
		AbstractPather* m_pather;
	};

}

#endif