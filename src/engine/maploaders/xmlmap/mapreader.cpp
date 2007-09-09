#include "xmlmap.h"
#include "rawdata.h"
#include "map.h"
#include "mapelevation.h"
#include "tinyxml/tinyxml.h"
#include "object.h"
#include "objectfactory.h"
#include <SDL_image.h>

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
		icL.clear();
	}

	Map* XMLMap::loadFile(const std::string& path) {
		Map* result = NULL;
		cleanup();
		//VFS* vfs = VFS::instance();
		TiXmlDocument doc(path);

		if (!doc.LoadFile()) {
			Log("xmlmap") << "Error loading file " << path << ": " << doc.ErrorDesc();
			return result;
		}
		TiXmlNode* node = doc.RootElement();
		TiXmlElement* el;
		if (node)
			el = node->ToElement();
		else {
			Log("xmlmap") << "Error: could not switch to xml:element?";
			return result;
		}
		if (node && strcmp(el->Value(), "map") == 0) {
			const char* mapname = el->Attribute("name");
			int mapid = -1;
			el->QueryIntAttribute("id", &mapid);
			Log("xmlmap") << "Loading: " << mapname << " (" << mapid << ")";

			result = new Map(mapname);
			TiXmlElement* el1 = el->FirstChildElement("tileset");
			if (el1 == NULL) {
				delete result;
				return NULL;
			}
			while(el1 != NULL) {
				result = loadTiles(result, el1);
				el1 = el1->NextSiblingElement("tileset");
			}
			el1 = el->FirstChildElement("spriteset");
			while (el1 != NULL) {
				TiXmlElement* sprite = el1->FirstChildElement("sprite");
				while (sprite != NULL) {
					const char* sprite_filename = sprite->Attribute("source");
					int sprite_gid = -1;
					sprite->QueryIntAttribute("id", &sprite_gid);
					if (sprite_gid == -1 || sprite_filename == NULL) {
						Log("xmlmap") << "Error: 'sprite' has to specify valid sprite_gid and source attributes!";
					}
					else
						icL.loadImageAs(sprite_filename, sprite_gid);

					sprite = sprite->NextSiblingElement("sprite");
				}
				el1 = el1->NextSiblingElement("spriteset");
			}
			TiXmlElement* el2 = el->FirstChildElement("elevation");
			if (el2 == NULL) {
				Log("xmlmap") << "Error: found no 'elevation' entry!";
				delete result;
				return NULL;
			}
			while (el2 != NULL) {
				result = loadElevation(result, el2);
				el2 = el->NextSiblingElement("elevation");
			}
		}
		else {
			Log("xmlmap") << "Error loading file " << path << 
				": Root element is " << el->Value() << " instead of 'map'";
		}
		return result;
	}
	Map* XMLMap::loadTiles(Map* map, TiXmlElement* el) {
		if (icL.loadTileset(el)) {
			Log("xmlmap") << "Tileset error, aborting ...";
			delete map;
			return NULL;
		}
		return map;
	}
	Map* XMLMap::loadElevation(Map* map, TiXmlElement* el) {
		// el should be ok
		int width, height;
		el->QueryIntAttribute("width", &width);
		el->QueryIntAttribute("height", &height);
		bool firstRun = (map->getElevationCount() == 0);

		MapElevation* me = NULL;
		if (firstRun)
			me = new MapElevation(static_cast<unsigned int>(width),
					static_cast<unsigned int>(height));
		else
			me = map->getElevation(0);


		TiXmlElement* mdat = el->FirstChildElement("data");
		if (mdat != NULL) {
			/*Log("xmlmap") << "Error: elevation does not contain a 'data' entry!";
				delete map;
				return NULL;*/
			// seek for w*h <tile> entries
			unsigned int x = 0;
			unsigned int y = 0;
			TiXmlElement* tilenode = mdat->FirstChildElement("tile");
			if (tilenode == NULL) {
				Log("xmlmap") << "Error loading <tile> node!";
				delete map;
				return NULL;
			}

			while (tilenode != NULL) {
				int tilegid = -1;
				tilenode->QueryIntAttribute("gid", &tilegid);
				if (tilegid > -1) {
					MapTile* mt = me->getTile(x, y);
					size_t iid = icL.getImageForId(tilegid);
					//Log("DEBUG") << "setting " << iid << " for " << tilegid;
					//mt->setImageIDs(iid, 0);
					if (firstRun)
						mt->setFloorImageID(iid);
					else
						mt->setRoofImageID(iid);
				}
				else {
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
			if ((x != 0) ||
					(y != static_cast<unsigned int>(height)))
				Log("xmlmap") << "Error: corrupt elevation! number of <tiles> != width*height";
		} // end plain "data" found
		else {
			mdat = el->FirstChildElement("sparsedata");
			if (mdat != NULL) {
				TiXmlElement *sdn = mdat->FirstChildElement("tileat");
				if (sdn == NULL) {
					Log("xmlmap") << "Warning: found an empty 'sparsedata' node";
				}
				while (sdn != NULL) {
					int gid = -1;
					int x = -1;
					int y = -1;
					sdn->QueryIntAttribute("gid", &gid);
					sdn->QueryIntAttribute("x", &x);
					sdn->QueryIntAttribute("y", &y);
					if ((gid == -1) || (x == -1) || (y == -1)) {
						Log("xmlmap") << "Error: 'tileat' nodes have to specify gid, x and y (>=0)";
						delete map;
						return NULL;
					}
					MapTile* mt = me->getTile(x, y);
					if (firstRun)
						mt->setFloorImageID(icL.getImageForId(gid));
					else
						mt->setRoofImageID(icL.getImageForId(gid));
				}
			} // end "sparsedata" found
			else {
				mdat = el->FirstChildElement("image");
				if (mdat == NULL) {
					Log("xmlmap") << "Error: elevation with no valid data child";
					delete map;
					return NULL;
				}
				else {
					const char* imgsrc = mdat->Attribute("source");
					if (imgsrc == NULL) {
						Log("xmlmap") << "Error: image node contains no 'source' attribute";
						delete map;
						return NULL;
					}
					SDL_Surface* img = IMG_Load(imgsrc);
					if (img == NULL) {
						Log("xmlmap") << "Error loading image " << imgsrc << " for elevation";
						delete map;
						return NULL;
					}
					if ((img->w != width)|(img->h != height)) {
						Log("xmlmap") << "Error: image size does not equal elevation";
					}
					SDL_LockSurface(img);
					uint8_t *pixel = static_cast<uint8_t*>(img->pixels);
					for (int y=0; y < height; y++) {
						for (int x=0; x < width; x++) {
							if (*pixel > 0) {
								MapTile* mt = me->getTile(x, y);
								size_t iid = icL.getImageForId(int(*pixel));
								if (firstRun)
									mt->setFloorImageID(iid);
								else
									mt->setRoofImageID(iid);
							}
							pixel += img->format->BytesPerPixel;
						}
					}
					SDL_UnlockSurface(img);
					SDL_FreeSurface(img);
				} // end image-as-data found
			}
		} // end of any-data handler section
		TiXmlElement* obj_con = el->FirstChildElement("objects");
		if (obj_con != NULL) {
			TiXmlElement* tobj = obj_con->FirstChildElement("object");
			while (tobj != NULL) {
				int sprite_gid = -1;
				int grid_pos = -2;
				const char *obj_typename = tobj->Attribute("typename");
				tobj->QueryIntAttribute("spriteid", &sprite_gid);
				tobj->QueryIntAttribute("gridpos", &grid_pos);
				if (obj_typename == NULL) {
					Log("xmlmap") << "Error: object does not specify typename attribute";
					tobj = tobj->NextSiblingElement("object");
					continue;
				}
				if (sprite_gid == -1 || grid_pos == -2) {
					Log("xmlmap") << "Error: invalid object attribute values";
					tobj = tobj->NextSiblingElement("object");
					continue;
				}
				FIFE::Object *obj =
				 FIFE::ObjectFactory::createStaticImage(obj_typename,icL.getImageForId(sprite_gid));
				obj->setPosition(grid_pos);
				me->addObject(obj);

				printf("obj: %s at %i id %i\n", obj_typename, grid_pos, sprite_gid);

				tobj = tobj->NextSiblingElement("object");
			}
		}
		else {
			Log("xmlmap") << "Info: map does not contain static objects";
		}

		if (firstRun)
			map->addElevation(me);
		return map;  
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
