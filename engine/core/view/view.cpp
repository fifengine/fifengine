/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "model/metamodel/timeprovider.h"
#include "model/structures/map.h"
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
		m_pipeline(),
		m_updated(false) {
	}

	View::~View() {
		clearCameras();
		
		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for(; r_it != m_renderers.end(); ++r_it) {
			delete r_it->second;
		}
		m_renderers.clear();
	}

	void View::clearCameras() {
		std::vector<Camera*>::iterator cam_it = m_cameras.begin();
		for(; cam_it != m_cameras.end(); ++cam_it) {
			delete *cam_it;
		}
		m_cameras.clear();
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
			FL_LOG(_log, LMsg("Enabling renderer ") << renderer->getName());
			m_pipeline.push_back(renderer);
			m_pipeline.sort(pipelineSort);
		} else {
			m_pipeline.remove(renderer);
		}
	}


	RendererBase* View::getRenderer(const std::string& name) {
		return m_renderers[name];
	}

	Camera* View::addCamera(const std::string& id, Layer *layer, Rect viewport, ExactModelCoordinate emc) {
		if (getCamera(id))
			throw NameClash("Duplicate camera id registered.");

		Camera* cam = new Camera(id, layer, viewport,emc);
		m_cameras.push_back(cam);
		return cam;
	}

	Camera* View::getCamera(const std::string& id) {
		std::vector<Camera*>::iterator it = m_cameras.begin();
		for (; it != m_cameras.end(); ++it) {
			if ((*it)->getId() == id)
				return *it;
		}
		return 0;
	}

	void View::removeCamera(Camera* camera) {
		std::vector<Camera*>::iterator it = std::find(m_cameras.begin(), m_cameras.end(), camera);
		if (it != m_cameras.end()) {
			m_cameras.erase(it);
		}
	}

	class InstanceDistanceSort {
	public:
		Camera* cam;
		bool operator()(const Instance* lhs, const Instance* rhs) {
			return (lhs->getVisual<InstanceVisual>()->getCacheItem(cam).screenpoint.z <
				rhs->getVisual<InstanceVisual>()->getCacheItem(cam).screenpoint.z);
		}
	};
	
/*	bool instanceDistanceSort(const Instance* lhs, const Instance* rhs) {
		return (lhs->getVisual<InstanceVisual>()->getCacheItem(cam).screenpoint.z <
			rhs->getVisual<InstanceVisual>()->getCacheItem(cam).screenpoint.z);
	}
*/
	bool instanceStackSort(const Instance* lhs, const Instance* rhs) {
		return (lhs->getVisual<InstanceVisual>()->getStackPosition() <
		        rhs->getVisual<InstanceVisual>()->getStackPosition());
	}

	void View::resetRenderers() {
		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for (; r_it != m_renderers.end(); ++r_it) {
			std::vector<Camera*>::iterator cam_it = m_cameras.begin();
			for (; cam_it != m_cameras.end(); ++cam_it) {
				const Location& loc = (*cam_it)->getLocationRef();
				r_it->second->reset();
				r_it->second->activateAllLayers(loc.getMap());
			}
		}
	}
	
	void View::update(ImagePool*m_imagepool,AnimationPool* m_animationpool) {
		FL_DBG(_log, "In View::update");

		// update each camera
		std::vector<Camera*>::iterator cam_it = m_cameras.begin();
		for (; cam_it != m_cameras.end(); ++cam_it) {
			Camera* cam = *cam_it;
			
			if (!cam->isEnabled()) {
				continue;
			}

			cam->update();

			const Location& loc = cam->getLocationRef();
			Map* map = loc.getMap();
			if (!map) {
				FL_ERR(_log, "No map for camera found");
				continue;
			}
			// update each layer
			m_renderbackend->pushClipArea(cam->getViewPort());

			const std::vector<Layer*>& layers = map->getLayers();
			std::vector<Layer*>::const_iterator layer_it = layers.begin();
			for (;layer_it != layers.end(); ++layer_it) {

				// sort instances on layer based on stack position + camera distance. done only once
				//  here instead passing it to each renderer.
				// instances are checked first if their image intersects with the viewport.
				// this reduces processing load during sorting later
				std::vector<Instance*> allinstances((*layer_it)->getInstances());
				std::vector<Instance*>::const_iterator instance_it = allinstances.begin();
				std::vector<Instance*> instances_to_render;
				for (;instance_it != allinstances.end(); ++instance_it) {
					Instance* instance = *instance_it;
					InstanceVisual* visual = instance->getVisual<InstanceVisual>();
					InstanceVisualCacheItem& vc = visual->getCacheItem(cam);
					
					// use cached values if there is no need to do full recalculation
					
					ScreenPoint drawpt;
					//std::cout << m_updated << ", " << (!cam->isWarped()) << ", " << (!instance->isMoved()) << "\n";
					if (m_updated && (!cam->isWarped()) && (!instance->isMoved()) && (vc.image)) {
						ScreenPoint cammove = cam->getLatestMovement();
						int pos_estimate_x = vc.screenpoint.x - cammove.x;
						int pos_estimate_y = vc.screenpoint.y - cammove.y;
						int pos_estimate_z = vc.screenpoint.z - cammove.z;
						//std::cout << "orig x = " << drawpt.x << ", est x = " << pos_estimate_x << "\n";
						//std::cout << "orig y = " << drawpt.y << ", est y = " << pos_estimate_y << "\n";
						drawpt.x = pos_estimate_x;
						drawpt.y = pos_estimate_y;
						drawpt.z = pos_estimate_z;
						//drawpt.z = cam->toScreenCoordinates( instance->getLocationRef().getMapCoordinates() ).z;
					} else {
						drawpt = cam->toScreenCoordinates( instance->getLocationRef().getMapCoordinates() );
					}
					Image* image = NULL;
					Action* action = instance->getCurrentAction();
					int angle = cam->getAngleBetween(instance->getLocationRef(), instance->getFacingLocation());
					if (action) {
						FL_DBG(_log, "Instance has action");
						int animation_id = action->getVisual<ActionVisual>()->getAnimationIndexByAngle(angle);

						Animation& animation = m_animationpool->getAnimation(animation_id);
						int animtime = scaleTime(instance->getTotalTimeMultiplier(), instance->getActionRuntime()) % animation.getDuration();
						image = animation.getFrameByTimestamp(animtime);
					} else {
						FL_DBG(_log, "No action");
						int imageid = vc.getStaticImageIndexByAngle(angle, instance);
						FL_DBG(_log, LMsg("Instance does not have action, using static image with id ") << imageid);
						if (imageid >= 0) {
							image = &m_imagepool->getImage(imageid);
						}
					}
					if (image) {
						vc.image = image;
						vc.screenpoint = drawpt;
						
						int w = image->getWidth();
						int h = image->getHeight();
						drawpt.x -= w / 2;
						drawpt.x += image->getXShift();
						drawpt.y -= h / 2;
						drawpt.y += image->getYShift();
						Rect r = Rect(drawpt.x, drawpt.y, w, h);
						vc.dimensions = r;
						
						if (r.intersects(cam->getViewPort())) {
							instances_to_render.push_back(instance);
						}
					}
				}

				InstanceDistanceSort ids;
				ids.cam = cam;
				std::stable_sort(instances_to_render.begin(), instances_to_render.end(), instanceStackSort);
				std::stable_sort(instances_to_render.begin(), instances_to_render.end(), ids);

				std::list<RendererBase*>::iterator r_it = m_pipeline.begin();
				for (; r_it != m_pipeline.end(); ++r_it) {
					if ((*r_it)->isActivedLayer(*layer_it)) {
						(*r_it)->render(cam, *layer_it, instances_to_render);
					}
				}
			}
			m_renderbackend->popClipArea();
			cam->resetUpdates();
		}
		m_updated = true;
	}
}

