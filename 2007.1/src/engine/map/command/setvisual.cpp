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
#include "map/view.h"
#include "map/visual.h"
#include "video/complexanimation.h"
#include "imagecache.h"
#include "log.h"

#include "setvisual.h"

namespace FIFE { namespace map { namespace command {

	void SetVisual::execute(const Info& cmd) {
		ObjectPtr moi = cmd.object;

		if (!moi) {
			Warn("mc_setvisual") << "No MOI for object " << cmd.object;
			return;
		}

		if (!moi->getLayer()) {
			Warn("mc_setvisual") 
				<< "Cannot set visual for object not on map: "
				<< moi->getLocation();
			return;
		}

		if (moi->getElevation() != m_view->getElevationNum()) {
			Log("mc_setvisual") << "Setting visual for object not on map view.";
			moi->setVisualLocation(cmd.stringParam);
			return;
		}

		if (cmd.stringParam == "") {
			Log("mc_setvisual") << "Setting null-visual for object " << cmd.object;
			if (moi->getVisualId() && m_view) {
				m_view->removeVisual(moi->getVisualId());
			}
			moi->setVisualId(0);
			return;
		}

		if (m_view) { 
			Visual* visual;
			if (moi->getVisualId()) {
				visual = m_view->getVisual(moi->getVisualId());
			} else {
				visual = new Visual(moi);
			}

			RenderableLocation loc(cmd.stringParam);
			loc.setType(RenderAble::RT_COMPLEX_ANIMATION);
			loc.setDirection(cmd.params[0]);
			moi->setVisualLocation(loc);

			visual->setRenderable(
				ImageCache::instance()->addImageFromLocation(loc),
				loc.getType() );
			ComplexAnimation* anim = visual->getComplexAnimation();
			if (anim) {
				anim->enqueueAction(Action(0, cmd.params[0]));
			}

			Debug("mc_setvisual") 
				<< "Setting Visual: " << cmd.stringParam
				<< " for object type: " 
				<< moi->get<std::string>(ObjectInfo::ObjectTypeParam);
			if (!moi->getVisualId()) {
				moi->setVisualId( m_view->addVisual(visual) );
			} else {
				if (m_view->getVisual(moi->getVisualId())) {
					m_view->notifyVisualChanged(moi->getVisualId());
				} else {
					moi->setVisualId( m_view->addVisual(visual) );
				}
			}
		} else { // !m_view
			moi->setVisualId(0);
		}
		moi->set<long>("_visual_id",moi->getVisualId());
		moi->set<std::string>("_visual_path",moi->getVisualLocation().getFilename());
	}
} } }
