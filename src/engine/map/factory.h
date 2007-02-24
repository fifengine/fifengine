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

#ifndef FIFE_MAP_FACTORY_H
#define FIFE_MAP_FACTORY_H

// Standard C++ library includes
#include <map>
#include <list>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "singleton.h"

namespace FIFE { namespace map {

	class Map;
	class Loader;
	class Archetype;
	class ArchetypeLoaderBase;
	class ObjectInfo;

	/** User-Interface to load any kind of supported mapfile.
	 * 
	 * The most important function is probably @c loadMap() which
	 * iterates through all supported @em MapLoader(s) until one
	 * succeeds.
	 *
	 * A @em MapLoader needs to be registered with the factory; 
	 * This is done in the constructor.
	 * 
	 * @see MapLoader
	 * @see Map
	 * 
	 * This class is derived from a @em singleton, you can access
	 * the object by calling the @c instance() member function.
	 * 
	 * @see DynamicSingleton
	 */
	class Factory : public DynamicSingleton<Factory> {
		public:
			/** Constructor.
			 * Called during Engine startup
			 */
			Factory();

			/** Destructor.
			 * Called during Engine shutdown
			 */
			virtual ~Factory();

			/** Attempts to load a mapfile.
			 *
			 * Tries all registered loaders until success.
			 *
			 * @param file Path to file to load.
			 * @return Pointer load @em Map instance (on success) or NULL pointer on failure.
			 * @note Exceptions from the maploaders are caught and @b not propagated.
			 * @bug Throwing an exception when all loaders fail might be a good idea.
			 */
			Map* loadMap(const std::string& path);

			/** Load an archetype (collection of tileid/prototypes) from a file.
			 */
			void loadArchetype(const std::string& type, const std::string& filename);

			/** Remove all loaded archetypes
			 */
			void clearArchetypes();

			/** Add an archetype.
			 */
			void addArchetype(Archetype* archetype);

			/** Map type to an internal id
			 *  
			 */
			size_t getPrototypeId(const std::string& type) const;

			/** Map internal id to type name
			 *  
			 */
			const std::string& getPrototypeName(size_t proto_id) const;

			/** Load a prototype of an object
			 *  Loading a prototype of an object will set the objects
			 *  data to default values, that can be overridden.
			 */
			void loadPrototype(ObjectInfo* object, size_t proto_id);

			/** Get ImageCache ID for tile with id id
			 */
			size_t getTileImageId(size_t tile_id) const;

		private:

			/** Registers a format-specific loader class with the factory.
			 *
			 * \param loader Pointer to a valid instance (@b no NULL pointers, please).
			 * \note Used internally
			 */
			void registerLoader(Loader* loader);

			/** Registers a archetype loader class with the factory.
			 *
			 * \param type Type name of the Archetypem the Loader loads.
			 * \param loader Pointer to a valid instance (@b no NULL pointers, please).
			 * \note Used internally
			 */
			void registerArchetypeLoader(const std::string&  type, ArchetypeLoaderBase* loader);

			/** Add a Tile to the Factories repository
			 *  Use this function during the loading phase of the AT.
			 */
			void addTile(size_t tile_id, size_t image_id);

			/** Add a Prototype to the Factories repository
			 *
			 */
			void addPrototype(Archetype* at, const std::string& proto_name, size_t proto_id);

			/** Removes all registered (archetype)loaders.
			 */
			void cleanup();

			typedef std::map<std::string, Loader*> type_loaders;
			// Registered maploaders.
			type_loaders m_loaders;

			typedef std::map<std::string, ArchetypeLoaderBase*> type_atloaders;
			// Registered AT loaders.
			type_atloaders m_atloaders;

			typedef std::list<Archetype*> type_archetypes;
			type_archetypes m_archetypes;

				
			typedef std::map<std::string,size_t> type_protoname_map;

			struct s_proto {
				type_protoname_map::iterator name_iterator;
				Archetype* archetype;
			};

			typedef std::map<size_t,s_proto> type_protoid_map;

			type_protoname_map m_protoname_map;
			type_protoid_map   m_protoid_map;

			typedef std::map<size_t,size_t> type_tileids;

			type_tileids m_tileids;

			friend class Archetype;
	};

	inline
	size_t Factory::getPrototypeId(const std::string& proto_name) const {
		type_protoname_map::const_iterator i(m_protoname_map.find(proto_name));
		if( i == m_protoname_map.end() ) {
			return 0;
		}
		return i->second;
	}

	inline
	const std::string& Factory::getPrototypeName(size_t proto_id) const {
		static std::string invalid = "";

		type_protoid_map::const_iterator i(m_protoid_map.find(proto_id));
		if( i == m_protoid_map.end() ) {
			return invalid;
		}
		return i->second.name_iterator->first;
	}

	inline
	size_t Factory::getTileImageId(size_t tile_id) const {
		type_tileids::const_iterator i(m_tileids.find(tile_id));
		if( i == m_tileids.end() ) {
			return 0;
		}
		return i->second;
	}

	inline
	void Factory::addTile(size_t tile_id, size_t image_id) {
		m_tileids[ tile_id ] = image_id;
	}

	inline
	void Factory::addPrototype(Archetype* at, const std::string& name, size_t id) {
		s_proto proto = { m_protoname_map.insert(std::make_pair(name, id)).first, at };
		m_protoid_map[ id ] = proto;
	}

} } //FIFE::map

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
