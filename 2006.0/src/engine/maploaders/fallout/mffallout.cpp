#include "mffallout.h"
#include "mffalloutlst.h"
#include "exception.h"
#include "mffalloutheader.h"
#include "map.h"
#include "maptile.h"
#include "mapelevation.h"
#include "maptile.h"
#include "mffalloutfrm.h"
#include "imagecache.h"
#include "mffalloutobjectfactory.h"
#include "debugutils.h"
#include "rawdata.h"
#include "mffalloutdat2.h"
#include "settingsmanager.h"
#include "mffalloutproviderdat2.h"
#include "mffalloutproviderdat1.h"
#include "vfssourcefactory.h"
#include "palutil.h"
#include "object.h"
#include "objectfactory.h"

#include <map>
#include <algorithm>

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
			ImageCache* imgcache = ImageCache::instance();
			MFFalloutFRM frm(path);

			s_imgdata imgdata;
			imgdata.cacheid = imgcache->addImage(frm.getFrame());
			imgdata.shift_x = frm.getShiftX(0);
			imgdata.shift_y = frm.getShiftY(0);
			imgdata.pal_animation = frm.getPalAnimation();

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
		generatePalAnimations(map);

		return map;
	}

	void MFFallout::loadTiles(Map* map, RawDataPtr data) {
		MFFalloutLST  lst("art/tiles/tiles.lst");
		for (unsigned int e = 0; e < m_header->getNumElevations(); ++e) {
			MapElevation* elevation = new MapElevation(100, 100);
			for (unsigned int y = 0; y < 100; ++y) {
				for (unsigned int x = 0; x < 100; ++x) {
					unsigned int roof = data->read16Big();
					unsigned int floor = data->read16Big();

					// this one is strange - are fallout maps realy right-to left or am i just confused? ;)
					MapTile* tile = elevation->getTile(99-x, y);

					if (roof > 1) {
						//const std::string& roof_file = "art/tiles/" + lst.getProFile(roof);
						std::string roof_file = "art/tiles/" + lst.getProFile(roof);
						const s_imgdata& imgdata = getImageID(roof_file);
						tile->setRoofImageID(imgdata.cacheid);
						//tile->setRoofShift(5, 5);
					}

					if (floor > 1) {
						std::string floor_file = "art/tiles/" + lst.getProFile(floor);
						const s_imgdata& imgdata = getImageID(floor_file);
						tile->setFloorImageID(imgdata.cacheid);
					}

				}
			}
			Log("DEBUG") << "Adding Elevation: " << e << " / " << m_header->getNumElevations();
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

	void MFFallout::generatePalAnimations(Map* map) {
		s_imgdata img;
		std::map<size_t,PALAnimation*> imgIdMap;

		type_cached::iterator i = m_cached.begin();
		for(; i != m_cached.end() ; ++i) {
			img = i->second;
			if( img.pal_animation ) {
				//Log("mlfallout") << "Found PAL Animation: " << img.pal_animation->getName()
				//		 << " in " << img.cacheid;
				imgIdMap[img.cacheid] = img.pal_animation;
			};
		};
		for (unsigned int elev = 0; elev < m_header->getNumElevations(); ++elev) {
			MapElevation* elevation = map->getElevation(elev);
			for (unsigned int y = 0; y < 100; ++y) {
				for (unsigned int x = 0; x < 100; ++x) {
					MapTile* tile = elevation->getTile(x, y);
					size_t imgId = tile->getFloorImageID();
					if( imgIdMap.find(imgId) == imgIdMap.end()  )
						continue;

					PALAnimation* palanim = imgIdMap[imgId];
					//Log("mlfallout") << "Adding PAL Animation Object " << palanim->getName()
					//	<< " at " << x << ", " << y;
					Animation* anim = palanim->generateAnimation();
					if( anim == 0 ) {
						continue;
					};
					Object* object = ObjectFactory::createTileOverlay(palanim->getName(),anim );
					object->setPosition( x + y * 100 );
					elevation->addObjectFront( object );
				};
			};
		};

		std::map<size_t,PALAnimation*>::iterator j = imgIdMap.begin();
		for(; j!=imgIdMap.end(); ++j )
			delete j->second;
	};

}//FIFE
