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
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "exception.h"
#include "util/xmlutil.h"

#include "object_loader.h"


namespace FIFE { namespace map { namespace loaders { namespace xml {

	ObjectLoader::ObjectLoader(TiXmlElement* element) : m_data("Table") {
		assert( element );

		TiXmlElement* zvalue_element = element->FirstChildElement("zvalue");
		if( zvalue_element ) {
			m_zvalue = xmlutil::queryElement<int>(zvalue_element);
		}

		TiXmlElement* is_static_element = element->FirstChildElement("is_static");
		if( is_static_element ) {
			m_isstatic = xmlutil::queryElement<bool>(is_static_element);
		}

		TiXmlElement* posi_element = element->FirstChildElement("position");
		if( posi_element ) {
			m_position = xmlutil::queryElement<Point>(posi_element);
		}

		xmlutil::loadMetadata( element, &m_data );
	}

	ObjectInfo* ObjectLoader::create() {
		ObjectInfo* object = new ObjectInfo();
		merge( object );
		return object;
	}

	void ObjectLoader::merge(ObjectInfo* object) {
		assert( object );

		if( m_zvalue.haveData ) {
			object->setZValue( m_zvalue.data ); 
		}

		if( m_isstatic.haveData ) {
			object->setStatic( m_isstatic.data );
		}

		if( m_position.haveData ) {
			object->getLocation().position = m_position.data;
		}

		object->updateAttributes( object, true );
	}

}}}}

