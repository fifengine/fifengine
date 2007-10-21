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
#include <iostream>
#include <algorithm>
#include <set>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "video/animation.h"
#include "video/animationpool.h"

#include "util/fife_math.h"
#include "util/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/action.h"
#include "model/structures/elevation.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view.h"
#include "abstractrenderer.h"
#include "camera.h"
#include "visual.h"

namespace FIFE {
	static Logger _log(LM_VIEWVIEW);
	
	View::View():
		m_cameras(),
		m_renderers() {
	}

	View::~View() {
		std::vector<Camera*>::iterator cam_it = m_cameras.begin();
		for(; cam_it != m_cameras.end(); ++cam_it) {
			delete *cam_it;
		}
		m_cameras.clear();
		
		std::vector<AbstractRenderer*>::iterator r_it = m_renderers.begin();
		for(; r_it != m_renderers.end(); ++r_it) {
			delete *r_it;
		}
		m_renderers.clear();
	}

	void View::addRenderer(AbstractRenderer* renderer) {
		m_renderers.push_back(renderer);
	}

	void View::removeRenderer(AbstractRenderer* renderer) {
		std::vector<AbstractRenderer*>::iterator it = m_renderers.begin();
		for(; it != m_renderers.end(); ++it) {
			if((*it) == renderer) {
				m_renderers.erase(it);
				return;
			}
		}
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
	
	bool instanceDistanceSort(const Instance* lhs, const Instance* rhs) {
		return (lhs->getVisual<InstanceVisual>()->getCameraCoordinate().z <
		        rhs->getVisual<InstanceVisual>()->getCameraCoordinate().z);
	}
	
	bool instanceStackSort(const Instance* lhs, const Instance* rhs) {
		return (lhs->getVisual<InstanceVisual>()->getStackPosition() <
		        rhs->getVisual<InstanceVisual>()->getStackPosition());
	}
		
	void View::update() {
		FL_DBG(_log, "In View::update");
		
		// update each camera
		std::vector<Camera*>::iterator cam_it = m_cameras.begin();
		for(; cam_it != m_cameras.end(); ++cam_it) {
			const Location& loc = (*cam_it)->getLocation();
			Elevation* elev = loc.getElevation();
			if (!elev) {
				FL_ERR(_log, "No elevation for camera found");
				continue;
			}
			
			// update each layer
			const std::vector<Layer*>& layers = elev->getLayers();
			std::vector<Layer*>::const_iterator layer_it = layers.begin();
			for (;layer_it != layers.end(); ++layer_it) {
			
				// sort instances on layer based on stack position + camera distance. done only once
				//  here instead passing it to each renderer
				std::set<int> stack_positions;				
				std::vector<Instance*> instances((*layer_it)->getInstances());
				std::vector<Instance*>::const_iterator instance_it = instances.begin();
				for (;instance_it != instances.end(); ++instance_it) {
					Instance* instance = *instance_it;
					InstanceVisual* visual = instance->getVisual<InstanceVisual>();
					ExactModelCoordinate c = instance->getLocation().getElevationCoordinates();
					visual->setCameraCoordinate((*cam_it)->toScreenCoordinates(c));
					stack_positions.insert(visual->getStackPosition());
				}
				std::sort(instances.begin(), instances.end(), instanceDistanceSort);
				std::sort(instances.begin(), instances.end(), instanceStackSort);
				
				// asks renderers to draw the layer
				std::set<int>::const_iterator stack_it = stack_positions.begin();
				for (;stack_it != stack_positions.end(); ++stack_it) {
					std::vector<AbstractRenderer*>::iterator rend_it = m_renderers.begin();
					for(; rend_it != m_renderers.end(); ++rend_it) {
						(*rend_it)->render(*cam_it, *layer_it, instances, *stack_it);
					}
				}
			}
		}
	}
}

