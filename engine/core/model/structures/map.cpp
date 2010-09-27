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
#include <string>

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/structures/purge.h"
#include "util/structures/rect.h"
#include "view/camera.h"
#include "view/rendererbase.h"
#include "video/renderbackend.h"
#include "video/imagepool.h"
#include "video/animationpool.h"

#include "map.h"
#include "layer.h"

namespace FIFE {

	Map::Map(const std::string& identifier, RenderBackend* renderBackend,
			const std::vector<RendererBase*>& renderers, ImagePool* imagePool,
			AnimationPool* animPool, TimeProvider* tp_master):
		m_id(identifier),
		m_timeprovider(tp_master),
		m_changelisteners(),
		m_changedlayers(),
		m_renderbackend(renderBackend),
		m_imagepool(imagePool),
		m_animpool(animPool),
		m_renderers(renderers),
		m_changed(false){
	}

	Map::~Map() {
        // remove all cameras
        std::vector<Camera*>::iterator iter = m_cameras.begin();
        for ( ; iter != m_cameras.end(); ++iter) {
            delete *iter;
        }
        m_cameras.clear();

		deleteLayers();
	}

	Layer* Map::getLayer(const std::string& id) {
		std::list<Layer*>::const_iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if((*it)->getId() == id)
				return *it;
		}

		throw NotFound(id);
	}

	uint32_t Map::getNumLayers() const {
		return m_layers.size();
	}

	Layer* Map::createLayer(const std::string& identifier, CellGrid* grid) {
		std::list<Layer*>::const_iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if(identifier == (*it)->getId())
				throw NameClash(identifier);
		}

		Layer* layer = new Layer(identifier, this, grid);
		m_layers.push_back(layer);
		m_changed = true;
		std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			(*i)->onLayerCreate(this, layer);
			++i;
		}

		return layer;
	}

	void Map::deleteLayer(Layer* layer) {
		std::list<Layer*>::iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if((*it) == layer) {
				std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
				while (i != m_changelisteners.end()) {
					(*i)->onLayerDelete(this, layer);
					++i;
				}
				delete layer;
				m_layers.erase(it);
				return ;
			}
		}
		m_changed = true;
	}

	void Map::deleteLayers() {
		std::list<Layer*>::iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
			while (i != m_changelisteners.end()) {
				(*i)->onLayerDelete(this, *it);
				++i;
			}
		}
		purge(m_layers);
		m_layers.clear();
	}

	bool Map::update() {
		m_changedlayers.clear();
		std::list<Layer*>::iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if ((*it)->update()) {
				m_changedlayers.push_back(*it);
			}
		}
		if (!m_changedlayers.empty()) {
			std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
			while (i != m_changelisteners.end()) {
				(*i)->onMapChanged(this, m_changedlayers);
				++i;
			}
		}

		// loop over cameras and update if enabled
		std::vector<Camera*>::iterator camIter = m_cameras.begin();
		for ( ; camIter != m_cameras.end(); ++camIter) {
			if ((*camIter)->isEnabled()) {
				(*camIter)->update();
				(*camIter)->render();
			}
		}

		bool retval = m_changed;
		m_changed = false;
		return retval;
	}

	void Map::addChangeListener(MapChangeListener* listener) {
		m_changelisteners.push_back(listener);
	}

	void Map::removeChangeListener(MapChangeListener* listener) {
		std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			if ((*i) == listener) {
				m_changelisteners.erase(i);
				return;
			}
			++i;
		}
	}

	Camera* Map::addCamera(const std::string &id, Layer *layer, const Rect& viewport) {
		if (layer == NULL) {
			throw NotSupported("Must have valid layer for camera");
		}

		if (getCamera(id)) {
			std::string errorStr = "Camera: " + id + " already exists";
			throw NameClash(errorStr);
		}

		// create new camera and add to list of cameras
		Camera* camera = new Camera(id, layer, viewport, m_renderbackend, m_imagepool, m_animpool);
		m_cameras.push_back(camera);

		std::vector<RendererBase*>::iterator iter = m_renderers.begin();
		for ( ; iter != m_renderers.end(); ++iter) {
			camera->addRenderer((*iter)->clone());
		}

		return camera;
	}

	void Map::removeCamera(const std::string &id) {
		std::vector<Camera*>::iterator iter = m_cameras.begin();
		for ( ; iter != m_cameras.end(); ++iter) {
			if ((*iter)->getId() == id) {
				// camera has been found delete it
				delete *iter;

				// now remove it from the vector
				// note this invalidates iterators, but we do not need
				// to worry about it in this case since we are done
				m_cameras.erase(iter);

				break;
			}
		}
	}

	Camera* Map::getCamera(const std::string &id) {
		std::vector<Camera*>::iterator iter = m_cameras.begin();
		for ( ; iter != m_cameras.end(); ++iter) {
			if ((*iter)->getId() == id) {
				return *iter;
			}
		}

		return NULL;
	}

	std::vector<Camera*>& Map::getCameras() {
		return m_cameras;
	}

} //FIFE

