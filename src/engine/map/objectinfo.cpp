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
#include "debugutils.h"

#include "objectinfo.h"

namespace FIFE { namespace map {

	const std::string ObjectInfo::VisualParam      = "visual";
	const std::string ObjectInfo::NameParam        = "name";
	const std::string ObjectInfo::ObjectTypeParam  = "objtype";
	const std::string ObjectInfo::LocationParam    = "location";
	const std::string ObjectInfo::OrientationParam = "orientation";

	ObjectInfo::ObjectInfo() : AttributedClass("map_object"),
	m_location(), m_inventory(), m_temporaryInventory(), params(), m_visualId(0),
	m_isStatic(false) {
		params["objtype"] = "undefined";
		params["name"] = "unnamed";
		params["visual"] = "";

		m_zvalue = 1;
	}

	ObjectInfo::~ObjectInfo() {
		for(size_t i=0; i != m_temporaryInventory.size(); ++i) {
			delete m_temporaryInventory[i];
		}

		/**
			Make a test whether a MapVisual is still set for this
			object. On Deletion this will try to dereference an
			invalid pointer.

			We could use smart_ptr'd MapObjectInfos instead, but
			Then this would probably result in an untraceable leak.
		*/
		if (m_visualId != 0) {
			Warn("map_object")
				<< "Dangling MapVisual pointing to deleted MapObjectInfo";
			debugPrint();
		}
	}

	void ObjectInfo::debugPrint() {
		Log("object")
			<< "name " << params["name"]
			<< " at " << m_location.position
			<< " on elevation " <<  m_location.elevation
			<< "#" << m_location.grid;
	}
} } //FIFE::map
