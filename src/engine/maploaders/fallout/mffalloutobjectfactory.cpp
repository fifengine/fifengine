/***************************************************************************
 *   Copyright (C) 2005 by the FIFE Team                                   *
 *   fife@developer1.cvsdude.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "mffalloutobjectfactory.h"
#include "object.h"
#include "objectfactory.h"
#include "rawdata.h"
#include "exception.h"
#include "mffalloutlst.h"
#include "mffalloutcritlst.h"
#include "debugutils.h"
#include "mffalloutfrm.h"
#include "imagecache.h"
#include "rawdata.h"
#include "vfs.h"
#include "animation.h"
#include "palutil.h"

#include <boost/lexical_cast.hpp>

namespace FIFE {

	MFFalloutObjectFactory::MFFalloutObjectFactory(unsigned int version) : m_fallout_version(version),
																																				 m_imgids(), 
																																				 m_lst_walls(new MFFalloutLST("art/walls/walls.lst")),
																																				 m_lst_items(new MFFalloutLST("art/items/items.lst")),
																																				 m_lst_scenery(new MFFalloutLST("art/scenery/scenery.lst")),
																																				 m_lst_misc(new MFFalloutLST("art/misc/misc.lst")),
																																				 m_lst_critters(new MFFalloutCritLST("art/critters/critters.lst"))
	{
	}

	MFFalloutObjectFactory::MFFalloutObjectFactory(const MFFalloutObjectFactory&) : m_imgids(), m_lst_walls(0), m_lst_items(0),
																																									m_lst_scenery(0), m_lst_misc(0), m_lst_critters(0) {}

	MFFalloutObjectFactory::~MFFalloutObjectFactory() {
		delete m_lst_misc;
		delete m_lst_scenery;
		delete m_lst_items;
		delete m_lst_walls;
		delete m_lst_critters;
		cleanup();
	}

	MFFalloutObjectFactory& MFFalloutObjectFactory::operator=(const MFFalloutObjectFactory&) {
		return *this;
	}

	void MFFalloutObjectFactory::cleanup() {
		type_imgids::const_iterator end = m_imgids.end();
		for (type_imgids::iterator i = m_imgids.begin(); i != end; ++i)
			delete i->second.palanim;
		
		m_imgids.clear();
	}

	uint32_t MFFalloutObjectFactory::getProSubType(const std::string& path) const {
		RawDataPtr file = VFS::instance()->open(path);
		file->setIndex(0x20);
		return file->read32Big();
	}

	Object* MFFalloutObjectFactory::createObject(RawDataPtr data) {
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

		Object* obj = loadObject(data, info);

		while (inventory_count--) {
			data->moveIndex(4);
			Object* iobj = createObject(data);
			if (obj) obj->addToInventory(iobj);
			else delete iobj;
		}

		if (obj) {
			obj->setPosition(info.position);
		}

		return obj;
	}

	Object* MFFalloutObjectFactory::loadObject(RawDataPtr data, const s_objectinfo& info) {
		uint8_t type = (info.proto_pid >> 24) & 0xff;

		Object* obj = 0;
		switch(type) {
			case objtype_item:
				obj = loadItem(data, info);
				break;

			case objtype_critter:
				obj = loadCritter(data, info);
				//loadCritterImages(obj, "critters", m_lst_critters, info);
				loadCritterAnimation(obj, "critters", m_lst_critters, info);
				break;

			case objtype_scenery:
				obj = loadScenery(data, info);
				break;

			case objtype_wall: {
				obj = new Object("Wall", info.position);
				loadImages(obj, "walls", m_lst_walls, info);
			}
			break;

			case objtype_tile: {
				data->moveIndex(4);
				obj = new Object("Tile", info.position);
				loadImages(obj, "walls", m_lst_walls, info);
			}
			break;

			case objtype_misc: {
				uint16_t id = info.proto_pid & 0xffff;
				if (id != 1 && id != 12)
					data->moveIndex(4*4);

				obj = new Object("Misc", info.position);
				loadImages(obj, "misc", m_lst_misc, info);
			}
			break;

			case objtype_interface:
			case objtype_inventory:
			case objtype_head:
			case objtype_background:
				break;
			default:
				throw InvalidFormat("unknown objecttype");
				break;
		}

		return obj;
	}

	Object* MFFalloutObjectFactory::loadItem(RawDataPtr data, const s_objectinfo& info) {
		uint16_t id = info.proto_pid & 0xffff;
		static MFFalloutLST itemlst("proto/items/items.lst");
		uint32_t subtype = getProSubType("proto/items/" + itemlst.getProFile(id - 1));

		Object* obj = 0;
		switch(subtype) {
			case itemtype_armor:
				obj = new Object("Armor", info.position);
				break;

			case itemtype_container:
				obj = new Object("Container", info.position);
				break;

			case itemtype_drug:
				obj = new Object("Armor", info.position);
				break;

			case itemtype_weapon: {
				data->moveIndex(8);
				obj = new Object("Weapon", info.position);
			}
			break;

			case itemtype_ammo: {
				data->moveIndex(4);
				obj = new Object("Ammo", info.position);
			}
			break;

			case itemtype_misc: {
				data->moveIndex(4);
				obj = new Object("MiscItem", info.position);
			}
			break;

			case itemtype_key: {
				data->moveIndex(4);
				obj = new Object("Key", info.position);
			}
			break;

			default:
				throw InvalidFormat("unknown itemtype");
				break;
		}

		if (obj)
			loadImages(obj, "items", m_lst_items, info);

		return obj;
	}

	Object* MFFalloutObjectFactory::loadCritter(RawDataPtr data, const s_objectinfo& info) {
		/*
			 static MFFalloutLST critterlst("art/critters/critters.lst");
			 const std::string framefile = "art/critters/" + critterlst.getFirstString() + "aa" + ".frm";
			 obj = new Object("Critter", info.position);
			 if (obj)
			 loadImages(obj, "critters", m_list_critters, info);

			 return obj;*/
		data->moveIndex(40);
		Object* result = new Object("Critter", info.position);
		return result;
	}

	Object* MFFalloutObjectFactory::loadScenery(RawDataPtr data, const s_objectinfo& info) {
		uint16_t id = info.proto_pid & 0xffff;
		static MFFalloutLST itemlst("proto/scenery/scenery.lst");
		uint32_t subtype = getProSubType("proto/scenery/" + itemlst.getProFile(id - 1));

		Object* obj = 0;
		switch (subtype) {
			case scenerytype_protal: {
				data->moveIndex(4);
				obj = new Object("Portal", info.position);
			}
			break;

			case scenerytype_stairs:
			case scenerytype_elevator: {
				data->moveIndex(2*4);
				obj = new Object("Stairs", info.position);
			}
			break;

			case scenerytype_ladderup:
			case scenerytype_ladderdown: {
				if (m_fallout_version == 20) // fallout2
					data->moveIndex(2*4);
				else
					data->moveIndex(4);
				obj = new Object("Ladder", info.position);
			}
			break;

			case scenerytype_genereic:
				obj = new Object("SceneryGeneric", info.position);
				break;

			default:
				throw InvalidFormat("unknown scenerytype");
				break;
		}

		if (obj)
			loadImages(obj, "scenery", m_lst_scenery, info);

		return obj;
	}

	void MFFalloutObjectFactory::loadImages(Object* object, const std::string& type, MFFalloutLST* lst, const s_objectinfo& info) {
		std::string path = "art/" + type +"/" + lst->getProFile((info.frm_pid & 0xffff));
		std::string cacheid = path + boost::lexical_cast<std::string>(info.orientation);
		type_imgids::const_iterator i = m_imgids.find(cacheid);

		s_frminfo frminfo;
		if (i == m_imgids.end()) {
			MFFalloutFRM frm(path);
			frminfo.imgid = ImageCache::instance()->addImage(frm.getFrame(info.orientation, info.frame));
			frminfo.xoffset = frm.getShiftX(info.orientation);
			frminfo.yoffset = frm.getShiftY(info.orientation);
			frminfo.palanim = frm.getPalAnimation();
			m_imgids.insert(std::make_pair(cacheid, frminfo));
			if( frminfo.palanim )
				Log("palanim") << "Found PAL Animation in Object Image!";
		} else
			frminfo = i->second;

		if( frminfo.palanim ){
			ObjectFactory::makeStaticAnimation(object,frminfo.palanim->generateAnimation(),
				frminfo.xoffset, frminfo.yoffset);
		} else {
			ObjectFactory::makeStaticImage(object,frminfo.imgid, frminfo.xoffset, frminfo.yoffset);
		};
	}

	void MFFalloutObjectFactory::loadCritterImages(Object* object, const std::string& type, MFFalloutCritLST* lst,
			const s_objectinfo& info) {
		size_t idx = info.frm_pid & 0x00000fff;
		if (idx > 200) {
			Log("FIXME") << "Safeguarding against way too large frm_pid for critter:" << idx;
			return;
		}
		std::string path = "art/" + type +"/" + lst->getProFile(idx) + "aa.frm";
		std::string cacheid = path + boost::lexical_cast<std::string>(info.orientation);
		type_imgids::const_iterator i = m_imgids.find(cacheid);

		s_frminfo frminfo;
		if (i == m_imgids.end()) {
			Log("TEST") <<  "Loading critter id: " << idx << " assumed path: " << path;
			MFFalloutFRM frm(path);
			frminfo.imgid = ImageCache::instance()->addImage(frm.getFrame(info.orientation, info.frame));
			frminfo.xoffset = frm.getShiftX(info.orientation);
			frminfo.yoffset = frm.getShiftY(info.orientation);
			frminfo.palanim = frm.getPalAnimation();
			m_imgids.insert(std::make_pair(cacheid, frminfo));
		} else {
			Log("TEST") << "cached: id " << idx << " assumed path: " << path;
			frminfo = i->second;
		}

		ObjectFactory::makeStaticImage(object,frminfo.imgid, frminfo.xoffset, frminfo.yoffset);
	}

	Animation* MFFalloutObjectFactory::loadCritterAnimation(Object* object, const std::string& type, MFFalloutCritLST* lst,
			const s_objectinfo& info) {

		uint32_t idx =  info.frm_pid & 0x00000fff;
		uint32_t id1 = (info.frm_pid & 0x0000f000) >> 12;
		uint32_t id2 = (info.frm_pid & 0x00ff0000) >> 16;
		uint32_t id3 = (info.frm_pid & 0x70000000) >> 28;

		if (id2 == 0x1b || id2 == 0x1d ||
				id2 == 0x1e || id2 == 0x37 ||
				id2 == 0x39 || id2 == 0x3a ||
				id2 == 0x21 || id2 == 0x40) {
			Log("DEBUG") << "switching critter id from " << idx;
			idx = lst->getReIndex(idx);
			Log("DEBUG") << "new critter id " << idx;
		}

		std::string path = "art/" + type +"/" + lst->getProFile(idx);

		char tmpBuf[3] = {0, 0, 0};
		if (id1 >= 0x0b) {
			Log("mffalloutanimationfactory") << "ARGH";
			return NULL;
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

		Log("FINAL") << path;
		MFFalloutFRM frm(path);
		Animation* result = new Animation(frm.getNumFrames());
		for (uint32_t j=0; j < frm.getNumFrames() ; j++) {
			std::string cacheid = path + boost::lexical_cast<std::string>(info.orientation) + "#" +
				boost::lexical_cast<std::string>(j);
			type_imgids::const_iterator i = m_imgids.find(cacheid);
			s_frminfo frminfo;
			if (i == m_imgids.end()) {
				frminfo.imgid = ImageCache::instance()->addImage(frm.getFrame(info.orientation, j));
				frminfo.xoffset = frm.getShiftX(info.orientation, j);
				frminfo.yoffset = frm.getShiftY(info.orientation, j);
				frminfo.palanim = frm.getPalAnimation();
				m_imgids.insert(std::make_pair(cacheid, frminfo));
			} 
			else
				frminfo = i->second;
			result->setFrame(j, frminfo.imgid, frminfo.xoffset, frminfo.yoffset);
		}
		result->setCurrentFrame(info.frame);
		uint16_t frm_fps = frm.getFramesPerSecond();

		if (!frm_fps)
			frm_fps = 1;

		result->setFrameDuration(1000/frm_fps);
		ObjectFactory::makeStaticAnimation(object,result);

		return result;
	}


}//FIFE
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
