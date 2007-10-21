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
#include "video/image.h"
#include "video/imagepool.h"
#include "video/animation.h"
#include "video/animationpool.h"
#include "util/logger.h"

#include "util/fife_math.h"
#include "util/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/action.h"
#include "model/structures/elevation.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "view/visual.h"
#include "instancerenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);
	
	InstanceRenderer::InstanceRenderer(RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool):
		m_renderbackend(renderbackend),
		m_imagepool(imagepool),
		m_animationpool(animpool) {
	
	}
	
	InstanceRenderer::~InstanceRenderer() {
	}
	
	int InstanceRenderer::getAngleBetween(const Location& loc1, const Location& loc2, Camera& cam) {
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
	
	void InstanceRenderer::render(Camera* camera, Layer* layer, stackpos2instances_t* instance_stack, int stackpos) {
		FL_DBG(_log, "Iterating layer...");
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw instances");
			return;
		}

		stackpos2instances_t::const_iterator i = instance_stack->find(stackpos);
		const std::vector<Instance*>& instances = i->second;
		std::vector<Instance*>::const_iterator instance_it = instances.begin();
		for (;instance_it != instances.end(); ++instance_it) {
			FL_DBG(_log, "Iterating instances...");
			Instance* instance = (*instance_it);
			Image* image = NULL;
			ExactModelCoordinate elevpos = instance->getLocation().getElevationCoordinates();
			ScreenPoint campos = camera->toScreenCoordinates(elevpos);
			
			FL_DBG(_log, LMsg("Instance layer coordinates = ") << instance->getLocation().getLayerCoordinates());
			FL_DBG(_log, LMsg("Instance elevation position = ") << elevpos);
			FL_DBG(_log, LMsg("Instance camera position = ") << campos);
			
			Action* action = instance->getCurrentAction();
			const Location& facing_loc = instance->getFacingLocation();
			int angle = getAngleBetween(instance->getLocation(), instance->getFacingLocation(), *camera);
			FL_DBG(_log, LMsg("Rendering instance with angle ") << angle);
			if (action) {
				FL_DBG(_log, "Instance has action");
				int animation_id = action->getVisual<ActionVisual>()->getAnimationIndexByAngle(angle);
				Animation& animation = m_animationpool->getAnimation(animation_id);
				int animtime = instance->getActionRuntime() % animation.getDuration();
				image = animation.getFrameByTimestamp(animtime);
				FL_DBG(_log, LMsg("Instance facing location = ") << facing_loc);
				FL_DBG(_log, LMsg("Calculated angle = ") << angle);
			} else {
				FL_DBG(_log, "No action");
				int imageid = instance->getVisual<InstanceVisual>()->getStaticImageIndexByAngle(angle);
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
			}
			else {
				FL_DBG(_log, "Instance does not have image to render");
			}
		}
	
	}
}
