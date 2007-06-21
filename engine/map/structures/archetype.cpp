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
#include "util/exception.h"
#include "util/purge.h"
#include "map/loaders/archetype_loader.h"

#include "archetype.h"

namespace FIFE { namespace map {

	Archetype::Archetype(const std::string& type, const std::string& filename, MapPtr parent)
		: m_typename(type), m_filename(filename), m_map(parent)
	{
	}

	Archetype* Archetype::load(const std::string& type, const std::string& filename, MapPtr map) {
		typedef std::map<std::string, ArchetypeLoaderBase*> type_atloaders;
		static type_atloaders loaders;

		if(loaders.empty()) {
			// memory leak?
			loaders.insert(std::make_pair("XML",ArchetypeLoaderBase::createLoader("XML")));
		}
/*
    // If it is already loaded, just return.
		std::list<Archetype*>::iterator i = m_archetypes.begin();
		for(; i != m_archetypes.end(); ++i) {
			if( (*i)->getTypeName() == type && (*i)->getFilename() == filename ) {
				return;
			}
		}
*/
		if(loaders.find(type) == loaders.end()) {
			throw NotFound(type + " Archetype Loader not found.");
		}
		Archetype* at = loaders[type]->load(filename, map);

		// 'load' schould not return zero, rather throw a reasonable exception
		assert(at);
		assert(at->getTypeName() == type);

		return at;
	}

	Archetype::~Archetype() {
		purge(m_archetypes);
	}

	void Archetype::addArchetype(Archetype* archetype) {
    m_archetypes.push_back(archetype);
	}

	void Archetype::loadPrototype(ObjectInfo*, size_t ) {
		throw NotSupported(m_typename + "::loadPrototype");
	}

}}
