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

#ifndef MFFALLOUTOBJECTFACTORYY_H
#define MFFALLOUTOBJECTFACTORYY_H

#include <stdint.h>
#include <string>
#include <map>
#include "vfs.h"
#include "renderable.h"

namespace FIFE {

	class Object;
	class RawData;
	class MFFalloutLST;
	class MFFalloutCritLST;
	class MFFalloutFRM;
	class Animation;
	class PALAnimation;
	class ImageCache;

	/** Creates game-objects from Fallout datafiles.
	 *
	 * Many fallout-specific helper classes are used to deal with different
	 * formats:
	 * \see MFFalloutLST
	 * \see MFFalloutFRM
	 * \see RawData
	 *
	 * \bug The fallout-specific game-Object needs access to some of the 
	 * variables/structs that are currently declared private within.
	 * 
	 * \bug This class can (really) only be used from within the
	 * fallout-maploader; creating new objects should be possible in-game
	 * (for example from the scriptengine).
	 * \bug The m_lst_* member variables are not used! Change that or remove them.
	 * \bug Remove unused functions.
	 */
	class MFFalloutObjectFactory {
		public:
			/** Create a new Object Factory
			 * @param fallout_version The Fallout file format version = 20 for fallout2
			 */
			MFFalloutObjectFactory(unsigned int fallout_version);

			/** Destructor
			 */
			~MFFalloutObjectFactory();

			/** Loads an Object
			 * @param data The VFS file to read from.
			 * @return A newly created Object
			 *
			 * This reads an Object from a specific index in the VFS file.
			 * Thus the RawDataPtr must have it's index at a specific
			 * position.
			 */
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
				scenerytype_portal,
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

// 			struct s_frminfo {
// 				size_t imgid;
// 				int16_t xoffset;
// 				int16_t yoffset;
// 				PALAnimation* palanim;
// 			};

			unsigned int m_fallout_version;

			ImageCache* m_imageCache;

			MFFalloutLST* m_lst_walls;
			MFFalloutLST* m_lst_items;
			MFFalloutLST* m_lst_scenery;
			MFFalloutLST* m_lst_misc;
			MFFalloutCritLST *m_lst_critters;


			/** Dispatches object loading to more specific functions.
			 */
			Object* loadObject(RawDataPtr data, const s_objectinfo& info);

			/** Load an Item object
			 */
			Object* loadItem(RawDataPtr data, const s_objectinfo& info);

			/** Load a Critter object
			 */
			Object* loadCritter(RawDataPtr data, const s_objectinfo& info);
	
			/** Load a scenery object
			 */
			Object* loadScenery(RawDataPtr data, const s_objectinfo& info);

			void loadImages(Object* object, const std::string& type, MFFalloutLST* lst, const s_objectinfo& info, RenderAble::RenderableTypes rt_type);
			void loadCritterImages(Object* object, const std::string& type, MFFalloutCritLST* lst, const s_objectinfo& info);
			void loadCritterAnimation(Object* object, const std::string& type, MFFalloutCritLST* lst,
					const s_objectinfo& info);


			uint32_t getProSubType(const std::string& path) const;

			void cleanup();
			MFFalloutObjectFactory(const MFFalloutObjectFactory&);
			MFFalloutObjectFactory& operator=(const MFFalloutObjectFactory&);
	};

}//FIFE

#endif

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
