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
#include "map/grid.h"
#include "map/objectinfo.h"
#include "map/objectmanager.h"
#include "tinyxml/tinyxml.h"
#include "debugutils.h"
#include "exception.h"
#include "imagecache.h"

#include "xml.h"

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
		m_cursor.elevation =0;
		m_cursor.elevationNumber = -1;
		icL.clear();
	}

	Map* XML::loadFile(const std::string& path) {
		cleanup();

		TiXmlDocument doc(path);

		if (!doc.LoadFile()) {
			Log("xmlmap")
				<< "Error loading file " << path << ": "
				<< doc.ErrorDesc();
			return 0;
		}

		TiXmlNode* node = doc.RootElement();
		TiXmlElement* el;
		if (node)
			el = node->ToElement();
		else {
			Log("xmlmap") << "Error: could not switch to xml:element?";
			return 0;
		}

		if (node && strcmp(el->Value(), "map") != 0)  {
			Log("xmlmap")
				<< "Error loading file " << path << ": "
				<< "Root element is " << el->Value() << " instead of 'map'";
			return 0;
		}

		m_map = 0;
		try {
			loadMap( el );
		} catch(Exception& e) {
			Warn("xmlmap") << e.getMessage();
			if( m_map )
				delete m_map;
			return 0;
		}
		return m_map;
	}

	void XML::loadMap(TiXmlElement* el) {
		const char* mapname = el->Attribute("name");
		int mapid = -1;
		el->QueryIntAttribute("id", &mapid);
		Log("xmlmap") << "Loading: " << mapname << " (" << mapid << ")";

		m_map = new Map(mapname);

		TiXmlElement* el1 = el->FirstChildElement("tileset");
		if (el1) {
			Warn("xmlmap") 
				<< "Ignoring <tileset> - put it into an Archetype. "
				<< "Sorry for breaking working maps :-(";
		}

		el1 = el->FirstChildElement("spriteset");
		while (el1) {
			loadSpriteSet(el1);
			el1 = el1->NextSiblingElement("spriteset");
		}

		el1 = el->FirstChildElement("animation");
		while (el1) {
			icL.loadAnimation(el1);
			el1 = el1->NextSiblingElement("animation");
		}

		el1 = el->FirstChildElement("geometry");
		while(el1) {
			Geometry::registerGeometry(s_geometry_info::load(el1));
			el1 = el1->NextSiblingElement("geometry");
		}

		el1 = el->FirstChildElement("archetypes");
		if (el1) {
			loadArchetypes(el1);
		}

		TiXmlElement* el2 = el->FirstChildElement("elevation");
		if (!el2) {
			throw Exception("Error: found no 'elevation' entry!");
		}
		while (el2) {
			loadElevation(el2);
			el2 = el->NextSiblingElement("elevation");
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

	void XML::loadSpriteSet(TiXmlElement* element) {
		TiXmlElement* sprite = element->FirstChildElement("sprite");
		while (sprite) {
			const char* sprite_filename = sprite->Attribute("source");
			int sprite_gid = -1;
			sprite->QueryIntAttribute("id", &sprite_gid);
			if (sprite_gid == -1 || !sprite_filename) {
				Log("xmlmap")
					<< "Error: 'sprite' has to specify valid"
					<< " sprite_gid and source attributes!";
			} else {
				spriteFilesMap[sprite_gid] = std::string(sprite_filename);
			}
			sprite = sprite->NextSiblingElement("sprite");
		}
	}

	void XML::loadTiles(TiXmlElement* el) {
		if (icL.loadTileset(el)) {
			throw Exception("Tileset error, aborting ...");
		}
	}

	void XML::loadArchetypes(TiXmlElement* el1) {
		el1 = el1->FirstChildElement("archetype");
		while(el1) {
			const char* type = el1->Attribute("type");
			const char* file = el1->Attribute("source");

			if( type == 0 ) {
				throw InvalidFormat("no type attribute on <archetype>");
			}

			if( file == 0 ) {
				throw InvalidFormat("no file attribute on <archetype>");
			}
			
			Factory::instance()->loadArchetype(type,file);
			el1 = el1->NextSiblingElement("archetype");
		}

	}

	void XML::loadElevation(TiXmlElement* el) {
		int reference_grid = 0;
 		el->QueryIntAttribute("refgrid", &reference_grid);
		
		TiXmlElement* el2 = el->FirstChildElement("layer");
		if (!el2) {
			throw Exception("Error: found no 'layer' entry!");
		}
		elevation_info structure;
		structure.numberOfGrids = 0;

		m_cursor.elevation = new Elevation(structure);
		m_cursor.elevationNumber += 1;
		try{
			while (el2) {
				loadLayer(el2);
				el2 = el2->NextSiblingElement("layer");
			}
		} catch(...) {
			delete m_cursor.elevation;
			throw;
		}
		m_cursor.elevation->setReferenceGrid(reference_grid);
		
		m_map->addElevation(m_cursor.elevation);
	}


	void XML::loadLayer(TiXmlElement* el) {
		assert(el);
 		int width, height, shiftx=0, shifty=0;
		int geometry;

 		el->QueryIntAttribute("width", &width);
 		el->QueryIntAttribute("height", &height);
 		el->QueryIntAttribute("geometry", &geometry);

 		el->QueryIntAttribute("shiftx", &shiftx);
 		el->QueryIntAttribute("shifty", &shifty);

		const char* outline = el->Attribute("outline");

		m_cursor.width = width;
		m_cursor.height = height;
		m_cursor.geometry = geometry;

		Elevation* me = m_cursor.elevation;
		Grid* grid = new Grid(Point(width,height),geometry); 
		me->addGrid(grid);
		m_cursor.layer = me->getNumGrids() - 1;

		Log("xmlmap")
			<< "Loading layer #" << m_cursor.layer
			<< "  size: " << Point(width,height);

		if(outline) {
			grid->setOverlayImage(outline);
		}
		grid->setShift(Point(shiftx,shifty));

 		TiXmlElement* mdat = el->FirstChildElement("data");
 		if (mdat) { // plain data found
			loadLayerData(mdat);
 		} else if ( (mdat = el->FirstChildElement("sparsedata")) ) {
			// "sparsedata" found
			loadLayerSparseData(mdat);
		} else if ( (mdat = el->FirstChildElement("image")) ) {
			// image-as-data found
			loadLayerImage(mdat);
		}
		
// 		if (!mdat) {
// 			throw Exception("Error: elevation with no valid data child");
//  		// end of any-data handler section
// 		}

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
		int sprite_gid = -1;
		int grid_pos_x = -2;
		int grid_pos_y = -2;
		int orientation = -1;
		int frame = -1;
		const char *obj_typename = element->Attribute("typename");
		element->QueryIntAttribute("spriteid", &sprite_gid);
		element->QueryIntAttribute("grid_x", &grid_pos_x);
		element->QueryIntAttribute("grid_y", &grid_pos_y);
		element->QueryIntAttribute("orientation", &orientation);
		element->QueryIntAttribute("frame", &frame);

		if (!obj_typename) {
			Log("xmlmap") << "Error: object does not specify typename attribute";
			return;
		}

		if (sprite_gid == -1 || (grid_pos_x == -2 && grid_pos_y == -2)) {
			Log("xmlmap") << "Error: invalid object attribute values";
			return;
		}

		ObjectManager* mom = m_map->getObjectManager();
		ObjectInfo* obj = new ObjectInfo();
		if (obj) {
			obj->setVisualLocation( spriteFilesMap[sprite_gid] );
			obj->set<std::string>(ObjectInfo::ObjectTypeParam,obj_typename);
			if (orientation > -1) {
				obj->set<size_t>(ObjectInfo::OrientationParam,orientation);
			}
			Location& loc = obj->getLocation();

			loc.elevation = m_cursor.elevationNumber;
			loc.position = Point(grid_pos_x, grid_pos_y);
			loc.grid = m_cursor.layer;

			obj->setStatic(true);
			obj->debugPrint();
			mom->addObject(obj);
		}
	}

	void XML::loadLayerData(TiXmlElement* element) {
		Grid* grid = m_cursor.elevation->getGrid(m_cursor.layer);
		int width  = m_cursor.width;
		int height = m_cursor.height;

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
				grid->setTileImage(x,y, iid);
			} else {
				Log("xmlmap") << "Error: a tile is missing a 'gid' attribute!";
				tilegid = 0;
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
		Grid* grid = m_cursor.elevation->getGrid(m_cursor.layer);

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

			grid->setTileImage(x, y, Factory::instance()->getTileImageId(gid));
			sdn = sdn->NextSiblingElement("tileat");
		}
	}

	void XML::loadLayerImage(TiXmlElement* element) {
		Grid* grid = m_cursor.elevation->getGrid(m_cursor.layer);
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
				grid->setTileImage(x, y, iid);
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

} } } } // FIFE::map::loaders::xml
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
