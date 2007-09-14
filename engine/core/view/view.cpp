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

namespace FIFE {
	static Logger _log(LM_VIEWVIEW);
	
	View::View(RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool):
		m_cameras(),
		m_renderbackend(renderbackend),
		m_imagepool(imagepool),
		m_animationpool(animpool) {
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
	
	int View::getAngleBetween(const Location& loc1, const Location& loc2, Camera& cam) {
		ScreenPoint pt1 = cam.toScreenCoordinates(loc1.getElevationCoordinates());
		ScreenPoint pt2 = cam.toScreenCoordinates(loc2.getElevationCoordinates());
		double dy = pt2.y - pt1.y;
		double dx = pt2.x - pt1.x;
		
		int angle = static_cast<int>(atan2(dy,dx)*(180.0/M_PI));
		FL_DBG(_log, LMsg("-> angle, pt1=") << pt1 << ", pt2=" << pt2 << ", angle=" << angle);
		if (dy > 0) {
			return 360 - angle;
		} else {
			return -angle;
		}
	}
	
	void View::updateCamera(Camera* camera) {
		FL_DBG(_log, "In View::updateCamera");
		const Location& loc = camera->getLocation();
		Elevation* elev = loc.getElevation();
		if (!elev) {
			FL_DBG(_log, "No elevation found, exiting");
			return;
		}
		const std::vector<Layer*>& layers = elev->getLayers();
		std::vector<Layer*>::const_iterator layer_it = layers.begin();
		while (layer_it != layers.end()) {
			FL_DBG(_log, "Iterating layer...");
			Layer* layer = (*layer_it);
			CellGrid* cg = layer->getCellGrid();
			if (!cg) {
				FL_DBG(_log, "No cellgrid assigned to layer...");
				++layer_it;
			}
			FL_DBG(_log, "Getting instance...");
			const std::vector<Instance*>& instances = layer->getInstances();
			std::vector<Instance*>::const_iterator instance_it = instances.begin();
			while (instance_it != instances.end()) {
				FL_DBG(_log, "Iterating instances...");
				Instance* instance = (*instance_it);
				Image* image = NULL;
				ExactModelCoordinate elevpos = instance->getLocation().getElevationCoordinates();
				ScreenPoint campos = camera->toScreenCoordinates(elevpos);
				
				FL_DBG(_log, LMsg("Instance layer coordinates = ") << instance->getLocation().getLayerCoordinates());
				FL_DBG(_log, LMsg("Instance elevation position = ") << elevpos);
				FL_DBG(_log, LMsg("Instance camera position = ") << campos);
				
				Action* action = instance->getCurrentAction();
				if (action) {
					const Location& facing_loc = instance->getFacingLocation();
					FL_DBG(_log, "Instance has action");
					int angle = getAngleBetween(instance->getLocation(), instance->getFacingLocation(), *camera);
					int animation_id = action->getAnimationIndexByAngle(angle);
					Animation& animation = m_animationpool->getAnimation(animation_id);
					int animtime = instance->getActionRuntime() % animation.getDuration();
					image = animation.getFrameByTimestamp(animtime);
					FL_DBG(_log, LMsg("Instance facing location = ") << facing_loc);
					FL_DBG(_log, LMsg("Calculated angle = ") << angle);
				} else {
					FL_DBG(_log, "No action");
					int static_rotation = static_cast<int>(cg->getRotation() + camera->getRotation());
					int imageid = instance->getStaticImageIndexByAngle(static_rotation);
					FL_DBG(_log, LMsg("Instance does not have action, using static image with id ") << imageid);
					if (imageid >= 0) {
						image = &m_imagepool->getImage(imageid);
					}
				}
				if (image) {
					ExactModelCoordinate exact_elevpos = instance->getLocation().getElevationCoordinates();
					ScreenPoint drawpt = camera->toScreenCoordinates(exact_elevpos);

					int w = image->getWidth();
					int h = image->getHeight();
					drawpt.x -= w / 2;
					drawpt.x += image->getXShift();
					drawpt.y -= h / 2;
					drawpt.y += image->getYShift();
					Rect r = Rect(drawpt.x, drawpt.y, w, h);
					FL_DBG(_log, LMsg("image(") << r << "), viewport (" << camera->getViewPort());
					if (r.intersects(camera->getViewPort())) {
						FL_DBG(_log, "Instance is visible in viewport, rendering");
						image->render(r);
					} else {
						FL_DBG(_log, "Instance is not visible in viewport, skipping");
					}

					// draw grid for testing purposes
					std::vector<ExactModelCoordinate> vertices;
					cg->getVertices(vertices, instance->getLocation().getLayerCoordinates());
					std::vector<ExactModelCoordinate>::const_iterator it = vertices.begin();
					ScreenPoint firstpt = camera->toScreenCoordinates(cg->toElevationCoordinates(*it));
					Point pt1(firstpt.x, firstpt.y);
					Point pt2;
					++it;
					while (it != vertices.end()) {
						ScreenPoint pts = camera->toScreenCoordinates(cg->toElevationCoordinates(*it));
						pt2.x = pts.x; pt2.y = pts.y;
						m_renderbackend->drawLine(pt1, pt2, 0, 255, 0);
						pt1 = pt2;
						++it;
					}
					m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), 0, 255, 0);
				}
				else {
					FL_DBG(_log, "Instance does not have image to render");
				}
				++instance_it;
			}
			++layer_it;
		}
	}

	void View::update() {
		std::vector<Camera*>::iterator it = m_cameras.begin();
		for(; it != m_cameras.end(); ++it) {
			updateCamera(*it);
		}
	}
}

