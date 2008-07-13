/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
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
#include "video/imagepool.h"
#include "video/animationpool.h"

#include "util/log/logger.h"
#include "model/structures/location.h"

#include "camera.h"
#include "view.h"

namespace FIFE {
	static Logger _log(LM_VIEWVIEW);
	
	View::View(RenderBackend* renderbackend, ImagePool* ipool, AnimationPool* apool):
		m_cameras(),
		m_renderbackend(renderbackend),
		m_ipool(ipool),
		m_apool(apool),
		m_renderers() {
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


	Camera* View::addCamera(const std::string& id, Layer *layer, Rect viewport, ExactModelCoordinate emc) {
		if (getCamera(id)) {
			throw NameClash("Duplicate camera id registered.");
		}
		Camera* cam = new Camera(id, layer, viewport, emc, m_renderbackend, m_ipool, m_apool);
		m_cameras.push_back(cam);
		
		std::map<std::string, RendererBase*>::iterator r_it = m_renderers.begin();
		for(; r_it != m_renderers.end(); ++r_it) {
			cam->addRenderer(r_it->second->clone());
		}
		return cam;
	}

	Camera* View::getCamera(const std::string& id) {
		std::vector<Camera*>::iterator it = m_cameras.begin();
		for (; it != m_cameras.end(); ++it) {
			if ((*it)->getId() == id)
				return *it;
		}
		return NULL;
	}

	void View::removeCamera(Camera* camera) {
		std::vector<Camera*>::iterator it = std::find(m_cameras.begin(), m_cameras.end(), camera);
		if (it != m_cameras.end()) {
			m_cameras.erase(it);
		}
	}

	void View::resetRenderers() {
		std::vector<Camera*>::iterator it = m_cameras.begin();
		while (it != m_cameras.end()) {
			(*it)->resetRenderers();
			++it;
		}
	}
	
	void View::update() {
		FL_DBG(_log, "In View::update");

		// update each camera
		std::vector<Camera*>::iterator cam_it = m_cameras.begin();
		for (; cam_it != m_cameras.end(); ++cam_it) {
			Camera* cam = *cam_it;
			if (!cam->isEnabled()) {
				continue;
			}
			cam->update();
			cam->render();
		}
	}
	
	void View::addRenderer(RendererBase* renderer) {
		m_renderers[renderer->getName()] = renderer;
	}
}

