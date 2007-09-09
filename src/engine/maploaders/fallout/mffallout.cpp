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

#include "mffallout.h"
#include "mffalloutlst.h"
#include "exception.h"
#include "mffalloutheader.h"
#include "map.h"
#include "maptile.h"
#include "mapelevation.h"
#include "maptile.h"
//#include "mffalloutfrm.h"
#include "imagecache.h"
#include "mffalloutobjectfactory.h"
#include "debugutils.h"
#include "rawdata.h"
#include "mffalloutdat2.h"
#include "settingsmanager.h"
#include "mffalloutproviderdat2.h"
#include "mffalloutproviderdat1.h"
#include "vfssourcefactory.h"
//#include "palutil.h"
#include "object.h"

#include <map>
#include <algorithm>

#include "maptilegrid.h"
#include "mapgeometry.h"
#include "mapstructure.h"

namespace FIFE {


	MFFallout::MFFallout() : MapLoader("Fallout"), m_header(0), m_cached() {
		VFSSourceFactory* vfsfactory = VFSSourceFactory::instance();
		vfsfactory->addProvider(new MFFalloutProviderDAT2());
		vfsfactory->addProvider(new MFFalloutProviderDAT1());

		VFS* vfs = VFS::instance();
		VFSSource* source = vfsfactory->createSource("master.dat");
		if (source)
			vfs->addSource(source);

		source = vfsfactory->createSource("critter.dat");
		if (source)
			vfs->addSource(source);

		//// Init Fallout Map Structure ////
		m_foElevationFormat = MapStructure::falloutElevationFormat(Point(100,100));
	}

	MFFallout::~MFFallout() {
		cleanup();
	}

	MFFallout::MFFallout(const MFFallout&) : MapLoader("Fallout"), m_header(0), m_cached() {}

	MFFallout& MFFallout::operator=(const MFFallout&) {
		return *this;
	}

	void MFFallout::cleanup() {
		delete m_header;
		m_header = 0;

		m_cached.clear();
	}

	const MFFallout::s_imgdata& MFFallout::getImageID(const std::string& path) {
		type_cached::const_iterator i = m_cached.find(path);

		if (i == m_cached.end()) {
			s_imgdata imgdata;
			ImageCache* imgcache = ImageCache::instance();
			imgdata.cacheid = imgcache->addImageFromFile(path);
			imgdata.pal_animation = 0;/*frm.getPalAnimation();*/

			i = m_cached.insert(std::make_pair(path, imgdata)).first;
		}
		return i->second;
	}

	Map* MFFallout::loadFile(const std::string& filepath) {
		cleanup();
		RawDataPtr file = VFS::instance()->open(filepath);

		m_header = new MFFalloutHeader(file);
		file->moveIndex(m_header->getNumGlobalVars()*4 + m_header->getNumLocalVars()*4);

		Map* map = new Map(m_header->getMapName());
		loadTiles(map, file);
		ignoreScripts(file);
		loadObjects(map, file);

		return map;
	}

	void MFFallout::loadTiles(Map* map, RawDataPtr data) {
		MFFalloutLST  lst("art/tiles/tiles.lst");
		for (unsigned int e = 0; e < m_header->getNumElevations(); ++e) {
			MapElevation* elevation = new MapElevation( m_foElevationFormat );
			for (unsigned int y = 0; y < 100; ++y) {
				for (unsigned int x = 0; x < 100; ++x) {
					unsigned int roof = data->read16Big();
					unsigned int floor = data->read16Big();

					// this one is strange - are fallout maps realy right-to left or am i just confused? ;)
					Point tilepos(99-x,y);
// 					MapTile* tile = elevation->getTile(99-x, y);
					MapTileGrid* floorgrid  = elevation->getTileGrid(0);
					MapTileGrid* roofgrid  = elevation->getTileGrid(1);
					if (roof > 1) {
						//const std::string& roof_file = "art/tiles/" + lst.getProFile(roof);
						std::string roof_file = "art/tiles/" + lst.getProFile(roof);
						const s_imgdata& imgdata = getImageID(roof_file);
// 						tile->setRoofImageID(imgdata.cacheid);
						roofgrid->setImageId( tilepos, imgdata.cacheid );
						//tile->setRoofShift(5, 5);
					}

					if (floor > 1) {
						std::string floor_file = "art/tiles/" + lst.getProFile(floor);
						const s_imgdata& imgdata = getImageID(floor_file);
// 						tile->setFloorImageID(imgdata.cacheid);
						floorgrid->setImageId( tilepos, imgdata.cacheid );
					}

				}
			}
			map->addElevation(elevation);
		}
	}

	void MFFallout::ignoreScripts(RawDataPtr data) {
		Log("mlfallout") << "ignoreing scripts...";
		for (unsigned int type = 0; type < 5; ++type) {
			unsigned int script_count = data->read32Big();
			Log("mlfallout") << "script type " << type << " ignoreing " << script_count << " scripts";
			if (script_count > 0) {
				/* loop counter must be modulo 16 (rounded up) */
				unsigned int loop = script_count;
				if (loop % 16)
					loop = script_count + (16 - script_count % 16);

				unsigned int checkcount = 0;
				for (unsigned int i = 0; i < loop; ++i) {
					uint32_t pid = data->read32Big();
					uint8_t current_type = (pid >> 24) & 0xff;

					unsigned int move = 15;
					if (current_type == 1)
						move += 2;
					else if (current_type == 2)
						move += 1;

					if (type == current_type)
						++checkcount;

					data->moveIndex(move*4);
					if ((i % 16) == 15) {
						uint32_t check = data->read32Big();
						data->moveIndex(4);

						if (checkcount < check) {
							Log("mffallout") << "CheckCount: " << checkcount << " Check: " << check;
							throw InvalidFormat("scriptcheck failed");
						}

						checkcount = 0;
					}
				}
			}
		}
	}

	void MFFallout::loadObjects(Map* map, RawDataPtr data) {
		uint32_t count_objects_all = data->read32Big();
		Log("mlfallout") << "trying to load " << count_objects_all << " objects";

		uint32_t count_objects_check = 0;
		MFFalloutObjectFactory objfactory(m_header->getVersion());

		unsigned int fife_elev = 0; // we always start with elevation 0 - some fallout maps don't do this, so we need to remap them here
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

			Log("mlfallout") << "Elevation " << elev  << " objects: " << count_objects_current;
			MapElevation* elevation = map->getElevation(fife_elev++);
			for (uint32_t i = 0; i < count_objects_current; ++i) {
				Object* obj = objfactory.createObject(data);
				elevation->addObject(obj);
			}

		}

		if (count_objects_all != count_objects_check) {
			Log("mlfallout") << "count_objects_all: " << count_objects_all << " count_objects_check: " << count_objects_check;
			throw InvalidFormat("sanity check for object-count failed");
		}
	}



}//FIFE
