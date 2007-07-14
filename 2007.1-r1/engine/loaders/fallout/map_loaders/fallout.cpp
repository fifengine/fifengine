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
#include <algorithm>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
// XXX Do we really need all this?
#include "map/structures/map.h"
#include "map/structures/layer.h"
#include "map/structures/objectinfo.h"
#include "map/structures/elevation.h"
#include "map/geometries/geometry.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfssourcefactory.h"
#include "util/debugutils.h"
#include "util/exception.h"
#include "video/imagecache.h"
#include "util/settingsmanager.h"

#include "fallout.h"
#include "dat2.h"
//#include "frm.h"
#include "header.h"
#include "list.h"
#include "objectfactory.h"
#include "providerdat2.h"
#include "providerdat1.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	Fallout::Fallout() : MapLoader("Fallout"), m_header(0), m_cached() {
		VFSSourceFactory* vfsfactory = VFSSourceFactory::instance();
		vfsfactory->addProvider(new ProviderDAT2());
		vfsfactory->addProvider(new ProviderDAT1());

////		Moved to content/scripts/startup.lua/load_vfs_sources
// 		VFS* vfs = VFS::instance();
// 		VFSSource* source = vfsfactory->createSource("master.dat");
// 		if (source) {
// 			vfs->addSource(source);
// 		}
// 		
// 		source = vfsfactory->createSource("critter.dat");
// 		if (source) {
// 			vfs->addSource(source);
// 		}

	}

	Fallout::~Fallout() {
		cleanup();
	}

	void Fallout::cleanup() {
		delete m_header;
		m_header = 0;

		m_cached.clear();
	}

	const Fallout::s_imgdata& Fallout::getImageID(const std::string& path) {
		type_cached::const_iterator i = m_cached.find(path);

		if (i == m_cached.end()) {
			s_imgdata imgdata;
			ImageCache* imgcache = ImageCache::instance();
			imgdata.cacheid = imgcache->addImageFromFile(path);
			imgdata.pal_animation = 0;

			i = m_cached.insert(std::make_pair(path, imgdata)).first;
		}
		return i->second;
	}

	MapPtr Fallout::loadFile(const std::string& filepath) {
		cleanup();
		RawDataPtr file = VFS::instance()->open(filepath);

		m_header = new Header(file);
		file->moveIndex(m_header->getNumGlobalVars() * 4 + 
		                m_header->getNumLocalVars() * 4);

		MapPtr map = Map::create();
		map->setMapName(m_header->getMapName());

		loadTiles(map, file);
		ignoreScripts(file);
		loadObjects(map, file);

		size_t start_elevation = m_header->getDefaultElevation();
		map->set<long>("_START_ELEVATION",start_elevation);

		Point playerPos(m_header->getDefaultPlayerPosition() % 200,
		                m_header->getDefaultPlayerPosition() / 200);

		map->getElevation(start_elevation)->set<Point>("_START_POSITION", playerPos);
		return map;
	}

	ElevationPtr Fallout::createElevation() {
		ElevationPtr elevation = Elevation::create();

		LayerPtr layer = Layer::create(Point(100,100),Geometry::FalloutTileGeometry);
		layer->set<std::string>("_OVERLAY_IMAGE","content/gfx/tiles/outlines/tile_outline_fallout.png");
		elevation->addLayer( layer );

		layer = Layer::create(Point(200,200),Geometry::FalloutObjectGeometry);
		layer->set<std::string>("_OVERLAY_IMAGE","content/gfx/objects/outlines/object_outline_fallout.png");
		layer->set<Point>("_OVERLAY_IMAGE_OFFSET", Point(-16, -12));
		elevation->addLayer( layer );

		layer = Layer::create(Point(100,100),Geometry::FalloutTileGeometry);
		layer->set<std::string>("_OVERLAY_IMAGE","content/gfx/tiles/outlines/tile_outline_fallout.png");
		layer->setShift(Point(0,96));
		elevation->addLayer( layer );

		elevation->setReferenceLayer(1);

		return elevation;
	}

	void Fallout::loadTiles(MapPtr map, RawDataPtr data) {
		list lst("art/tiles/tiles.lst");
		for (unsigned int e = 0; e < m_header->getNumElevations(); ++e) {
			ElevationPtr elevation = createElevation();

			for (unsigned int y = 0; y < 100; ++y) {
				for (unsigned int x = 0; x < 100; ++x) {
					unsigned int roof = data->read16Big();
					unsigned int floor = data->read16Big();

					// this one is strange - are fallout maps realy right-to left or am 
					// i just confused? ;)
					Point tilepos(99-x,y);

					LayerPtr floorgrid  = elevation->getLayer(0);
					LayerPtr roofgrid   = elevation->getLayer(2);
					if (roof > 1) {
						std::string roof_file = "art/tiles/" + lst.getProFile(roof);
						const s_imgdata& imgdata = getImageID(roof_file);
						roofgrid->setTileImage(tilepos, imgdata.cacheid);
					}

					if (floor > 1) {
						std::string floor_file = "art/tiles/" + lst.getProFile(floor);
						const s_imgdata& imgdata = getImageID(floor_file);
						floorgrid->setTileImage(tilepos, imgdata.cacheid);
					}
				}
			}
			LayerPtr layer = elevation->getLayer(1);
			layer->addParam<bool>("BLOCKING", false);
			map->addElevation(elevation);
		}
	}

	void Fallout::ignoreScripts(RawDataPtr data) {
		Log("mlfallout") << "ignoring scripts...";
		for (unsigned int type = 0; type < 5; ++type) {
			unsigned int script_count = data->read32Big();
			Log("mlfallout") 
				<< "script type " << type 
				<< " ignoring " << script_count << " scripts";
			if (script_count > 0) {
				/* loop counter must be modulo 16 (rounded up) */
				unsigned int loop = script_count;
				if (loop % 16) {
					loop = script_count + (16 - script_count % 16);
				}
				unsigned int checkcount = 0;
				for (unsigned int i = 0; i < loop; ++i) {
					uint32_t pid = data->read32Big();
					uint8_t current_type = (pid >> 24) & 0xff;

					unsigned int move = 15;
					if (current_type == 1) {
						move += 2;
					} else if (current_type == 2) {
						move += 1;
					}

					if (type == current_type) {
						++checkcount;
					}

					data->moveIndex(move * 4);
					if ((i % 16) == 15) {
						uint32_t check = data->read32Big();
						data->moveIndex(4);

						if (checkcount < check) {
							Log("mffallout") 
								<< "CheckCount: " << checkcount << " Check: " << check;
							throw InvalidFormat("scriptcheck failed");
						}

						checkcount = 0;
					}
				}
			}
		}
	}

	void Fallout::loadObjects(MapPtr map, RawDataPtr data) {
		uint32_t count_objects_all = data->read32Big();
		Log("mlfallout") << "trying to load " << count_objects_all << " objects";

		uint32_t count_objects_check = 0;
		ObjectFactory objfactory(m_header->getVersion());

		unsigned int fife_elev = 0; 
		// we always start with elevation 0 - some fallout maps don't do this, 
		// so we need to remap them here
		for (unsigned int elev = 0; elev < 3; ++elev) {
			uint32_t count_objects_current = data->read32Big();
			count_objects_check += count_objects_current;

			bool haselevation = m_header->hasElevation(elev);
			if (!haselevation) {
				if (count_objects_current != 0)
					throw InvalidFormat("objects on omitted elevation");

				Log("mlfallout") << "ignoring elevation: " << elev;
				continue;
			}

			Log("mlfallout") 
				<< "Elevation " << elev  << " objects: " << count_objects_current;
			LayerPtr layer = map->getElevation(fife_elev)->getLayer(1);

			for (uint32_t i = 0; i < count_objects_current; ++i) {
				ObjectPtr obj = objfactory.createObject(data);
				if( obj ) {
					obj->setElevation( fife_elev );
					obj->setLayer( 1 );
					layer->addObject( obj );
				}
			}
			fife_elev++;
		}

		if (count_objects_all != count_objects_check) {
			Log("mlfallout") 
				<< "count_objects_all: " << count_objects_all
				<< " count_objects_check: " << count_objects_check;
			throw InvalidFormat("sanity check for object-count failed");
		}
	}
} } } }
