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
#include "util/purge.h"
#include "util/xmlutil.h"
#include "map/factory.h"
#include "map/objectinfo.h"

#include "prototype.h"


namespace FIFE { namespace map { namespace loaders { namespace xml {

	Prototype::Prototype(TiXmlElement* element) : m_data("Table") {
		assert( element );

		TiXmlElement* proto_element = element->FirstChildElement("prototype");
		while( proto_element ) {
			const char* proto_name = proto_element->Attribute("name");
			if( !proto_name ) {
				throw InvalidFormat("no 'name' attribute on <prototype>");
			}
			m_prototypes.push_back( Factory::instance()->getPrototypeId(proto_name) );
			proto_element = proto_element->NextSiblingElement("prototype");
		}


		TiXmlElement* zvalue_element = element->FirstChildElement("zvalue");
		if( zvalue_element ) {
			m_zvalue = xmlutil::queryElement<int>(zvalue_element);
		}

		TiXmlElement* dynamic_element = element->FirstChildElement("dynamic");
		if( dynamic_element ) {
			m_isstatic = false;
		}

		TiXmlElement* posi_element = element->FirstChildElement("position");
		if( posi_element ) {
			m_position = xmlutil::queryElement<Point>(posi_element);
		}

		TiXmlElement* visual_element = element->FirstChildElement("visual");
		if( visual_element ) {
			loadVisual( visual_element );
		}

		xmlutil::loadMetadata( element, &m_data );

		TiXmlElement* inventory_element = element->FirstChildElement("inventory");
		if( inventory_element ) {
			inventory_element = inventory_element->FirstChildElement("object");
			while( inventory_element ) {
				m_inventory.push_back(new Prototype(inventory_element));
				inventory_element = inventory_element->NextSiblingElement("object");
			}
		}
	}

	Prototype::~Prototype() {
		purge( m_inventory );
	}

	void Prototype::loadVisual(TiXmlElement* element) {
		assert( element );
		RenderableLocation location;
		int attr_ok;

		TiXmlElement* image_element = element->FirstChildElement("image");
		if( image_element ) {
			location.setType( RenderAble::RT_IMAGE );

			const char* source = image_element->Attribute("source");
			if( source == 0 ) {
				throw InvalidFormat("no 'source' attribute on <image> element");
			}
			location.setFilename( source );

			int x_off, y_off;
			attr_ok = image_element->QueryIntAttribute("x_offset",&x_off);
			if( attr_ok == TIXML_SUCCESS ) {
				location.addExtension(RenderableLocation::X, x_off);
			}

			attr_ok = image_element->QueryIntAttribute("y_offset",&y_off);
			if( attr_ok == TIXML_SUCCESS ) {
				location.addExtension(RenderableLocation::Y, y_off);
			}

			m_rlocation = location;
		}

		TiXmlElement* animation_element = element->FirstChildElement("animation");
		if( animation_element ) {
			location.setType( RenderAble::RT_ANIMATION );

			const char* source = animation_element->Attribute("source");
			if( source == 0 ) {
				throw InvalidFormat("no 'source' attribute on <animation> element");
			}
			location.setFilename( source );

			m_rlocation = location;
		}
	}

	ObjectPtr Prototype::create() {
		ObjectPtr object(ObjectInfo::create());
		merge( object.get() );
		return object;
	}

	void Prototype::merge(ObjectInfo* object) {
		assert( object );

		for( size_t i=0; i!=m_prototypes.size(); ++i ) {
			object->loadPrototype( m_prototypes[i] );
		}

		if( m_zvalue.haveData ) {
			object->setZValue( m_zvalue.data ); 
		}

		if( m_isstatic.haveData ) {
			object->setStatic( m_isstatic.data );
		}

		if( m_position.haveData ) {
			object->getLocation().position = m_position.data;
		}

		if( m_rlocation.haveData ) {
			object->setVisualLocation( m_rlocation.data );
		}

		object->updateAttributes( &m_data, true );

		for( size_t i=0; i!= m_inventory.size(); ++i) {
			object->addToInventory( m_inventory[i]->create() );
		}
	}

}}}}

