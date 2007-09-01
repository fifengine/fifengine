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
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/action.h"
#include "model/structures/elevation.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view.h"
#include "camera.h"

namespace FIFE {
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
	
	void View::updateCamera(Camera* camera) {
		std::cout << "In View::updateCamera\n";
		const Location& loc = camera->getLocation();
		Elevation* elev = loc.elevation;
		if (!elev) {
			std::cout << "No elevation found, exiting\n";
			return;
		}
		std::cout << "Getting layers...\n";
		const std::vector<Layer*>& layers = elev->getLayers();
		std::vector<Layer*>::const_iterator layer_it = layers.begin();
		while (layer_it != layers.end()) {
			std::cout << "Iterating layer...\n";
			Layer* layer = (*layer_it);
			CellGrid* cg = layer->getCellGrid();
			if (!cg) {
				std::cout << "No cellgrid assigned to layer...\n";
				++layer_it;
			}
			std::cout << "Getting instance...\n";
			const std::vector<Instance*>& instances = layer->getInstances();
			std::vector<Instance*>::const_iterator instance_it = instances.begin();
			while (instance_it != instances.end()) {
				std::cout << "Iterating instances...\n";
				Instance* instance = (*instance_it);
				Image* image = NULL;
				DoublePoint layerpos = intPt2doublePt(instance->getPosition());
				DoublePoint elevpos = cg->toElevationCoords(layerpos);
				std::cout << "Instance layer position = " << instance->getPosition() << "\n";
				std::cout << "Instance elevation position = " << elevpos << "\n";
				Action* action = instance->getCurrentAction();
				if (action) {
					std::cout << "Instance has action\n";
					DoublePoint elevface = cg->toElevationCoords(intPt2doublePt(instance->getFacingCell()));
					float dx = static_cast<float>(elevface.x - elevpos.x);
					float dy = static_cast<float>(elevface.y - elevpos.y);
					int angle = static_cast<int>(atan2f(dx,dy)*180.0/M_PI);
					std::cout << "Got angle " << angle << " for animation\n";
					int animation_id = action->getAnimationIndexByAngle(angle);
					Animation& animation = m_animationpool->getAnimation(animation_id);
					image = animation.getFrameByTimestamp(instance->getActionRuntime());
				} else {
					int imageid = instance->getStaticImageId();
					std::cout << "Instance does not have action, using static image with id " << imageid << "\n";
					image = &m_imagepool->getImage(imageid);
				}
				if (image) {
					std::cout << "Instance has image to render\n";
					double offset_dist = instance->getOffsetDistance();
					std::cout << "Instance offset distance = " << offset_dist << "\n";
					DoublePoint exact_pos = layerpos;
					if (offset_dist > 0) {
						exact_pos = cg->getOffset(instance->getPosition(), 
						                          instance->getOffsetTarget(), 
						                          offset_dist);
					}
					std::cout << "Instance exact position in layer = " << exact_pos << "\n";
					DoublePoint exact_elevpos = cg->toElevationCoords(exact_pos);
					std::cout << "Instance exact position in elevation  = " << exact_elevpos << "\n";
					Point drawpt = camera->toScreenCoords(exact_elevpos);
					std::cout << "Instance exact position in screen = " << drawpt << "\n";
					int w = image->getWidth();
					int h = image->getHeight();
					drawpt.x -= w / 2;
					drawpt.x += image->getXShift();
					drawpt.y -= h / 2;
					drawpt.y += image->getYShift();
					Rect r = Rect(drawpt.x, drawpt.y, w, h);
					std::cout << "image(" << r << "), viewport (" << camera->getViewPort() << ")\n";
					if (r.intersects(camera->getViewPort())) {
						std::cout << "Instance is visible in viewport, rendering\n";
						image->render(r);
					} else {
						std::cout << "Instance is not visible in viewport, skipping\n";
					}
				}
				else {
					std::cout << "Instance does not have image to render\n";
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

