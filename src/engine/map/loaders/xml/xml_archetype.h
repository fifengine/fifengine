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

#ifndef FIFE_MAP_LOADERS_XML_ARCHETYPE_H
#define FIFE_MAP_LOADERS_XML_ARCHETYPE_H

// Standard C++ library includes
#include <string>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/archetype.h"
#include "tinyxml/tinyxml.h"

namespace FIFE { namespace map { namespace loaders { namespace xml {

	class ObjectLoader;

	/** XML Archetype
	 * @see http://wiki.fifengine.de/index.php?title=Archetypes
	 */
	class XMLArchetype : public Archetype {
		public:
			/** Constructor
			 *  Loads an archetype from the given filename
			 */
			XMLArchetype(const std::string& filename);

			/** Constructor
			 *  Loads an embedded archetype
			 */
			XMLArchetype(TiXmlElement* e);

			virtual void loadPrototype(ObjectInfo* object, size_t proto_id);
		
		protected:
			void load(TiXmlElement* e);
			void loadArchetypes(TiXmlElement* e);
			void loadPrototypes(TiXmlElement* e);

			void loadTileset(TiXmlElement* e);
			void loadImage(TiXmlElement* e, int firstgid, int& gid, int lastgid,
			               int tw, int th);

			typedef std::map<size_t,ObjectLoader*> type_prototypes;
			type_prototypes m_prototypes;
	};

}}}}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
