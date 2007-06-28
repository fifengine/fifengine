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
#include <string>

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/purge.h"
#include "loaders/archetype_loader.h"
#include "map/geometries/geometry.h"

#include "elevation.h"
#include "layer.h"
#include "map.h"
#include "archetype.h"

namespace FIFE { namespace map {

	long Map::m_count = 0;

	MapPtr Map::create() {
		MapPtr map(new Map);
		map->m_self = map;
		return map;
	}

	Map::Map() 
		: AttributedClass("map") {
		m_count += 1;
		m_atloaders.insert(std::make_pair("XML",ArchetypeLoaderBase::createLoader("XML")));
	}

	Map::~Map() {
		purge(m_archetypes);
		// Use purge_map for deleting std::map values
		purge_map(m_atloaders);
		clearElevations();
		m_count -= 1;
	}

	long Map::globalCount() {
		return m_count;
	}

	const std::string& Map::getMapName() const {
		return m_mapname;
	}
	void Map::setMapName(const std::string& name) {
		m_mapname = name;
	}

	void Map::loadArchetype(const std::string& type, const std::string& filename) {
		// If it is already loaded, just return.
		std::list<Archetype*>::iterator i = m_archetypes.begin();
		for(; i != m_archetypes.end(); ++i) {
			if( (*i)->getTypeName() == type && (*i)->getFilename() == filename ) {
				return;
			}
		}

		if(m_atloaders.find(type) == m_atloaders.end()) {
			throw NotFound(type + " Archetype Loader not found.");
		}

		Archetype* at = m_atloaders[type]->load(filename, MapPtr(m_self));

		// 'load' schould not return zero, rather throw a reasonable exception
		assert(at);
		assert(at->getTypeName() == type);

		addArchetype(at);
	}

	void Map::addArchetype(Archetype* archetype) {
		m_archetypes.push_back(archetype);
	}

	std::list<Archetype*>& Map::dumpArchetypes() {
		return m_archetypes;
	}

	void Map::loadPrototype(ObjectInfo* object, size_t proto_id) {
		assert( object );
		if( proto_id == 0 || proto_id >= m_protoid_map.size() ) {
			return;
		}

		s_proto& proto = m_protoid_map[proto_id];

		assert(proto.archetype);
		proto.archetype->loadPrototype(object,proto_id);
	}

	size_t Map::addPrototype(Archetype* at, const std::string& name) {
		size_t id = m_protoid_map.size();
		s_proto proto = { m_protoname_map.insert(std::make_pair(name, id)).first, at };
		m_protoid_map.push_back( proto );
		return id;
	}

	size_t Map::getPrototypeId(const std::string& proto_name) const {
		type_protoname_map::const_iterator i(m_protoname_map.find(proto_name));
		if( i == m_protoname_map.end() ) {
			return 0;
		}
		return i->second;
	}

	const std::string& Map::getPrototypeName(size_t proto_id) const {
		static std::string invalid = "";
		if( proto_id > m_protoid_map.size() ) {
			return invalid;
		}

		const s_proto& proto = m_protoid_map[proto_id];
		return proto.name_iterator->first;
	}

	void Map::addTile(size_t tileid, size_t imageid) {
		m_tileids[tileid] = imageid;
	}

	size_t Map::getTileImageId(size_t tile_id) const {
		type_tileids::const_iterator i(m_tileids.find(tile_id));
		if( i == m_tileids.end() ) {
			return 0;
		}
		return i->second;
	}

	size_t Map::getNumElevations() const {
		return m_elevations.size();
	}

	ElevationPtr Map::getElevation(size_t index) const {
		if (index >= getNumElevations()) {
			throw IndexOverflow(std::string("invalid elevationlevel: ")
			                    + boost::lexical_cast<std::string>(index));
		}

		return *(m_elevations.begin() + index);
	}

	void Map::addElevation(ElevationPtr elevation) {
		if( !elevation ) {
			throw NotSupported("can't add invalid elevation");
		}
		if( elevation->getMap() ) {
			throw Duplicate("elevation already in a map");
		}
		m_elevations.push_back(elevation);
		elevation->m_map = m_self;
	}

	void Map::insertElevation(size_t index, ElevationPtr elevation) {
		if( !elevation ) {
			throw NotSupported("can't add invalid elevation");
		}
		if( elevation->getMap() ) {
			throw Duplicate("elevation already in a map");
		}
		if (index >= getNumElevations()) {
			throw IndexOverflow(std::string("invalid elevationlevel: ")
			                    + boost::lexical_cast<std::string>(index));
		}
		m_elevations.insert(m_elevations.begin() + index,elevation);
		elevation->m_map = m_self;
	}

	void Map::removeElevation(size_t index) {
		if (index >= getNumElevations()) {
			throw IndexOverflow(std::string("invalid elevationlevel: ")
			                    + boost::lexical_cast<std::string>(index));
		}
		m_elevations[index]->m_map.reset();
		m_elevations.erase(m_elevations.begin() + index);
	}

	void Map::clearElevations() {
		for(size_t i=0; i!= getNumElevations(); ++i) {
			m_elevations[i].reset();
		}
		m_elevations.clear();
	}

	void Map::setScript(Map::ScriptType st, const ScriptContainer& script) {
		m_scripts[st] = script;
	}

	bool Map::hasScript(Map::ScriptType st) const {
		type_scriptmap::const_iterator it = m_scripts.find(st);
		if (it == m_scripts.end()) {
			return false;
		}
		if (it->second.value == "") {
			return false;
		}
		return true;
	}

	const ScriptContainer& Map::getScript(Map::ScriptType st) {
		return m_scripts[st];
	}

	bool Map::isValidLocation(const Location& location) const {
		if (location.elevation >= m_elevations.size())
			return false;

		if (location.layer >= m_elevations[location.elevation]->getNumLayers())
			return false;

		return m_elevations[location.elevation]->
			     getLayer(location.layer)->isValidPosition(location.position);
	}

} } //FIFE::map
