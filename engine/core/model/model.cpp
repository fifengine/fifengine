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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/purge.h"
#include "model/metamodel/abstractpather.h"
#include "model/metamodel/object.h"
#include "model/metamodel/grids/cellgrid.h"
#include "structures/map.h"
#include "structures/layer.h"
#include "structures/instance.h"
#include "util/base/exception.h"
#include "view/rendererbase.h"
#include "video/renderbackend.h"
#include "video/imagepool.h"
#include "video/animationpool.h"

#include "model.h"

namespace FIFE {

	Model::Model(RenderBackend* renderbackend, const std::vector<RendererBase*>& renderers,
					ImagePool* imagepool, AnimationPool* animpool):
		FifeClass(),
		m_last_namespace(NULL),
		m_timeprovider(NULL),
		m_renderbackend(renderbackend),
		m_imagepool(imagepool),
		m_animpool(animpool),
		m_renderers(renderers){
	}

	Model::~Model() {
		purge(m_maps);
		for(std::list<namespace_t>::iterator nspace = m_namespaces.begin(); nspace != m_namespaces.end(); ++nspace)
			purge_map(nspace->second);
		purge(m_pathers);
		purge(m_created_grids);
		purge(m_adopted_grids);
	}

	Map* Model::createMap(const std::string& identifier) {
		std::list<Map*>::const_iterator it = m_maps.begin();
		for(; it != m_maps.end(); ++it) {
			if(identifier == (*it)->getId()) {
				throw NameClash(identifier);
			}
		}

		Map* map = new Map(identifier, m_renderbackend, m_renderers, m_imagepool, m_animpool, &m_timeprovider);
		m_maps.push_back(map);
		return map;
	}

	void Model::adoptPather(AbstractPather* pather) {
		m_pathers.push_back(pather);
	}

	AbstractPather* Model::getPather(const std::string& pathername) {
		std::vector<AbstractPather*>::const_iterator it = m_pathers.begin();
		for(; it != m_pathers.end(); ++it) {
			if ((*it)->getName() == pathername) {
				return *it;
			}
		}
		return NULL;
	}

	void Model::adoptCellGrid(CellGrid* grid) {
		m_adopted_grids.push_back(grid);
	}

	CellGrid* Model::getCellGrid(const std::string& gridtype) {
		std::vector<CellGrid*>::const_iterator it = m_adopted_grids.begin();
		for(; it != m_adopted_grids.end(); ++it) {
			if ((*it)->getType() == gridtype) {
				CellGrid* newcg = (*it)->clone();
				m_created_grids.push_back(newcg);
				return newcg;
			}
		}
		return NULL;
	}


	Map* Model::getMap(const std::string& identifier) const {
		std::list<Map*>::const_iterator it = m_maps.begin();
		for(; it != m_maps.end(); ++it) {
			if((*it)->getId() == identifier)
				return *it;
		}

		throw NotFound(std::string("Tried to get non-existent map: ") + identifier + ".");
	}

	void Model::deleteMap(Map* map) {
		std::list<Map*>::iterator it = m_maps.begin();
		for(; it != m_maps.end(); ++it) {
			if(*it == map) {
				delete *it;
				m_maps.erase(it);
				return ;
			}
		}
	}

	uint32_t Model::getNumMaps() const {
		return m_maps.size();
	}

	void Model::deleteMaps() {
		purge(m_maps);
		m_maps.clear();
	}

	std::list<std::string> Model::getNamespaces() const {
		std::list<std::string> namespace_list;
		std::list<namespace_t>::const_iterator nspace = m_namespaces.begin();
		for(; nspace != m_namespaces.end(); ++nspace) {
			namespace_list.push_back(nspace->first);
		}
		return namespace_list;
	}

	Object* Model::createObject(const std::string& identifier, const std::string& name_space, Object* parent) {
		// Find or create namespace
		namespace_t* nspace = selectNamespace(name_space);
		if(!nspace) {
			m_namespaces.push_back(namespace_t(name_space,objectmap_t()));
			nspace = selectNamespace(name_space);
		}

		// Check for nameclashes
		objectmap_t::const_iterator it = nspace->second.find(identifier);
		if( it != nspace->second.end() ) {
			throw NameClash(identifier);
		}

		// Finally insert & create
		Object* object = new Object(identifier, name_space, parent);
		nspace->second[identifier] = object;
		return object;
	}

	bool Model::deleteObject(Object* object) {
		// WARNING: This code has obviously not been tested (thoroughly).

		// Check if any instances exist. If yes - bail out.
		std::list<Layer*>::const_iterator jt;
		std::vector<Instance*>::const_iterator kt;
		for(std::list<Map*>::iterator it = m_maps.begin(); it != m_maps.end(); ++it) {
			for(jt = (*it)->getLayers().begin(); jt != (*it)->getLayers().end(); ++jt) {
				for(kt = (*jt)->getInstances().begin(); kt != (*jt)->getInstances().end(); ++kt) {
					Object* o = (*kt)->getObject();
					if(o == object) {
						return false;
					}
				}
			}
		}

		// Check if the namespace exists
		namespace_t* nspace = selectNamespace(object->getNamespace());
		if(!nspace)
			return true;

		// If yes - delete+erase object.
		objectmap_t::iterator it = nspace->second.find(object->getId());
		if( it != nspace->second.end()) {
			delete it->second;
			nspace->second.erase(it);
		}

		return true;
	}

	bool Model::deleteObjects() {
		// If we have layers with instances - bail out.
		std::list<Layer*>::const_iterator jt;
		for(std::list<Map*>::iterator it = m_maps.begin(); it != m_maps.end(); ++it) {
			for(jt = (*it)->getLayers().begin(); jt != (*it)->getLayers().end(); ++jt) {
				if((*jt)->hasInstances())
					return false;
			}
		}

		// Otherwise delete every object in every namespace
		std::list<namespace_t>::iterator nspace = m_namespaces.begin();
		while(nspace != m_namespaces.end()) {
			objectmap_t::iterator it = nspace->second.begin();
			for(; it != nspace->second.end(); ++it) {
				delete it->second;
			}
			nspace = m_namespaces.erase(nspace);
		}
		m_last_namespace = 0;
		return true;
	}

	Object* Model::getObject(const std::string& id, const std::string& name_space) {
		namespace_t* nspace = selectNamespace(name_space);
		if(nspace) {
			objectmap_t::iterator it = nspace->second.find(id);
			if( it !=  nspace->second.end() )
				return it->second;
		}
		return 0;
	}

	std::list<Object*> Model::getObjects(const std::string& name_space) const {
		std::list<Object*> object_list;
		const namespace_t* nspace = selectNamespace(name_space);
		if(nspace) {
			objectmap_t::const_iterator it = nspace->second.begin();
			for(; it != nspace->second.end(); ++it )
				object_list.push_back(it->second);
		}

		return object_list;
	}

	const Model::namespace_t* Model::selectNamespace(const std::string& name_space) const {
		std::list<namespace_t>::const_iterator nspace = m_namespaces.begin();
		for(; nspace != m_namespaces.end(); ++nspace) {
			if( nspace->first == name_space ) {
				return &(*nspace);
			}
		}
		return 0;
	}

	Model::namespace_t* Model::selectNamespace(const std::string& name_space) {
		if( m_last_namespace && m_last_namespace->first == name_space )
			return m_last_namespace;
		std::list<namespace_t>::iterator nspace = m_namespaces.begin();
		for(; nspace != m_namespaces.end(); ++nspace) {
			if( nspace->first == name_space ) {
				m_last_namespace = &(*nspace);
				return m_last_namespace;
			}
		}
		m_last_namespace = 0;
		return 0;
	}

	void Model::update() {
		std::list<Map*>::iterator it = m_maps.begin();
		for(; it != m_maps.end(); ++it) {
			(*it)->update();
		}
		std::vector<AbstractPather*>::iterator jt = m_pathers.begin();
		for(; jt != m_pathers.end(); ++jt) {
			(*jt)->update();
		}
	}

} //FIFE

