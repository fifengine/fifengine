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
#include <memory>

// 3rd party library includes
#include <SDL_image.h>
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "map/map.h"
#include "map/elevation.h"
#include "map/factory.h"
#include "map/geometry.h"
#include "map/layer.h"
#include "map/objectinfo.h"
#include "xml/tinyxml/tinyxml.h"
#include "xml/xmlutil.h"
#include "util/debugutils.h"
#include "util/exception.h"
#include "video/imagecache.h"

#include "xml.h"
#include "xml_archetype.h"
#include "prototype.h"

// I did some minor fixes in here (beyond just reformatting) but it's nowhere
// near exhaustive; my understanding is that we're replacing the XML file
// format anyway.
//
// TinyXML is actually kind of hairy by modern C++ standards - all kinds of
// old school stdio in the implementation and "custom linked list" at the
// interface (instead of iterators). In any event, one thing we should do
// (I didn't try to make all the changes) is use the return value from
// QueryIntAttribute/QueryIntValue, instead of trying to initialize to
// an out-of-range value and check if it was untouched.
//
// We should probably also make simple wrappers for things like iterating
// over children with some callback, initializing with "required" attributes
// or else throwing an exception, etc.
//
//  --zahlman

namespace FIFE { namespace map { namespace loaders { namespace xml {

	XML::~XML() {
		cleanup();
	}

	void XML::cleanup() {
		m_cursor.elevation.reset();
		m_cursor.elevationNumber = -1;
	}

	MapPtr XML::loadFile(const std::string& path) {
		cleanup();

		TiXmlDocument doc(path);

		if (!doc.LoadFile()) {
			Log("xmlmap")
				<< "Error loading file " << path << ": "
				<< doc.ErrorDesc();
			return MapPtr();
		}

		TiXmlNode* node = doc.RootElement();
		TiXmlElement* el;
		if (node)
			el = node->ToElement();
		else {
			Log("xmlmap") << "Error: could not switch to xml:element?";
			return MapPtr();
		}

		if (node && strcmp(el->Value(), "map") != 0)  {
			Log("xmlmap")
				<< "Error loading file " << path << ": "
				<< "Root element is " << el->Value() << " instead of 'map'";
			return MapPtr();
		}

		m_map.reset();
		try {
			loadMap( el );
		} catch(Exception& e) {
			Warn("xmlmap") << e.getMessage();
			if( m_map ) {
				m_map.reset();
			}
			throw;
		}
		return m_map;
	}

	void XML::loadMap(TiXmlElement* el) {
		std::string mapname;
		int format_version;

		// Read properties
		TiXmlElement* props_element = el->FirstChildElement("properties");
		if (!props_element) {
			throw InvalidFormat("no <properties> section in <map>");
		}
		mapname        = xmlutil::queryElement<std::string>(props_element,"name");
		format_version = xmlutil::queryElement<int>(props_element,"format");

		if( props_element->NextSiblingElement("properties") ) {
			throw InvalidFormat("more than one <properties> section in <map>");
		}

		if( mapname == "" ) {
			throw InvalidFormat("empty <name> in <map><properties>");
		}

		if( format_version != 1 ) {
			throw InvalidFormat("wrong <format> in <map><properties>");
		}

		// Deprecated sections
		TiXmlElement* el1 = el->FirstChildElement("tileset");
		if (el1) {
			Warn("xmlmap") 
				<< "Ignoring <tileset> - put it into an Archetype. "
				<< "Sorry for breaking working maps :-(";
		}

		el1 = el->FirstChildElement("spriteset");
		if (el1) {
			Warn("xmlmap") 
				<< "Ignoring <spriteset>. "
				<< "Sorry for breaking working maps :-(";
		}

		el1 = el->FirstChildElement("animation");
		if (el1) {
			Warn("xmlmap") 
				<< "Ignoring <animation> "
				<< "Sorry for breaking working maps :-(";
		}

		// Geometries + Archetypes
		TiXmlElement* geometry_element = el->FirstChildElement("geometries");
		if( geometry_element ) {
			geometry_element = geometry_element->FirstChildElement("geometry");
			while(geometry_element) {
				Geometry::registerGeometry(s_geometry_info::load(geometry_element));
				geometry_element = geometry_element->NextSiblingElement("geometry");
			}
		}

		el1 = el->FirstChildElement("archetypes");
		if (el1) {
			loadArchetypes(el1);
		}

		// Finally create map, load metadata and loop through elevation
		m_map = Map::create();
		m_map->setMapName(mapname);

		TiXmlElement* metadata_element = el->FirstChildElement("metadata");
		if (metadata_element) {
			xmlutil::loadMetadata(metadata_element,m_map.get());
		}


		TiXmlElement* el2 = el->FirstChildElement("elevation");
		if (!el2) {
			throw Exception("Error: found no 'elevation' entry!");
		}
		while (el2) {
			loadElevation(el2);
			el2 = el2->NextSiblingElement("elevation");
		}

		// I'm sure this can be refactored into something nicer. --zahlman
		el1 = el->FirstChildElement("run");
		if (el1) {
			el2 = el1->FirstChildElement("script");
			if (el2) {
				Log("xmlmap") << "DEBUG: run script";
				const char * scriptname = el2->Attribute("source");
				if (scriptname) {
					m_map->setScript(Map::OnStartScript,
					                 ScriptContainer::fromFile(scriptname));
				} else {
					throw Exception("Error: no 'source' attribute in 'script'");
				}
			} else {
				el2 = el1->FirstChildElement("string");
				if (el2) {
					const char* cmd = el2->FirstChild()->Value();
					if (cmd) {
						m_map->setScript(Map::OnStartScript,
						                 ScriptContainer::fromString(cmd));
					} else {
						throw Exception ("Error: no valid run->string command");
					}
				} else {
					throw Exception("Error: neither 'string' nor 'script' child in 'run'");
				}
			}
		}
	}

	void XML::loadArchetypes(TiXmlElement* e) {
		assert(e);
		e = e->FirstChildElement("archetype");
		while (e) {
			const char* type = e->Attribute("type");
			const char* source = e->Attribute("source");

			if (!type) {
				throw InvalidFormat("no type attribute on <archetype>");
			}

			if (!source) {
				XMLArchetype* xmlat = new XMLArchetype(e);	
				Factory::instance()->addArchetype(xmlat);
			} else {
				Factory::instance()->loadArchetype(type, source);
			}

			e = e->NextSiblingElement("archetype");
		}
	}

	void XML::loadElevation(TiXmlElement* el) {
		int refgrid = 0;

		TiXmlElement* props_element = el->FirstChildElement("properties");
		if (!props_element) {
			throw InvalidFormat("no <properties> section in <elevation>");
		}
		if( props_element->NextSiblingElement("properties") ) {
			throw InvalidFormat("more than one <properties> section in <elevation>");
		}

		refgrid = xmlutil::queryElement<int>(props_element,"refgrid");


		TiXmlElement* el2 = el->FirstChildElement("layer");
		if (!el2) {
			throw Exception("Error: found no 'layer' entry!");
		}
		m_cursor.elevation = Elevation::create();
		m_cursor.elevationNumber += 1;

		TiXmlElement* metadata_element = el->FirstChildElement("metadata");
		if (metadata_element) {
			xmlutil::loadMetadata(metadata_element, m_cursor.elevation.get());
		}

		try{
			while (el2) {
				loadLayer(el2);
				el2 = el2->NextSiblingElement("layer");
			}
		} catch(...) {
			m_cursor.elevation.reset();
			throw;
		}
		m_cursor.elevation->setReferenceLayer(refgrid);
		m_map->addElevation(m_cursor.elevation);
	}


	void XML::loadLayer(TiXmlElement* el) {
		assert(el);

		Point size, shift;
		int geometry;

		TiXmlElement* props_element = el->FirstChildElement("properties");
		if (!props_element) {
			throw InvalidFormat("no <properties> section in <layer>");
		}
		if( props_element->NextSiblingElement("properties") ) {
			throw InvalidFormat("more than one <properties> section in <elevation>");
		}
		size     = xmlutil::queryElement<Point>(props_element,"size");
		shift    = xmlutil::queryElement<Point>(props_element,"shift",Point());
		geometry = xmlutil::queryElement<int>(props_element,"geometry");

		m_cursor.width  = size.x;
		m_cursor.height = size.y;

		ElevationPtr me = m_cursor.elevation;
		LayerPtr layer = Layer::create(size,geometry);
		me->addLayer(layer);
		m_cursor.layer = me->getNumLayers() - 1;
		layer->setShift(shift);

		TiXmlElement* metadata_element = el->FirstChildElement("metadata");
		if (metadata_element) {
			xmlutil::loadMetadata(metadata_element, layer.get());
		}

		Log("xmlmap")
			<< "Loading layer #" << m_cursor.layer
			<< "  size: " << size;


 		TiXmlElement* mdat = el->FirstChildElement("data");
 		if (mdat) { // plain data found
			loadLayerData(mdat);
 		} else if ( (mdat = el->FirstChildElement("sparsedata")) ) {
			// "sparsedata" found
			loadLayerSparseData(mdat);
		}
#if 0
		if ( (mdat = el->FirstChildElement("image")) ) {
			// image-as-data found
			loadLayerImage(mdat);
		}
#endif
		
 		TiXmlElement* obj_element = el->FirstChildElement("objects");
 		if (!obj_element) {
			Log("xmlmap") << "Info: layer does not contain static objects";
			return;
		}

 		for (obj_element = obj_element->FirstChildElement("object");
		     obj_element;
		     obj_element = obj_element->NextSiblingElement("object")) {
			loadObject(obj_element);
 		}
	}

	void XML::loadObject(TiXmlElement* element) {
		assert( element );
		ObjectPtr object(ObjectInfo::create());

		const char* proto_name = element->Attribute("prototype");
		if( proto_name ) {
			// Shortcut for the <object prototype="proto" x="100" y="100"/>
			// case.
			Point p;
			int attr_ok, x,y;

			object->loadPrototype( proto_name );

			// FIXME: Policy?
			// Not sure whether to allow setting only x or y?
			attr_ok = element->QueryIntAttribute("x",&x );
			if( attr_ok == TIXML_SUCCESS ) {
				p.x = x;

				attr_ok = element->QueryIntAttribute("y",&y );
				if( attr_ok == TIXML_SUCCESS ) {
					p.y = y;
				}
				object->setPosition(p);
			}

			
		} else {
			// ObjectLoader does the hard work.
			// Might be inefficient, as it translates to
			// an internal state. But we'll see if this
			// amounts to something.

			Prototype loader(element);
			loader.merge( object.get() );
		}

		// And finally add it.

		object->debugPrint();
		LayerPtr layer = m_cursor.elevation->getLayer(m_cursor.layer);
		object->setLayer( m_cursor.layer );
		layer->addObject( object );
	}

	void XML::loadLayerData(TiXmlElement* element) {
		LayerPtr layer = m_cursor.elevation->getLayer(m_cursor.layer);
		int width    = m_cursor.width;
		int height   = m_cursor.height;

		// seek for w*h <tile> entries
		unsigned int x = 0;
		unsigned int y = 0;
		TiXmlElement* tilenode = element->FirstChildElement("tile");
		if (!tilenode) {
			throw Exception("Error loading <tile> node!");
		}
		

		while (tilenode) {
			int tilegid = -1;
			tilenode->QueryIntAttribute("gid", &tilegid);
			if (tilegid > -1) {
				size_t iid = Factory::instance()->getTileImageId(tilegid);
				layer->setTileImage(x,y, iid);
			} else {
				Log("xmlmap") << "Error: a tile is missing a 'gid' attribute!";
			}
			tilenode = tilenode->NextSiblingElement("tile");

			x++;
			if (x == static_cast<unsigned int>(width)) {
				y++;
				x = 0;
			}
		}

		if ((x != 0) || (y != static_cast<unsigned int>(height))) {
			Log("xmlmap")
				<< "Error: corrupt elevation!"
				<< " number of <tiles> != width*height";
		}
	}

	void XML::loadLayerSparseData(TiXmlElement* element) {
		LayerPtr layer = m_cursor.elevation->getLayer(m_cursor.layer);

		TiXmlElement *sdn = element->FirstChildElement("tileat");
		if (!sdn) {
			Log("xmlmap") << "Warning: found an empty 'sparsedata' node";
			return;
		}

		while (sdn) {
			int gid = -1;
			int x = -1;
			int y = -1;
			sdn->QueryIntAttribute("gid", &gid);
			sdn->QueryIntAttribute("x", &x);
			sdn->QueryIntAttribute("y", &y);
			if ((gid == -1) || (x == -1) || (y == -1)) {
				throw Exception(
					"Error: 'tileat' nodes have to specify:"
					" gid, x and y (>=0)");
			}

			layer->setTileImage(x, y, Factory::instance()->getTileImageId(gid));
			sdn = sdn->NextSiblingElement("tileat");
		}
	}

#if 0
	void XML::loadLayerImage(TiXmlElement* element) {
		Layer* Layer = m_cursor.elevation->getLayer(m_cursor.layer);
		int width  = m_cursor.width;
		int height = m_cursor.height;

		const char* imgsrc = element->Attribute("source");
		if (!imgsrc) {
			throw Exception("Error: image node contains no 'source' attribute");
		}
			
		SDL_Surface *img = icL.loadImageAsSurface(imgsrc);
		if (!img) {
			throw Exception(std::string("Error loading image ") +
			                imgsrc + " for elevation");
		}

		if ((img->w != width) || (img->h != height)) {
			Log("xmlmap") << "Error: image size does not equal elevation";
			// throw ???
		}

		if (SDL_MUSTLOCK(img)) {
			SDL_LockSurface(img);
		}
		Log("xmlmap") 
			<< "image-src: " << imgsrc << " " << img->w << "x" << img->h 
			<< " " << int(img->format->BytesPerPixel) 
			<< " bpp, pitch: " << int(img->pitch);
		
		uint8_t *pixel = static_cast<uint8_t*>(img->pixels);
		uint8_t v;
		for (int y=0; y < height; y++) {
			for (int x=0; x < width; x++) {
				v = *pixel;
				size_t iid = Factory::instance()->getTileImageId(v);
				if (iid == 0) {
					Log("xmlmap") 
						<< "Invalid id: " << int(v) << " at (" << x << ", " << y << ")";
				}
				Layer->setTileImage(x, y, iid);
				pixel += img->format->BytesPerPixel;
			}
			// does this work everywhere?
			pixel += (img->pitch / img->format->BytesPerPixel + 
			          img->pitch % img->format->BytesPerPixel - img->w);
		}
		
		if (SDL_MUSTLOCK(img)) {
			SDL_UnlockSurface(img);
		}
		SDL_FreeSurface(img);
	}
#endif

} } } } // FIFE::map::loaders::xml
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
