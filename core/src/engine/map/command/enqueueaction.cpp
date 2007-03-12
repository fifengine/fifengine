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
#include "map/action.h"
#include "map/map.h"
#include "map/elevation.h"
#include "map/layer.h"
#include "map/geometry.h"
#include "map/objectinfo.h"
#include "map/view.h"
#include "map/visual.h"
#include "video/complexanimation.h"
#include "imagecache.h"
#include "log.h"

#include "enqueueaction.h"

namespace FIFE { namespace map { namespace command {

	class MotionProxy {
		public:
			MotionProxy(Map* map, View* mapview, size_t visualId);
			void operator()(const Action& action);

		private:
			View *m_View;
			Geometry *m_geometry;
			size_t m_visualId;
	};

	MotionProxy::MotionProxy(Map* map, View* mapview, size_t visualId)
		: m_View(mapview), m_visualId(visualId) {
		Location loc(m_View->getVisual(m_visualId)->getLocation());
		m_geometry = map->getElevation(loc.elevation)->getLayer(loc.layer)->getGeometry();
	}

	void MotionProxy::operator()(const Action& action) {
		Visual* v = m_View->getVisual(m_visualId);
		// This fixes segfaults, if the visuals get removed
		// but the animation is one turn() in advance active and
		// get's called ... -> SEGFAULTS at random times. - phoku
		if (!v) {
			return;
		}

		Location loc(v->getLocation());

		Point p0(loc.position);
		loc.position = p0 + m_geometry->directionToGrid(action.direction, p0);

		Debug("motion_proxy")
			<< "called for visual: " << m_visualId
			<< " new posi: " << loc.position
			<< " old posi: " << p0;

		v->setLocation(loc);
		m_View->notifyVisualChanged(m_visualId);
	}

	void EnqueueAction::execute(const Info& cmd) {
		ObjectPtr moi = cmd.object;

		if (!moi) {
			Warn("mc_enqueueaction") << "No MOI for object " << cmd.object;
			return;
		}

		if (!m_view) {
			Log("mc_enqueueaction")	<< "No mapview set.";
			return;
		}

		if (moi->getVisualId() == 0) {
			Warn("mc_enqueueaction") << "No visual id";
			return;
		}
		
		Visual* visual = m_view->getVisual(moi->getVisualId());
		if (!visual) {
			Warn("mc_enqueueaction") << "No visual";
			return;
		}

		ComplexAnimation* anim = visual->getComplexAnimation();
		if (anim) {
			Action action(cmd.params[0], cmd.params[1]);
			if (cmd.params[3]) {
				action.endCallback = MotionProxy(m_map.get(), m_view, moi->getVisualId());
			}
			anim->enqueueAction(action);
		} else {
			Warn("mc_enqueueaction") << "Not an animation?";
		}
	}
} } }
