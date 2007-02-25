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
#ifdef HAVE_XMLMAP
#include "loaders/xml/xml.h"
#include "loaders/xml/xml_archetype.h"
#endif
#include "loaders/fallout/fallout.h"
#include "debugutils.h"
#include "exception.h"

#include "archetype.h"
#include "archetype_loader.h"
#include "factory.h"
#include "geometry.h"
#include "loader.h"

namespace FIFE { namespace map {

	Factory::Factory() : m_loaders() {
		registerLoader(new loaders::fallout::Fallout());
#ifdef HAVE_XMLMAP
		registerLoader(new loaders::xml::XML());
		registerArchetypeLoader("XML",new ArchetypeLoader<loaders::xml::XMLArchetype>());
#endif
		Geometry::registerGeometry(s_geometry_info(
			Geometry::FalloutTileGeometry,
			"RECTANGULAR",
			Point(80,36),  // TILE SIZE
			Point(48,24),  // TRANSFORM
			Point(),       // OFFSET
			0));           // FLAGS: NONE
		Geometry::registerGeometry(s_geometry_info(
			Geometry::FalloutObjectGeometry,
			"HEXAGONAL",
			Point(32,16),  // TILESIZE
			Point(16,12),  // TRANSFORM
			Point(32,10),  // OFFSET
			Geometry::ShiftXAxis));           // FLAGS: SHIFT AROUND X AXIS
	}

	Factory::~Factory() {
		clearArchetypes();
		cleanup();
	}

	void Factory::cleanup() {
		// FIXME: use boost::ptr_map ?
		type_loaders::const_iterator end = m_loaders.end();
		for (type_loaders::iterator i = m_loaders.begin(); i != end; ++i) {
			delete i->second;
		}
		m_loaders.clear();

		type_atloaders::const_iterator atend = m_atloaders.end();
		for (type_atloaders::iterator i = m_atloaders.begin(); i != atend; ++i) {
			delete i->second;
		}
		m_atloaders.clear();
	}

	void Factory::clearArchetypes() {
		type_archetypes::iterator i = m_archetypes.begin();
		for(; i != m_archetypes.end(); ++i) {
			delete *i;
		}

		m_protoname_map.clear();
		m_protoid_map.clear();
		m_tileids.clear();
	}

	void Factory::registerLoader(Loader* loader) {
		assert( loader );
		m_loaders.insert(std::make_pair(loader->getName(), loader));
		Log("map::factory") << "new maploader: " << loader->getName();
	}

	Map* Factory::loadMap(const std::string& path) {
		type_loaders::const_iterator end = m_loaders.end();
		for (type_loaders::iterator i = m_loaders.begin(); i != end; ++i) {
			try {
				Loader* loader = i->second;
				Debug("maploader") << "trying to load " << path;
				return loader->loadFile(path);
			} catch (const Exception& ex) {
				Log("maploader") << ex.getMessage();
				continue;
			}
		}

		Log("map::factory") << "no loader succeeded for " << path << " :(";
		return 0;
	}

	void Factory::registerArchetypeLoader(const std::string& type, ArchetypeLoaderBase* loader) {
		assert( loader );
		if( m_atloaders.find(type) != m_atloaders.end() ) {
			throw NameClash(type + " Archetype Loader already registered.");
		}
		m_atloaders[ type ] = loader;
	}

	void Factory::loadArchetype(const std::string& type, const std::string& filename) {
		// If it is already loaded, just return.
		std::list<Archetype*>::iterator i = m_archetypes.begin();
		for(; i != m_archetypes.end(); ++i) {
			if( (*i)->getTypeName() == type && (*i)->getFilename() == filename ) {
				return;
			}
		}

		if( m_atloaders.find(type) == m_atloaders.end() ) {
			throw NotFound(type + " Archetype Loader not found.");
		}
		Archetype* at = m_atloaders[type]->load( filename );

		// 'load' schould not return zero, rather throw a reasonable exception
		assert( at );
		assert( at->getTypeName() == type );

		addArchetype( at );
	}

	void Factory::addArchetype(Archetype* archetype) {
		m_archetypes.push_front( archetype );
	}

	void Factory::loadPrototype(ObjectInfo* object, size_t proto_id) {
		assert( object );
		if( proto_id >= m_protoid_map.size() ) {
			return;
		}

		s_proto& proto = m_protoid_map[proto_id];

		assert( proto.archetype );
		proto.archetype->loadPrototype(object,proto_id);
	}

} } //FIFE::map
