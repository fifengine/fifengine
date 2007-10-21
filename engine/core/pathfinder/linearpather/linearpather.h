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

#ifndef FIFE_PATHFINDER_LINEAR_H
#define FIFE_PATHFINDER_LINEAR_H

// Standard C++ library includes
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"
#include "model/structures/map.h"
#include "model/metamodel/abstractpather.h"

namespace FIFE {

	/** Naive pathfinder implementation
	*/
	class LinearPather: public AbstractPather {
	public:
		LinearPather(): m_session_counter(0), m_map(NULL) {}

		virtual ~LinearPather() {}

		int getNextLocation(const Instance* instance, const Location& target, 
		                    double speed, Location& nextLocation,
		                    Location& facingLocation, int session_id=-1);
		
		bool cancelSession(const int session_id) { 
			m_session2face.erase(session_id);
			return true; 
		}

		std::string getName() const { return "LinearPather"; }
	
	private:
		std::map< int, Location > m_session2face;
		unsigned int m_session_counter;
		Map* m_map;
	};
}

#endif

