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
#include "camera.h"
#include "visual.h"

namespace FIFE {
	static Logger _log(LM_VIEWVIEW);
	
	View::View(RenderBackend* renderbackend):
		m_cameras(),
		m_renderbackend(renderbackend),
		m_renderers(),
		m_pipeline() {
	}

	View::~View() {
		std::vector<Camera*>::iterator cam_it = m_cameras.begin();
		for(; cam_it != m_cameras.end(); ++cam_it) {
			delete *cam_it;
		}
		m_cameras.clear();
		
		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for(; r_it != m_renderers.end(); ++r_it) {
			delete r_it->second;
		}
		m_renderers.clear();
	}

	bool pipelineSort(const RendererBase* lhs, const RendererBase* rhs) {
		return (lhs->getPipelinePosition() < rhs->getPipelinePosition());
	}
	
	void View::addRenderer(RendererBase* renderer) {
		renderer->setRendererListener(this);
		m_renderers[renderer->getName()] = renderer;
		if (renderer->isEnabled()) {
			m_pipeline.push_back(renderer);
		}
		m_pipeline.sort(pipelineSort);
	}
	
	void View::onRendererPipelinePositionChanged(RendererBase* renderer) {
		m_pipeline.sort(pipelineSort);
	}
	
	void View::onRendererEnabledChanged(RendererBase* renderer) {
		assert(m_renderers[renderer->getName()]);
		if (renderer->isEnabled()) {
			//std::cout << "Enabling renderer " << renderer->getName() << "\n";
			m_pipeline.push_back(renderer);
		} else {
			m_pipeline.remove(renderer);
		}
	}


	RendererBase* View::getRenderer(const std::string& name) {
		return m_renderers[name];
	}

	Camera* View::addCamera() {
		Camera* cam = new Camera();
		m_cameras.push_back(cam);
		return cam;
	}

	void View::removeCamera(Camera* camera) {
		std::vector<Camera*>::iterator it = std::find(m_cameras.begin(), m_cameras.end(), camera);
		if (it != m_cameras.end()) {
			m_cameras.erase(it);
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
		
	void View::resetRenderers() {
		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for(; r_it != m_renderers.end(); ++r_it) {
			std::vector<Camera*>::iterator cam_it = m_cameras.begin();
			for(; cam_it != m_cameras.end(); ++cam_it) {
				const Location& loc = (*cam_it)->getLocation();
				r_it->second->activateAllLayers(loc.getElevation());
			}
		}
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
			//std::cout << "Drawing camera" << "\n";
			// update each layer
			m_renderbackend->pushClipArea((*cam_it)->getViewPort());
			
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
				
				//std::cout << "  Drawing layer " << (*layer_it)->Id() << "\n";
				// asks renderers to draw the layer
				std::set<int>::const_iterator stack_it = stack_positions.begin();
				for (;stack_it != stack_positions.end(); ++stack_it) {
					//std::cout << "    stackpos " << *stack_it << "\n";
					std::list<RendererBase*>::iterator r_it = m_pipeline.begin();
					for(; r_it != m_pipeline.end(); ++r_it) {
						if ((*r_it)->isActivedLayer(*layer_it)) {
							//std::cout << "      Drawing " << (*r_it)->getName() << "\n";
							(*r_it)->render(*cam_it, *layer_it, instances, *stack_it);
						}
					}

				}
			}
			m_renderbackend->popClipArea();
		}
	}
}

