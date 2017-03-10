/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

#include "map.h"
#include "layer.h"
#include "cellcache.h"
#include "instance.h"
#include "triggercontroller.h"

namespace FIFE {

	Map::Map(const std::string& identifier, RenderBackend* renderBackend,
			const std::vector<RendererBase*>& renderers, TimeProvider* tp_master):
		m_id(identifier),
		m_filename(""),
		m_timeProvider(tp_master),
		m_changeListeners(),
		m_changedLayers(),
		m_renderBackend(renderBackend),
		m_renderers(renderers),
		m_changed(false) {

		m_triggerController = new TriggerController(this);
	}

	Map::~Map() {
		delete m_triggerController;
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
		return NULL;
	}

	uint32_t Map::getLayerCount() const {
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
		std::vector<MapChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
			(*i)->onLayerCreate(this, layer);
			++i;
		}

		return layer;
	}

	void Map::deleteLayer(Layer* layer) {
		std::list<Layer*>::iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if((*it) == layer) {
				std::vector<MapChangeListener*>::iterator i = m_changeListeners.begin();
				while (i != m_changeListeners.end()) {
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
		std::list<Layer*> temp_layers = m_layers;
		std::list<Layer*>::iterator temp_it = temp_layers.begin();
		for(; temp_it != temp_layers.end(); ++temp_it) {
			std::vector<MapChangeListener*>::iterator i = m_changeListeners.begin();
			while (i != m_changeListeners.end()) {
				(*i)->onLayerDelete(this, *temp_it);
				++i;
			}
			std::list<Layer*>::iterator it = m_layers.begin();
			for(; it != m_layers.end(); ++it) {
				if(*it == *temp_it) {
					delete *it;
					m_layers.erase(it);
					break;
				}
			}
		}
	}

	void Map::getMinMaxCoordinates(ExactModelCoordinate& min, ExactModelCoordinate& max) {
		if (m_layers.empty()) {
			return;
		}
		std::list<Layer*>::iterator it = m_layers.begin();
		Layer* layer = *it;
		for (; it != m_layers.end(); ++it) {
			ModelCoordinate newMin, newMax;
			(*it)->getMinMaxCoordinates(newMin, newMax, layer);

			if (newMin.x < min.x) {
				min.x = newMin.x;
			}
			if (newMax.x > max.x) {
				max.x = newMax.x;
			}
			if (newMin.y < min.y) {
				min.y = newMin.y;
			}
			if (newMax.y > max.y) {
				max.y = newMax.y;
			}
		}
		Location lmin(layer);
		Location lmax(layer);
		lmin.setExactLayerCoordinates(min);
		lmax.setExactLayerCoordinates(max);

		min = lmin.getMapCoordinates();
		max = lmax.getMapCoordinates();
	}

	bool Map::update() {
		m_changedLayers.clear();
		// transfer instances from one layer to another
		if (!m_transferInstances.empty()) {
			std::map<Instance*, Location>::iterator it = m_transferInstances.begin();
			for (; it != m_transferInstances.end(); ++it) {
				Instance* inst = (*it).first;
				Location target_loc = (*it).second;
				Layer* source = inst->getOldLocationRef().getLayer();
				Layer* target = target_loc.getLayer();
				if (source != target) {
					source->removeInstance(inst);
					target->addInstance(inst, target_loc.getExactLayerCoordinates());
				}
			}
			m_transferInstances.clear();
		}
		std::vector<CellCache*> cellCaches;
		std::list<Layer*>::iterator it = m_layers.begin();
		// update Layers
		for(; it != m_layers.end(); ++it) {
			if ((*it)->update()) {
				m_changedLayers.push_back(*it);
			}
			CellCache* cache = (*it)->getCellCache();
			if (cache) {
				cellCaches.push_back(cache);
			}
		}
		// loop over Caches and update
		for (std::vector<CellCache*>::iterator cacheIt = cellCaches.begin();
			cacheIt != cellCaches.end(); ++cacheIt) {
			(*cacheIt)->update();
		}
		if (!m_changedLayers.empty()) {
			std::vector<MapChangeListener*>::iterator i = m_changeListeners.begin();
			while (i != m_changeListeners.end()) {
				(*i)->onMapChanged(this, m_changedLayers);
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
		m_changeListeners.push_back(listener);
	}

	void Map::removeChangeListener(MapChangeListener* listener) {
		std::vector<MapChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
			if ((*i) == listener) {
				m_changeListeners.erase(i);
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
		Camera* camera = new Camera(id, layer, viewport, m_renderBackend);
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

	const std::vector<Camera*>& Map::getCameras() const {
		return m_cameras;
	}

	uint32_t Map::getActiveCameraCount() const {
		uint32_t count = 0;
		std::vector<Camera*>::const_iterator it = m_cameras.begin();
		for ( ; it != m_cameras.end(); ++it) {
			if ((*it)->isEnabled()) {
				count += 1;
			}
		}
		return count;
	}

	void Map::addInstanceForTransfer(Instance* instance, const Location& target) {
		std::pair<std::map<Instance*, Location>::iterator, bool> insertiter = m_transferInstances.insert(std::make_pair(instance, target));
		if (insertiter.second == false) {
			Location& loc = insertiter.first->second;
			loc.setLayer(target.getLayer());
			loc.setExactLayerCoordinates(target.getExactLayerCoordinates());
		}
	}

	void Map::removeInstanceForTransfer(Instance* instance) {
		std::map<Instance*, Location>::iterator it = m_transferInstances.find(instance);
		if (it != m_transferInstances.end()) {
			m_transferInstances.erase(it);
		}
	}

	void Map::initializeCellCaches() {
		if (m_layers.empty()) {
			return;
		}

		std::list<Layer*>::iterator layit = m_layers.begin();
		// first add interacts to walkables
		for (; layit != m_layers.end(); ++layit) {
			if ((*layit)->isInteract()) {
				Layer* temp = getLayer((*layit)->getWalkableId());
				if (temp) {
					temp->addInteractLayer(*layit);
				}
			}
		}
		// then create CellCaches for walkables
		layit = m_layers.begin();
		for (; layit != m_layers.end(); ++layit) {
			if ((*layit)->isWalkable()) {
				(*layit)->createCellCache();
			}
		}
	}

	void Map::finalizeCellCaches() {
		// create Cells and generate neighbours
		std::list<Layer*>::iterator layit = m_layers.begin();
		for (; layit != m_layers.end(); ++layit) {
			CellCache* cache = (*layit)->getCellCache();
			if (cache) {
				cache->createCells();
				cache->forceUpdate();
			}
		}
	}
} //FIFE

