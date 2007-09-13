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

namespace FIFE {

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
			return -1;
	}
}