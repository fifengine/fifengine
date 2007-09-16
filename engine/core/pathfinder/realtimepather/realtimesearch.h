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

#ifndef FIFE_PATHFINDER_REALTIMESEARCH
#define FIFE_PATHFINDER_REALTIMESEARCH

#include "pathfinder/search.h"

namespace FIFE {

	class Map;

	/** RealTimeSearch using A*
	 *
	 * For now this class uses offline A*, however eventually this will be switched over to RTA*.
	 */
	class RealTimeSearch : public Search {
	public:
		RealTimeSearch(int session_id, const Location& from, const Location& to, AbstractPather* pather) 
			: Search(session_id, from, to, pather) {
		}

		virtual std::vector<Location> updateSearch();
	private:
		//The shortest path tree.
		std::vector<int> m_spt;
		//The search frontier.
		std::vector<int> m_sf;

		//TODO: Add more pathfinding related stuff.
	};

}

#endif
