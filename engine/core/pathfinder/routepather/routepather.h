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

#ifndef FIFE_PATHFINDER_ROUTEPATHER
#define FIFE_PATHFINDER_ROUTEPATHER

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
	class RoutePatherSearch;

	class RoutePather : public AbstractPather {
	public:
		/** Constructor.
		 *
		 */
		RoutePather() : m_map(0), m_nextFreeSessionId(0), m_maxticks(100) {
			m_ticksleft = m_maxticks;
		}
		
		void setMap(Map* map);
		int getNextLocation(const Instance* instance, const Location& target, 
		                    double distance_to_travel, Location& nextLocation,
		                    Location& facingLocation, int session_id=-1);

		bool cancelSession(const int session_id);
		std::string getName() const { return "RoutePather"; };
		
		void resetTicks() { m_ticksleft = m_maxticks; }
		
	private:
		typedef std::list<Location> Path;
		typedef std::map<int, Search*> SessionMap;
		typedef std::map<int, Path> PathMap;
		typedef std::map<Layer*, SearchSpace*> SearchSpaceMap;

		/**
		 *
		 */
		void followPath(const Instance* instance, Path& path, double speed, Location& nextLocation, Location& facingLocation);

		//The map the search is running on.
		Map*	       m_map;

		//A map of currently running sessions (searches).
		SessionMap	   m_sessions;

		//Calculated paths for the movement phase.
		PathMap		   m_paths;

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
