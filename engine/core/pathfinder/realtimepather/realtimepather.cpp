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

#include "realtimepather.h"
#include "realtimesearch.h"
#include "pathfinder/searchspace.h"

namespace FIFE {

	void RealTimePather::calculateSearchSpace() {
		//TODO: Finish this function.
	}

	void RealTimePather::setMap(Map* map) {
		if(!map) {
			return;
		}
		//TODO: Put some additional error checking here. 
		m_map = map;
	}

	int RealTimePather::getNextLocations(const Location& curPos, const Location& target, 
		std::vector<Location>& nextLocations, const int session_id) {
			//TODO: Finish this function. This function will simply locate the search with the given
			//session id and then update it and return the given path. If no session with given id exists
			//create a new one.
			if(curPos == target) {
				return -1;
			}
			if(session_id != -1) {
				//search session map for id.
				SessionMap::iterator i = m_sessions.find(session_id);
				if( i != m_sessions.end() ) {
					//update search.
					nextLocations = i->second->updateSearch();
					//If the search has finished terminate the session.
					int search_status = i->second->getSearchStatus();
					if(search_status == Search::search_status_complete || search_status == Search::search_status_failed ) {
						delete i->second;
						m_sessions.erase(i);
						return search_status;
					}
					return session_id;
				}
			}
			int newSessionId = m_nextFreeSessionId++;
			if(m_searchspace->isInSearchSpace(curPos) && m_searchspace->isInSearchSpace(target)) {
				SessionMap::value_type newSession(newSessionId, new RealTimeSearch(newSessionId, curPos, target, this));
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