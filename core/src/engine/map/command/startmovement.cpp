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
#include "map/map.h"
#include "map/objectinfo.h"
#include "map/objectmanager.h"
#include "map/view.h"
#include "map/visual.h"
#include "imagecache.h"
#include "log.h"

#include "startmovement.h"

namespace FIFE { namespace map { namespace command {

	void StartMovement::execute(const Info& cmd) {
		ObjectPtr moi = cmd.object;
		long x = cmd.params[0];
		long y = cmd.params[1];

		if (!moi) {
			Warn("mc_startmovement") << "No MOI for object " << cmd.object;
			return;
		}

		if (!m_map->isValidLocation(moi->getLocation())) {
			Warn("mc_startmovement") 
				<< "Cannot move visual for object not on map.";
			return;
		}

		moi->getLocation().position.x += x;
		moi->getLocation().position.y += y;
		if (moi->getLocation().elevation != m_view->getElevationNum()) {
			Log("mc_startmovement") 
				<< "Will just set location for object not on mapview";
			return;
		}

		if (m_view) {
			Visual* visual = m_view->getVisual(moi->getVisualId());
			if (visual == 0) {
				Warn("mc_startmovement")
					<< "No Visual set for object.";
				return;
			}
		} else {
			moi->setVisualId(0);
		}
	}

} } }
