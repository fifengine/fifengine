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
#include "video/renderbackend.h"
#include "util/fife_math.h"

#include "view.h"
#include "camera.h"

namespace FIFE {
	View::View(RenderBackend* renderbackend):
		m_cameras(),
		m_renderbackend(renderbackend) {
	}

	View::~View() {
		std::vector<Camera*>::iterator it = m_cameras.begin();
		for(; it != m_cameras.end(); ++it) {
			delete *it;
		}
		m_cameras.clear();
	}

	void View::addCamera(Camera* camera) {
		m_cameras.push_back(camera);
	}

	void View::removeCamera(Camera* camera) {
		std::vector<Camera*>::iterator it = m_cameras.begin();
		for(; it != m_cameras.end(); ++it) {
			if((*it) == camera) {
				m_cameras.erase(it);
				return;
			}
		}
	}
	
/**
INITIAL PSEUDO CODE FOR UPDATES

def update():
	for camera in cameras:
		for layer in elevation:
			drawLayer(layer, camera)
			

def drawLayer(layer, camera):
	cellgrid = layer.getCellGrid()
	for instance in layer:
		image = None
		action = instance.getCurrentAction()
		if action:

			# !!! resolve angle based on camera, layer and instance orientations
			angle = ...

			animationid = action.getAnimationIndexByAngle(angle)
			image = animationpool.getAnimation(animationid).getFrameByTimestamp(instance.action_runtime)
		else:
			image = imagepool.getImage(instance.getStaticImageId())

		if image:
			logical_position = DoublePoint(instance.getLocation().position)
			offset_distance = instance.getOffsetDistance()
			if offset_distance:
				logical_position = cellgrid.getOffset(position , instance.getFacingCell(), offset_distance)
			
			# !!! resolve screen coordinates based on camera and layer orienation/zoom + instance position
			coords = ...

			r = Rect(coords.x, coords.y, image.getWidth() * zoom, image.getHeight() * zoom)
			image.render(r)

*/	


	void View::update() {
		std::vector<Camera*>::iterator it = m_cameras.begin();
		for(; it != m_cameras.end(); ++it) {
			Camera* cam = (*it);
			const Location& loc = cam->getLocation();

			m_renderbackend->drawLine(Point(1,1), Point(100,100), 50, 50, 50);
			
		}
	}
}

