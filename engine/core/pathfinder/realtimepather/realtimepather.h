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

#ifndef FIFE_PATHFINDER_REALTIME
#define FIFE_PATHFINDER_REALTIME

// Standard C++ library includes
#include <map>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/abstractpather.h"
#include "model/structures/location.h"
#include "model/structures/map.h"

namespace FIFE {

	class Search;
	class SearchSpace;

	class RealTimePather : public AbstractPather {
	public:
		/** Constructor.
		 *
		 */
		RealTimePather() : m_map(0), m_nextFreeSessionId(0), m_maxticks(100) {
			m_ticksleft = m_maxticks;
		}
		/** Sets the map that will be used for the search.
		 *
		 * @param map A pointer to the map to be used. 
		 */
		virtual void setMap(Map* map);

		/** Retrieves the next locations in the search.
		 *
		 * @param curPos A reference to the current location of the requester.
		 * @param target A reference to the target destination of the requester. 
		 * @param nextLocations A reference to a vector that will be filled with
		 *                      intermediate locations. 
		 * @param session_id An integer value representing the session to use, -1
		 *                   is the default value and is used to start a new session. 
		 * @return The session id of the new session. 
		 */
		virtual int getNextLocations(const Location& curPos, const Location& target, 
			std::vector<Location>& nextLocations, const int session_id = -1);

		/** Cancels a given session.
		 *
		 * This function is called when (for instance) the user changes their mind about
		 * a destination while the agent is already moving, the old session needs to be
		 * cancelled and a new one created. 
		 * 
		 * @param session_id The id of the session to cancel.
		 * @return A boolean to signify whether the session was successfully found and cancelled.
		 */
		virtual bool cancelSession(const int session_id);

		/**
		 *
		 */
		virtual void resetTicks() { m_ticksleft = m_maxticks; }
	private:
		typedef std::map<int, Search*> SessionMap;
		typedef std::map<Layer*, SearchSpace*> SearchSpaceMap;

		//The map the search is running on.
		Map*	       m_map;

		//A map of currently running sessions (searches).
		SessionMap	   m_sessions;

		//A map of searchspaces.
		SearchSpaceMap m_searchspaces; 

		//The next free session id.
		int            m_nextFreeSessionId;

		//The amount of ticks left for updating.
		int			   m_ticksleft;

		//The maximum number of ticks allowed.
		int			   m_maxticks;
	};

}

#endif
