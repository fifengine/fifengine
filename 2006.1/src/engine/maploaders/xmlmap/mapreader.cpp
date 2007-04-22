/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "xmlmap.h"
#include "rawdata.h"
#include "map.h"
#include "mapelevation.h"
#include "tinyxml/tinyxml.h"
#include "object.h"
#include "imagecache.h"
#include <SDL_image.h>


#include "debugutils.h"
#include "exception.h"
#include "maptilegrid.h"
#include "mapgeometry.h"

namespace FIFE {
	XMLMap::XMLMap() : MapLoader("XMLMap") {
	}
	XMLMap::~XMLMap() {
		cleanup();
	}

	XMLMap::XMLMap(const XMLMap&) : MapLoader("XMLMap") {
	}
	XMLMap& XMLMap::operator = (const XMLMap&) {
		return *this;
	}

	void XMLMap::cleanup() {
		m_cursor.elevation =0;
		m_cursor.layer = -1;
		icL.clear();
	};

	void XMLMap::advanceElevationCursor() {
		++m_cursor.layer;
	};

	Map* XMLMap::loadFile(const std::string& path) {
		cleanup();

		TiXmlDocument doc(path);

		if (!doc.LoadFile()) {
			Log("xmlmap")
				<< "Error loading file " << path << ": "
				<< doc.ErrorDesc();
			return 0;
		};

		TiXmlNode* node = doc.RootElement();
		TiXmlElement* el;
		if (node)
			el = node->ToElement();
		else {
			Log("xmlmap") << "Error: could not switch to xml:element?";
			return 0;
		};

		if (node && strcmp(el->Value(), "map") != 0)  {
			Log("xmlmap")
				<< "Error loading file " << path << ": "
				<< "Root element is " << el->Value() << " instead of 'map'";
			return 0;
		};

		m_map = 0;
		try {
			loadMap( el );
		} catch(Exception& e) {
			Warn("xmlmap") << e.getMessage();
			if( m_map )
				delete m_map;
			return 0;
		};
		return m_map;
	};

	void XMLMap::loadMap(TiXmlElement* el)
	{
		const char* mapname = el->Attribute("name");
		int mapid = -1;
		el->QueryIntAttribute("id", &mapid);
		Log("xmlmap") << "Loading: " << mapname << " (" << mapid << ")";

		m_map = new Map(mapname);

		TiXmlElement* el1 = el->FirstChildElement("tileset");
		if (el1 == NULL)
			throw Exception("no tileset!");

		while(el1 != NULL) {
			loadTiles(el1);
			el1 = el1->NextSiblingElement("tileset");
		};

		el1 = el->FirstChildElement("spriteset");
		while (el1 != NULL) {
			loadSpriteSet(el1);
			el1 = el1->NextSiblingElement("spriteset");
		}

		el1 = el->FirstChildElement("animation");
		while (el1 != NULL) {
			icL.loadAnimation(el1);
			el1 = el1->NextSiblingElement("animation");
		}

		TiXmlElement* el2 = el->FirstChildElement("elevation");
		if (el2 == NULL)
			throw Exception("Error: found no 'elevation' entry!");

		while (el2 != NULL) {
			advanceElevationCursor(); // temp hack
			loadElevation(el2);
			el2 = el->NextSiblingElement("elevation");
		};
		m_map->addElevation( m_cursor.elevation );

		el1 = el->FirstChildElement("run");
		if (el1) {
			el2 = el1->FirstChildElement("script");
			if (el2) {
				Log("xmlmap") << "DEBUG: run script";
				const char * scriptname = el2->Attribute("source");
				if (scriptname)
					m_map->setScript(Map::OnMapStartScript,	ScriptContainer::fromFile(scriptname) );
				else
					throw("Error: no 'source' attribute in 'script'");
			}
			else {
				el2 = el1->FirstChildElement("string");
				if (el2) {
					const char* cmd = el2->FirstChild()->Value();
					if (cmd)
						m_map->setScript(Map::OnMapStartScript,	ScriptContainer::fromString(cmd) );
					else
						throw("Error: no valid run->string command");
				}
				else
					throw Exception("Error: neither 'string' nor 'script' child in 'run'");
			}
		}
	};

	void XMLMap::loadSpriteSet(TiXmlElement* element) {
		TiXmlElement* sprite = element->FirstChildElement("sprite");
		while (sprite != NULL) {
			const char* sprite_filename = sprite->Attribute("source");
			int sprite_gid = -1;
			sprite->QueryIntAttribute("id", &sprite_gid);
			if (sprite_gid == -1 || sprite_filename == NULL) {
				Log("xmlmap")
					<< "Error: 'sprite' has to specify valid"
					<< " sprite_gid and source attributes!";
			} else
				icL.loadImageAs(sprite_filename, sprite_gid);

			sprite = sprite->NextSiblingElement("sprite");
		}
	};

	void XMLMap::loadTiles(TiXmlElement* el) {
		if (icL.loadTileset(el))
			throw Exception("Tileset error, aborting ...");
	};

	void XMLMap::loadElevation(TiXmlElement* el) {
 		// el should be ok
 		int width, height;
 		el->QueryIntAttribute("width", &width);
 		el->QueryIntAttribute("height", &height);

		if( m_cursor.elevation == 0 ) {
			m_cursor.elevation = new MapElevation( MapStructure::falloutElevationFormat(Point(width, height) ));
		};
		m_cursor.width = width;
		m_cursor.height = height;

 		TiXmlElement* mdat = el->FirstChildElement("data");
 		if (mdat != NULL) { // plain data found
			loadElevationData(mdat);
 		} else if( (mdat = el->FirstChildElement("sparsedata")) ) {
			// "sparsedata" found
			loadElevationSparseData(mdat);

		} else if( (mdat = el->FirstChildElement("image")) ) {
			// image-as-data found
			loadElevationImage(mdat);
		} ;
		if (mdat == NULL)
			throw Exception("Error: elevation with no valid data child");
 		// end of any-data handler section

 		TiXmlElement* obj_element = el->FirstChildElement("objects");
 		if (obj_element == NULL) {
			Log("xmlmap") << "Info: map does not contain static objects";
			return;
		};

 		obj_element = obj_element->FirstChildElement("object");
		while (obj_element != NULL) {
			loadObject(obj_element);
 			obj_element = obj_element->NextSiblingElement("object");
 		};
	}

	void XMLMap::loadObject(TiXmlElement* element) {
		//DEBUG_PRINT("loading object");
		int sprite_gid = -1;
		int grid_pos = -2;
		const char *obj_typename = element->Attribute("typename");
		element->QueryIntAttribute("spriteid", &sprite_gid);
		element->QueryIntAttribute("gridpos", &grid_pos);

		if (obj_typename == NULL) {
			Log("xmlmap") << "Error: object does not specify typename attribute";
			return;
		}

		if (sprite_gid == -1 || grid_pos == -2) {
			Log("xmlmap") << "Error: invalid object attribute values";
			return;
		}

		MapElevation* me = m_cursor.elevation;
		FIFE::Object *obj = new FIFE::Object(obj_typename, grid_pos);
		obj->setVisual(icL.getImageForId(sprite_gid));
		me->addObject(obj);

		printf("obj: %s at %i id %i\n", obj_typename, grid_pos, sprite_gid);
	};

	void XMLMap::loadElevationData(TiXmlElement* element) {
		MapTileGrid* grid = 0;
		// seek for w*h <tile> entries
		unsigned int x = 0;
		unsigned int y = 0;
		TiXmlElement* tilenode = element->FirstChildElement("tile");
		if (tilenode == NULL)
			throw Exception("Error loading <tile> node!");

		MapElevation* me = m_cursor.elevation;
		if( m_cursor.layer == 0 ) {
			grid = me->getTileGrid(0);
		} else {
			grid = me->getTileGrid(1);
		}
		int width  = m_cursor.width;
		int height = m_cursor.height;

		while (tilenode != NULL) {
			int tilegid = -1;
			tilenode->QueryIntAttribute("gid", &tilegid);
			if (tilegid > -1) {
				size_t iid = icL.getImageForId(tilegid);
				//Log("DEBUG") << "setting " << iid << " for " << tilegid;
				//mt->setImageIDs(iid, 0);
				grid->setImageId( Point(x,y), iid );
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
		};

		if ((x != 0) || (y != static_cast<unsigned int>(height))) {
			Log("xmlmap")
				<< "Error: corrupt elevation!"
				<< " number of <tiles> != width*height";
		};
	};

	void XMLMap::loadElevationSparseData(TiXmlElement* element) {
		TiXmlElement *sdn = element->FirstChildElement("tileat");
		if (sdn == NULL) {
			Log("xmlmap") << "Warning: found an empty 'sparsedata' node";
			return;
		}

		MapTileGrid* grid = 0;
		MapElevation* me = m_cursor.elevation;
		if( m_cursor.layer == 0 ) {
			grid = me->getTileGrid(0);
		} else {
			grid = me->getTileGrid(1);
		}
//		int width  = m_cursor.width;
//		int height = m_cursor.height;

		while (sdn != NULL) {
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

			grid->setImageId(Point(x, y),gid );
		}
	};

	void XMLMap::loadElevationImage(TiXmlElement* element ) {
		int width  = m_cursor.width;
		int height = m_cursor.height;

		const char* imgsrc = element->Attribute("source");
		if (imgsrc == NULL)
			throw Exception("Error: image node contains no 'source' attribute");

		//SDL_Surface* img = IMG_Load(imgsrc);
		SDL_Surface *img = icL.loadImageAsSurface(imgsrc);
		if (img == NULL)
			throw Exception(std::string("Error loading image ")+  imgsrc +" for elevation");

		if ((img->w != width)|(img->h != height)) {
			Log("xmlmap") << "Error: image size does not equal elevation";
			// throw ???
		};

		MapTileGrid* grid = 0;
		MapElevation* me = m_cursor.elevation;
		if( m_cursor.layer == 0 ) {
			grid = me->getTileGrid(0);
		} else {
			grid = me->getTileGrid(1);
		}

		if( SDL_MUSTLOCK(img) )
			SDL_LockSurface(img);
		Log("xmlmap") << "image-src: " << imgsrc << " " << img->w << "x" << img->h 
			<< " " << int(img->format->BytesPerPixel) << " bpp, pitch: " << int(img->pitch);
		uint8_t *pixel = static_cast<uint8_t*>(img->pixels);
		uint8_t v;
		for (int y=0; y < height; y++) {
			for (int x=0; x < width; x++) {
				v = *pixel;
				size_t iid = icL.getImageForId(v);
				if (iid == 0) {
					Log("xmlmap") << "Invalid id: " << int(v) << " at (" << x << ", " << y << ")";
				}
				grid->setImageId(Point(x, y),iid );
				pixel += img->format->BytesPerPixel;
			}
			// does this work everywhere?
			pixel += (img->pitch/img->format->BytesPerPixel + img->pitch % img->format->BytesPerPixel - img->w);
		}
		if( SDL_MUSTLOCK(img) )
			SDL_UnlockSurface(img);

		SDL_FreeSurface(img);
	};

} // FIFE
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
