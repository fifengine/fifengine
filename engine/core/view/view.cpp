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
		for (; r_it != m_renderers.end(); ++r_it) {
			std::vector<Camera*>::iterator cam_it = m_cameras.begin();
			for (; cam_it != m_cameras.end(); ++cam_it) {
				const Location& loc = (*cam_it)->getLocation();
				r_it->second->reset();
				r_it->second->activateAllLayers(loc.getElevation());
			}
		}
	}

	void View::update(ImagePool*m_imagepool,AnimationPool* m_animationpool) {
		FL_DBG(_log, "In View::update");

		// update each camera
		std::vector<Camera*>::iterator cam_it = m_cameras.begin();
		for (; cam_it != m_cameras.end(); ++cam_it) {
			if (!(*cam_it)->isEnabled()) {
				continue;
			}

			(*cam_it)->update();

			const Location& loc = (*cam_it)->getLocation();
			Elevation* elev = loc.getElevation();
			if (!elev) {
				FL_ERR(_log, "No elevation for camera found");
				continue;
			}
			// update each layer
			m_renderbackend->pushClipArea((*cam_it)->getViewPort());

			const std::vector<Layer*>& layers = elev->getLayers();
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

					CellGrid* cg = instance->getLocation().getLayer()->getCellGrid();
					if (!cg) {
						FL_WARN(_log, "No cellgrid assigned to layer, cannot draw instances");
						exit(0);
						return;
					}
					ScreenPoint drawpt = (*cam_it)->toScreenCoordinates( instance->getLocation().getElevationCoordinates() );
					Image* image = NULL;
					Action* action = instance->getCurrentAction();
					int angle = (*cam_it)->getAngleBetween(instance->getLocation(), instance->getFacingLocation());
					InstanceVisual* visual = instance->getVisual<InstanceVisual>();
					if (action) {
						FL_DBG(_log, "Instance has action");
						int animation_id = action->getVisual<ActionVisual>()->getAnimationIndexByAngle(angle);

						Animation& animation = m_animationpool->getAnimation(animation_id);
						int animtime = instance->getActionRuntime() % animation.getDuration();
						image = animation.getFrameByTimestamp(animtime);
					} else {
						FL_DBG(_log, "No action");
						int imageid = visual->getStaticImageIndexByAngle(angle);
						FL_DBG(_log, LMsg("Instance does not have action, using static image with id ") << imageid);
						if (imageid >= 0) {
							image = &m_imagepool->getImage(imageid);
						}
					}
					if (image) {
						int w = image->getWidth();
						int h = image->getHeight();
						drawpt.x -= w / 2;
						drawpt.x += image->getXShift();
						drawpt.y -= h / 2;
						drawpt.y += image->getYShift();
						Rect r = Rect(drawpt.x, drawpt.y, w, h);

						if (!r.intersects((*cam_it)->getViewPort()))
							continue;

						visual->setCachedImageDimensions(r);
						visual->setCameraCoordinate(drawpt);
						visual->setCachedImage(image);
						instances_to_render.push_back(instance);
					}
				}

				std::stable_sort(instances_to_render.begin(), instances_to_render.end(), instanceStackSort);
				std::stable_sort(instances_to_render.begin(), instances_to_render.end(), instanceDistanceSort);

				std::list<RendererBase*>::iterator r_it = m_pipeline.begin();
				for (; r_it != m_pipeline.end(); ++r_it) {
					if ((*r_it)->isActivedLayer(*layer_it)) {
						(*r_it)->render(*cam_it, *layer_it, instances_to_render);
					}
				}
			}
			m_renderbackend->popClipArea();
		}
	}
}

