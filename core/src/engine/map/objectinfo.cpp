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
#include "factory.h"

namespace FIFE { namespace map {

	const std::string ObjectInfo::NameParam        = "name";
	const std::string ObjectInfo::ObjectTypeParam  = "objtype";
	const std::string ObjectInfo::LocationParam    = "location";
	const std::string ObjectInfo::OrientationParam = "orientation";

	ObjectInfo::ObjectInfo() 
		: AttributedClass("GObject"),
		  m_visualId(0),
		  m_isStatic(true) {

		set<std::string>("objtype","undefined");
		set<std::string>("name","unnamed");

		m_zvalue = 2;
	}

	ObjectInfo::~ObjectInfo() {
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

	void ObjectInfo::loadPrototype(size_t proto_id) {
		Factory::instance()->loadPrototype(this, proto_id);

		// Move the proto_id to the end of m_protoid
		// if it was used before, and otherwise append it.

		size_t proto_pos = m_protoid.size();
		for(size_t i = 0; i != m_protoid.size(); ++i) {
			if( m_protoid[i] == proto_id ) {
				proto_pos = i;
				break;
			}
		}
		if( proto_pos == m_protoid.size() ) {
			m_protoid.push_back(proto_id);
		} else {
			for(size_t i = proto_pos; i != m_protoid.size() - 1; ++i) {
				m_protoid[ i ] = m_protoid[ i + 1 ];
			}
			m_protoid.back() = proto_id;
		}

	}

	void ObjectInfo::loadPrototype(const std::string& proto_name) {
		loadPrototype( Factory::instance()->getPrototypeId(proto_name) );
	}

	const std::vector<size_t>& ObjectInfo::listPrototypes() const {
		return m_protoid;
	}

	void ObjectInfo::debugPrint() {
		Log log("object");

		log	<< "name " << get<std::string>("name")
			<< " at " << m_location.position
			<< " on elevation " <<  m_location.elevation
			<< "#" << m_location.layer
			<< " prototypes=";

		for(size_t i=0; i!= m_protoid.size(); ++i) {
			log <<  Factory::instance()->getPrototypeName( m_protoid[i] );
			if( i != m_protoid.size() - 1) {
				log << ",";
			}
		}
	}
} } //FIFE::map
