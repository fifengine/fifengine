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
#ifndef MFFALLOUTOBJECTFACTORYY_H
#define MFFALLOUTOBJECTFACTORYY_H

#include <stdint.h>
#include <string>
#include <map>
#include "vfs.h"

namespace FIFE {

	class Object;
	class RawData;
	class MFFalloutLST;
	class MFFalloutCritLST;
	class MFFalloutFRM;
	class Animation;
	class PALAnimation;

	class MFFalloutObjectFactory {
		public:
			MFFalloutObjectFactory(unsigned int fallout_version);
			~MFFalloutObjectFactory();

			Object* createObject(RawDataPtr data);

		private:

			enum e_object_types
			{
				objtype_item,
				objtype_critter,
				objtype_scenery,
				objtype_wall,
				objtype_tile,
				objtype_misc,
				objtype_interface,
				objtype_inventory,
				objtype_head,
				objtype_background
			};

			enum e_item_types
			{
				itemtype_armor,
				itemtype_container,
				itemtype_drug,
				itemtype_weapon,
				itemtype_ammo,
				itemtype_misc,
				itemtype_key
			};

			enum e_scenery_types
			{
				scenerytype_protal,
				scenerytype_stairs,
				scenerytype_elevator,
				scenerytype_ladderup,
				scenerytype_ladderdown,
				scenerytype_genereic
			};

			struct s_objectinfo {
				int32_t position;
				uint32_t frame;
				uint32_t orientation;
				uint32_t frm_pid;
				uint32_t proto_pid;
			};

			struct s_frminfo {
				size_t imgid;
				int16_t xoffset;
				int16_t yoffset;
				PALAnimation* palanim;
			};

			unsigned int m_fallout_version;

			typedef std::map<std::string, s_frminfo> type_imgids;
			type_imgids m_imgids;

			MFFalloutLST* m_lst_walls;
			MFFalloutLST* m_lst_items;
			MFFalloutLST* m_lst_scenery;
			MFFalloutLST* m_lst_misc;
			MFFalloutCritLST *m_lst_critters;


			Object* loadObject(RawDataPtr data, const s_objectinfo& info);
			Object* loadItem(RawDataPtr data, const s_objectinfo& info);
			Object* loadCritter(RawDataPtr data, const s_objectinfo& info);
			Object* loadScenery(RawDataPtr data, const s_objectinfo& info);

			void loadImages(Object* object, const std::string& type, MFFalloutLST* lst, const s_objectinfo& info);
			void loadCritterImages(Object* object, const std::string& type, MFFalloutCritLST* lst, const s_objectinfo& info);
			Animation* loadCritterAnimation(Object* object, const std::string& type, MFFalloutCritLST* lst,
					const s_objectinfo& info);


			uint32_t getProSubType(const std::string& path) const;

			void cleanup();
			MFFalloutObjectFactory(const MFFalloutObjectFactory&);
			MFFalloutObjectFactory& operator=(const MFFalloutObjectFactory&);
	};

}//FIFE

#endif

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
