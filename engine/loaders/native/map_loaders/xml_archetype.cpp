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
#include "util/exception.h"
#include "video/imagecache.h"
#include "util/log.h"
#include "util/purge.h"
#include "loaders/renderable_location.h"
#include "video/pixelbuffer.h"

#include "xml_archetype.h"
#include "prototype.h"

namespace FIFE { namespace map { namespace loaders { namespace xml {

	XMLArchetype::XMLArchetype(const std::string& filename, MapWeakPtr parent)
		: Archetype("XML",filename, parent) {

		Log("xml::archetype")
			<< "Loading " << filename;

		TiXmlDocument doc(filename);

		if (!doc.LoadFile()) {
			throw InvalidFormat(std::string("Error loading AT file '") 
				+ filename + "' "
				+ doc.ErrorDesc());
		}

		TiXmlNode* node = doc.RootElement();
		TiXmlElement* el;
		if (node) {
			el = node->ToElement();
		} else {
			throw InvalidFormat("Error: could not switch to xml:element?");
		}
		load(el);
	}

	XMLArchetype::XMLArchetype(TiXmlElement* e, MapWeakPtr parent) : Archetype("XML", "embedded", parent) {
		load(e);
	}

	XMLArchetype::~XMLArchetype() {
		purge_map<type_prototypes>(m_prototypes);
	}

	void XMLArchetype::load(TiXmlElement* e) {
		assert( e );

		TiXmlElement* at_element = e->FirstChildElement("archetypes");
		if( at_element ) {
			loadArchetypes( at_element );
		}

		TiXmlElement* tileset_element = e->FirstChildElement("tileset");
		while( tileset_element ) {
			loadTileset( tileset_element );
			tileset_element = tileset_element->NextSiblingElement("tileset");
		}

		TiXmlElement* proto_element = e->FirstChildElement("prototypes");
		if( proto_element ) {
			loadPrototypes( proto_element );
		}
	}

	void XMLArchetype::loadArchetypes(TiXmlElement* e) {
		assert(e);
		e = e->FirstChildElement("archetype");
		while (e) {
			const char* type = e->Attribute("type");
			const char* source = e->Attribute("source");

			if (!type) {
				throw InvalidFormat("no type attribute on <archetype>");
			}

			if (!source) {
				Archetype* xmlat = new XMLArchetype(e, m_map);	
				addArchetype(xmlat);
			} else {
				MapPtr map = m_map.lock();
				map->loadArchetype(type, source);
			}

			e = e->NextSiblingElement("archetype");
		}
	}

	void XMLArchetype::loadTileset(TiXmlElement* element) {
		int tilewidth, tileheight, gid, firstgid, lastgid;
		int attr_ok;

		const char* tilesetname = element->Attribute("name");
		if (!tilesetname) {
			throw InvalidFormat("no 'name' attribute on <tileset>");
		}

		attr_ok = element->QueryIntAttribute("tilewidth", &tilewidth);
		if( attr_ok == TIXML_WRONG_TYPE ) {
			throw InvalidFormat("'tilewidth' attribute on <tileset> is not an integer");
		}
		if( attr_ok == TIXML_NO_ATTRIBUTE ) {
			throw InvalidFormat("no 'tilewidth' attribute on <tileset>");
		}

		attr_ok = element->QueryIntAttribute("tileheight", &tileheight);
		if( attr_ok == TIXML_WRONG_TYPE ) {
			throw InvalidFormat("'tileheight' attribute on <tileset> is not an integer");
		}
		if( attr_ok == TIXML_NO_ATTRIBUTE ) {
			throw InvalidFormat("no 'tileheight' attribute on <tileset>");
		}

		attr_ok = element->QueryIntAttribute("firstgid", &firstgid);
		if( attr_ok == TIXML_SUCCESS ) {
			gid = firstgid;
			if( firstgid <= 0 ) {
				throw InvalidFormat("'firstgid' attribute on <tileset> is <= 0");
			}

			attr_ok = element->QueryIntAttribute("lastgid", &lastgid);
			if( attr_ok != TIXML_SUCCESS ) {
				lastgid = 1000000; //FIXME
			} else {
				if( lastgid <= firstgid ) {
					throw InvalidFormat("'lastgid' attribute on <tileset> is <= firstgid");
				}
			}

		} else {
			gid = firstgid = 0;
			lastgid = 1000000; //FIXME
		}

		if( tilewidth <= 0 ) {
			throw InvalidFormat("'tilewidth' attribute on <tileset> is <= 0");
		}
		if( tileheight <= 0 ) {
			throw InvalidFormat("'tileheight' attribute on <tileset> is <= 0");
		}

		TiXmlElement* image_element = element->FirstChildElement("image");
		while( image_element ) {
			loadImage( image_element, firstgid, gid, lastgid, tilewidth, tileheight );
			image_element = image_element->NextSiblingElement("image");
		}
	}

#define CHECK_OR_THROW(attr)                                                         \
attr_ok = element->QueryIntAttribute(#attr,&attr);                                   \
if(attr_ok == TIXML_WRONG_TYPE ) {                                                   \
	throw InvalidFormat("'" #attr "' attribute on <image> is not an integer");     \
}                                                                                    \
if(attr_ok == TIXML_SUCCESS && attr < 0 ) {                                          \
	throw InvalidFormat("'" #attr "' attribute on <image> is < 0");                \
}                                                                                    \
if(attr_ok == TIXML_NO_ATTRIBUTE) {                                                  \
	attr = 0;                                                                      \
}

	void XMLArchetype::loadImage(TiXmlElement* element, int firstgid, int& gid, int lastgid,
	                             int tw, int th) {
		assert( element );

		int singletile;
		int x_offset, y_offset, rows, cols, row_padding, col_padding;
		int attr_ok;
		
		const char* source = element->Attribute("source");
		if( source == 0 ) {
			throw InvalidFormat("no 'source' attribute on <image>");
		}

		attr_ok = element->QueryIntAttribute("firstgid",&gid);
		if( attr_ok == TIXML_WRONG_TYPE ) {
			throw InvalidFormat("'firstgid' attribute on <image> is not an integer");
		}
		// SUCCESS and NO_ATTR are both ok.
		if( gid < firstgid || gid > lastgid ) {
			throw InvalidFormat("'firstgid' attribute on <image> is out of id range");
		}

		// CHECK SINGLE TILE CASE
		attr_ok = element->QueryIntAttribute("singletile",&singletile);
		if( attr_ok == TIXML_WRONG_TYPE ) {
			throw InvalidFormat("'singletile' attribute on <image> is not an integer");
		}
		if( attr_ok == TIXML_SUCCESS && singletile != 0 ) {
			size_t image_id = ImageCache::instance()->addImageFromFile( source );
			addTile( gid, image_id );
			gid += 1;
			return;
		}
		// SINGLE TILE CASE DONE HERE
		CHECK_OR_THROW(x_offset);
		CHECK_OR_THROW(y_offset);
		CHECK_OR_THROW(row_padding);
		CHECK_OR_THROW(col_padding);
		CHECK_OR_THROW(rows);
		CHECK_OR_THROW(cols);

		size_t base_image = ImageCache::instance()->addImageFromFile(source);
		if( rows == 0 || cols == 0 ) {
			PixelBufferPtr pbuffer = ImageCache::instance()->getPixelBuffer(base_image);
			if( !pbuffer ) {
				throw InvalidFormat("'source' attribute on <image>"
				                    " does not specifiy a valid image");
			}
			rows = (pbuffer->getSurface()->w - x_offset)/(tw + row_padding);
			cols = (pbuffer->getSurface()->h - y_offset)/(th + col_padding);
		}

		for(int y=0; y < rows; ++y) {
			for(int x=0; x < cols; ++x) {
				RenderableLocation location(RenderAble::RT_SUBIMAGE);
				location.addExtension(RenderableLocation::IMAGE_ID, base_image);
				location.addExtension(RenderableLocation::X, x*(tw+row_padding) + x_offset);
				location.addExtension(RenderableLocation::Y, y*(th+col_padding) + y_offset);
				location.addExtension(RenderableLocation::W, tw);
				location.addExtension(RenderableLocation::H, th);
	
				size_t image_id = ImageCache::instance()->addImageFromLocation(location);
				addTile( gid, image_id );
				Debug("xml::archetype")
					<< "TILE:" << source << "(" << base_image << ") crop= ["
					<< x*(tw+row_padding) + x_offset << "x"
					<< y*(th+col_padding) + y_offset << " - "
					<< tw << "x" << th
					<< "] => " << gid << "(" << image_id << ")";
				gid += 1;
			}
		}
	}
#undef CHECK_OR_THROW

	void XMLArchetype::loadPrototype(ObjectInfo* object, size_t proto_id) {
		type_prototypes::iterator i = m_prototypes.find( proto_id );
		if( i == m_prototypes.end() ) {
			throw NotFound(std::string("no proto_id '")
			               + boost::lexical_cast<std::string>(proto_id)
			               + "' in XML Archetype " + getFilename());
		}
		i->second->merge( object );
	}

	void XMLArchetype::loadPrototypes(TiXmlElement* element) {
		assert( element );
		element = element->FirstChildElement("prototype");
		while( element ) {
			const char* name = element->Attribute("name");
			if( name == 0 ) {
				throw InvalidFormat("no 'name' attribute on <prototype>");
			}

			Prototype* objLoader = new Prototype(element, m_map);
			size_t proto_id = addPrototype(name);
			m_prototypes.insert( std::make_pair(proto_id,objLoader) );

			Log("xml::archetype")
				<< " loaded Prottype '" << name << "' as " << proto_id;

			element = element->NextSiblingElement("prototype");
		}
	}

}}}}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
