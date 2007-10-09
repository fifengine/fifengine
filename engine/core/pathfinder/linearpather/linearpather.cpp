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
#include "linearpather.h"

#include "util/logger.h"

namespace FIFE {
	int LinearPather::getNextLocations(const Location& curpos, const Location& target, 
                                           std::vector<Location>& nextlocations, const int session_id) {									 
		assert(curpos.getElevation() == target.getElevation());
		assert(curpos.getLayer() == target.getLayer());

		ModelCoordinate curpt = curpos.getLayerCoordinates();
		ModelCoordinate tgtpt = target.getLayerCoordinates();

		int dx = tgtpt.x - curpt.x;
		int dy = tgtpt.y - curpt.y;

		Location nextnode(curpos);
		ModelCoordinate p = nextnode.getLayerCoordinates();
		if (dx > 0) {
			p.x += 1;
		}
		if (dx < 0) {
			p.x -= 1;
		}
		if (dy > 0) {
			p.y += 1;
		}
		if (dy < 0) {
			p.y -= 1;
		}
		nextnode.setLayerCoordinates(p);
		nextlocations.clear();
		nextlocations.push_back(nextnode);
		return session_id;
	}
}
