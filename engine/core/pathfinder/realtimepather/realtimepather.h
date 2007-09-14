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

#include <map>
#include <vector>

#include "model/structures/location.h"
#include "model/structures/map.h"
#include "pathfinder/abstractpather.h"

namespace FIFE {

	class Search;

	class RealTimePather : public AbstractPather {
	public:
		RealTimePather() { }

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

	private:
		typedef std::map<int, Search*> SessionMap;

		//The map the search is running on.
		Map*	   m_map;

		//A map of currently running sessions (searches).
		SessionMap m_sessions;

		//The next free session id.
		int m_nextfreeId;

	};

}

#endif