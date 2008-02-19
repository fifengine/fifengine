/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_PATHFINDER_ROUTEPATHERSEARCH
#define FIFE_PATHFINDER_ROUTEPATHERSEARCH

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "pathfinder/search.h"
#include "util/priorityqueue.h"

namespace FIFE {

	class Map;
	class SearchSpace;
	class Heuristic;

	/** RoutePatherSearch using A*
	 *
	 * For now this class uses offline A*, however eventually this will be switched over to RTA*.
	 */
	class RoutePatherSearch : public Search {
	public:
		RoutePatherSearch(const int session_id, const Location& from, const Location& to, SearchSpace* searchSpace);

		virtual void updateSearch();

		virtual Path calcPath();
	private:
		//The class to use to calculate the heuristic value.
		Heuristic*                m_heuristic;
		//The destination coordinate as an int.
		int                       m_destCoordInt;
		//The start coordinate as an int.
		int                       m_startCoordInt;
		//The next coordinate int to check out.
		int                       m_next;
		//The shortest path tree.
		std::vector<int>          m_spt;
		//The search frontier.
		std::vector<int>	      m_sf;
		//A table to hold the costs.
		std::vector<float>		  m_gCosts;
		//priority queue to hold nodes on the sf in order. 
		PriorityQueue<int, float> m_sortedfrontier;
	};

}

#endif
