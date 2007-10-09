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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "pathfinder/searchspace.h"

#include "realtimepather.h"
#include "realtimesearch.h"

#include "util/logger.h"

namespace FIFE {
	void RealTimePather::setMap(Map* map) {
		if(!map) {
			return;
		}
		m_map = map;
	}

	int RealTimePather::getNextLocations(const Location& curPos, const Location& target, 
		std::vector<Location>& nextLocations, const int session_id) {
			//Make sure that we're not navigating to the same tile.
			if((curPos.getLayerCoordinates() == target.getLayerCoordinates()) &&
				curPos.getLayer() == target.getLayer()) {
				return -1;
			}
			//A session_id was passed in, therefore the path search has already begun.
			if(session_id != -1) {
				//search session map for id.
				SessionMap::iterator i = m_sessions.find(session_id);
				if( i != m_sessions.end() ) {
					//update search.
					while(m_ticksleft) {
						nextLocations = i->second->updateSearch();
						//If the search has finished terminate the session.
						int search_status = i->second->getSearchStatus();
						if(search_status == Search::search_status_complete || search_status == Search::search_status_failed ) {
							delete i->second;
							m_sessions.erase(i);
							return -1; //Search is complete, return -1.
						}
						--m_ticksleft;
					}
					return session_id;
				}
			}
			//First get the layer
			if(curPos.getLayer() != target.getLayer()) {
				//Don't allow cross layer movement (for now).
				return -1;
			}
			//Search for the layer in the searchspace map.
			SearchSpaceMap::iterator i = m_searchspaces.find(curPos.getLayer());
			if(i == m_searchspaces.end()) {
				//This layer has never been searched before so create a searchspace.
				SearchSpace* newSpace = new SearchSpace(curPos.getLayer());
				i = m_searchspaces.insert(SearchSpaceMap::value_type(curPos.getLayer(), newSpace)).first;

			}
			//Create a new session.
			int newSessionId = m_nextFreeSessionId++;
			if(i->second->isInSearchSpace(curPos) && i->second->isInSearchSpace(target)) {
				SessionMap::value_type newSession(newSessionId, new RealTimeSearch(newSessionId, curPos, target, i->second));
				m_sessions.insert(newSession);
				return newSessionId;
			}
			return -1;
	}

	bool RealTimePather::cancelSession(const int session_id) {
		if(session_id >= 0) {
			SessionMap::iterator i = m_sessions.find(session_id);
			if(i != m_sessions.end()) {
				delete i->second;
				m_sessions.erase(i);
				return true;
			}
		}
		return false;
	}
}
