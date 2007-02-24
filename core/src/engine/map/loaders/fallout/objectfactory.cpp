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
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/map.h"
#include "map/objectinfo.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "settingsmanager.h"
#include "debugutils.h"
#include "exception.h"
#include "imagecache.h"

#include "critlst.h"
#include "list.h"
#include "objectfactory.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	ObjectFactory::ObjectFactory(unsigned int version) 
		: m_fallout_version(version),
		m_imageCache(ImageCache::instance()),
		m_lst_walls("art/walls/walls.lst"),
		m_lst_items("art/items/items.lst"),
		m_lst_scenery("art/scenery/scenery.lst"),
		m_lst_misc("art/misc/misc.lst"),
		m_lst_critters("art/critters/critters.lst"),
		m_default_moi()
	{
		m_default_moi.getLocation().grid = 1;
	}

	uint32_t ObjectFactory::getProSubType(const std::string& path) const {
		RawDataPtr file = VFS::instance()->open(path);
		file->setIndex(0x20);
		return file->read32Big();
	}

	ObjectInfo* ObjectFactory::createObject(RawDataPtr data) {
		s_objectinfo info;

		data->moveIndex(4); // serpartor or something like that..
		info.position = data->read32Big();
		data->moveIndex(4*4); // unknown or currently unused fields

		info.frame = data->read32Big();
		info.orientation = data->read32Big();
		info.frm_pid = data->read32Big();
		data->moveIndex(4*2);
		info.proto_pid = data->read32Big();
		data->moveIndex(4*6);

		uint32_t inventory_count = data->read32Big();
		data->moveIndex(3*4);

		ObjectInfo* obj = loadObject(data, info);

		while (inventory_count--) {
			data->moveIndex(4);

			ObjectInfo* iobj = createObject(data);
			if (obj) {
				if (iobj) {
					obj->addToInventory( iobj );
				}
			} else {
				delete iobj;
			}
		}

		if (obj) {
			obj->getLocation().position = Point( info.position % 200, info.position / 200 );
		}

		return obj;
	}

	ObjectInfo* ObjectFactory::loadObject(RawDataPtr data, const s_objectinfo& info) {
		uint8_t type = (info.proto_pid >> 24) & 0xff;

		ObjectInfo* obj = new ObjectInfo(m_default_moi);
		switch(type) {
			case objtype_item:
				loadItem(data, info, obj);
				break;

			case objtype_critter:
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Critter");
				loadCritter(data, info, obj);
				loadCritterAnimation("critters", m_lst_critters, info, obj);
				break;

			case objtype_scenery:
				loadScenery(data, info, obj);
				obj->setStatic(true);
				break;

			case objtype_wall: {
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Wall");
				obj->setStatic(true);
				obj->setZValue( 1 );
				loadImages("walls", m_lst_walls, info, obj);
			}
			break;

			case objtype_tile: {
				data->moveIndex(4);
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Tile");
				obj->setStatic(true);
				obj->setZValue( 0 );
				loadImages("walls", m_lst_walls, info, obj);
			}
			break;

			case objtype_misc: {
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Misc");
				uint16_t id = info.proto_pid & 0xffff;
				
				if (id != 1 && id != 12)
					data->moveIndex(4*4);

				//obj = new Object("Misc", info.position);
				loadImages("misc", m_lst_misc, info, obj);
				Debug("fallout::ObjectFactory")
					<< "Misc Item: " 
					<< obj->get<std::string>(ObjectInfo::ObjectTypeParam)
					<< " Visual: " << obj->getVisualLocation().toString()
					<< " Proto-PID: " << id;
			}
			break;

			case objtype_interface:
			case objtype_inventory:
			case objtype_head:
			case objtype_background:
				delete obj;
				return 0;

				break;
			default:
				throw InvalidFormat("unknown objecttype");
				break;
		}

		return obj;
	}

	void ObjectFactory::loadItem(RawDataPtr data, const s_objectinfo& info, ObjectInfo* obj) {
		uint16_t id = info.proto_pid & 0xffff;
		static list itemlst("proto/items/items.lst");
		uint32_t subtype = getProSubType("proto/items/" + itemlst.getProFile(id - 1));

		switch(subtype) {
			case itemtype_armor:
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Armor");
				break;

			case itemtype_container:
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Container");
				break;

			case itemtype_drug:
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Item");
				break;

			case itemtype_weapon: {
				data->moveIndex(8);
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Drug");
			}
			break;

			case itemtype_ammo: {
				data->moveIndex(4);
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Ammo");
			}
			break;

			case itemtype_misc: {
				data->moveIndex(4);
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Misc");
			}
			break;

			case itemtype_key: {
				data->moveIndex(4);
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Key");
			}
			break;

			default:
				throw InvalidFormat("unknown itemtype");
				break;
		}

#if 0
		if (obj) {
			loadImages("items", m_lst_items, info, obj);
		}
#endif
	}

	void ObjectFactory::loadCritter(RawDataPtr data, const s_objectinfo& info, ObjectInfo* moi) {
		// The rest is unknown!
		data->moveIndex(4*4);
		size_t ai = data->read32Big();
		size_t groupId = data->read32Big();
		data->moveIndex(4);
		size_t hitpoints = data->read32Big();
		data->moveIndex(4*2);

		moi->set<size_t>("ai",ai);
		moi->set<size_t>("group",groupId);
		moi->set<size_t>("hitpoints",hitpoints);
	}

	void ObjectFactory::loadScenery(RawDataPtr data, const s_objectinfo& info, ObjectInfo* obj) {
		uint16_t id = info.proto_pid & 0xffff;
		static list itemlst("proto/scenery/scenery.lst");
		uint32_t subtype = getProSubType("proto/scenery/" + itemlst.getProFile(id - 1));

		switch (subtype) {
			case scenerytype_portal: {
				data->moveIndex(4);
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Portal");
			}
			break;

			case scenerytype_stairs:
			case scenerytype_elevator: {
				data->moveIndex(2*4);
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Elevator");
			}
			break;

			case scenerytype_ladderup:
			case scenerytype_ladderdown: {
				if (m_fallout_version == 20) // fallout2
					data->moveIndex(2*4);
				else
					data->moveIndex(4);
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Ladder");
			}
			break;

			case scenerytype_genereic:
				obj->set<std::string>(ObjectInfo::ObjectTypeParam,"Scenery");
				correctSceneryZValue(info.frm_pid, obj);
				break;

			default:
				throw InvalidFormat("unknown scenerytype");
				break;
		}

		if (obj) {
			loadImages("scenery", m_lst_scenery, info, obj);
		}
	}

	void ObjectFactory::loadImages(const std::string& type, list& lst, const s_objectinfo& info, ObjectInfo* obj) {
		std::string path = "art/" + type +"/" + lst.getProFile((info.frm_pid & 0xffff));

		obj->setVisualLocation( RenderableLocation(RenderAble::RT_IMAGE, path) );
	}

	void ObjectFactory::loadCritterAnimation(const std::string& type, CritLST& lst,
			const s_objectinfo& info, ObjectInfo* obj) {
		std::string path;
		bool useXMLCritter = SettingsManager::instance()->read<bool>("UseXMLCritters",false);
		uint32_t idx =  info.frm_pid & 0x00000fff;
		uint32_t id1 = (info.frm_pid & 0x0000f000) >> 12;
		uint32_t id2 = (info.frm_pid & 0x00ff0000) >> 16;
		uint32_t id3 = (info.frm_pid & 0x70000000) >> 28;

		// Not sure about "re-indexing" ... is it necessary
		// for XML Critters?
		if (id2 == 0x1b || id2 == 0x1d ||
				id2 == 0x1e || id2 == 0x37 ||
				id2 == 0x39 || id2 == 0x3a ||
				id2 == 0x21 || id2 == 0x40) {
			Log("DEBUG") << "switching critter id from " << idx;
			idx = lst.getReIndex(idx);
			Log("DEBUG") << "new critter id " << idx;
		}

		// New XML Critters can be toggled through
		// a config value.
		if (!useXMLCritter) {
	
			path = "art/" + type +"/" + lst.getProFile(idx);
			//DEBUG_PRINT("Critter: idx: " << idx << " path:" << path);
	
			char tmpBuf[3] = {0, 0, 0};
			if (id1 >= 0x0b) {
				Log("mffalloutanimationfactory") << "ARGH";
				return;
			}
			if (id2 >= 0x26 && id2 <= 0x2f) {
				tmpBuf[0] = char(id1) + 'c';
				tmpBuf[1] = char(id2) + 0x3d;
				path.append(tmpBuf);
			}
			else if (id2 == 0x24) {
				path.append("ch");
			}
			else if (id2 == 0x25) {
				path.append("cj");
			}
			else if (id2 >= 0x30) {
				tmpBuf[0] = 'r';
				tmpBuf[1] = char(id2) + 0x31;
				path.append(tmpBuf);
			}
			else if (id2 >= 0x14) {
				tmpBuf[0] = 'b';
				tmpBuf[1] = char(id2) + 0x4d;
				path.append(tmpBuf);
			}
			else if (id2 == 0x12) {
				if (id1 == 0x01) {
					path.append("dm");
				}
				else if (id1 == 0x04) {
					path.append("gm");
				}
				else {
					path.append("as");
				}
			}
			else if (id2 == 0x0d) {
				if (id1 > 0) {
					tmpBuf[0] = char(id1) + 'c';
					tmpBuf[1] = 'e';
					path.append(tmpBuf);
				}
				else {
					path.append("an");
				}
			}
			else {
				if (id2 <= 1 && id1 > 0) {
					tmpBuf[0] = char(id1) + 'c';
				}
				else {
					tmpBuf[0] = 'a';
				}
				tmpBuf[1] = char(id2) + 'a';
				path.append(tmpBuf);
			}
	
			path.append(".fr");
			if (!id3)
				path.append("m");
			else
				path.append(boost::lexical_cast<std::string>(id3 - 1));
		} else {
			// XML Critter path.
			path = SettingsManager::instance()->read<std::string>("XMLCrittersBaseDirectory","content/gfx/critters/");
			path += lst.getProFile(idx);
			path += ".xml";
		}

		obj->setVisualLocation( RenderableLocation(RenderAble::RT_COMPLEX_ANIMATION, path) );
		obj->set<size_t>(obj->OrientationParam,info.orientation);
	}

	void ObjectFactory::correctSceneryZValue(uint32_t frm_pid,ObjectInfo* moi) {
		std::string fname = m_lst_scenery.getProFile(frm_pid & 0xffff);
		
		if( fname == "slimpol1.frm" || fname == "slimpol2.frm" 
		    || fname == "slimpol3.frm"
		    || fname == "goop01.frm" || fname == "goop02.frm"
		    || fname == "GOOP03.FRM" || fname == "GOOP04.FRM"
		  ) {
			moi->setZValue(0);
			moi->set<std::string>(ObjectInfo::ObjectTypeParam,"Tile");
		}
		
	}


} } } }//FIFE::map::loaders::fallout
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
