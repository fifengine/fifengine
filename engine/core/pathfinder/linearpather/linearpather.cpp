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

namespace FIFE { namespace model {
	int LinearPather::getNextNode(const Location& curpos, const Location& target,
	                                      Location& nextnode, const int session_id) {
		assert(curpos.elevation == target.elevation);
		assert(curpos.layer == target.layer);

		int dx = target.position.x - curpos.position.x;
		int dy = target.position.y - curpos.position.y;

		nextnode.elevation = curpos.elevation;
		nextnode.layer = curpos.layer;

		Point& p = nextnode.position;
		if (dx > 0) {
			p.x = curpos.position.x + 1;
		}
		if (dx < 0) {
			p.x = curpos.position.x - 1;
		}
		if (dy > 0) {
			p.y = curpos.position.y + 1;
		}
		if (dy < 0) {
			p.y = curpos.position.y - 1;
		}
		return session_id;
	}
}}
