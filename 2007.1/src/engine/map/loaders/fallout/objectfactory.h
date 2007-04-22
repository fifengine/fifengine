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

#ifndef FIFE_MAP_LOADERS_FALLOUT_OBJECTFACTORY_H
#define FIFE_MAP_LOADERS_FALLOUT_OBJECTFACTORY_H

// Standard C++ library includes
#include <map>
#include <string>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"
#include "video/renderable.h"

#include "critlst.h"

namespace FIFE { 
	class ImageCache;

namespace map { 

	class ObjectInfo;
	typedef boost::shared_ptr<ObjectInfo> ObjectPtr;

namespace loaders { namespace fallout {

	class RawData;
	class list;
	class FRM;
	class Animation;
	class PALAnimation;

	/** Creates game-objects from Fallout datafiles.
	 *
	 * Many fallout-specific helper classes are used to deal with different
	 * formats:
	 * \see map::loaders::fallout::list
	 * \see map::loaders::fallout::FRM
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
	class ObjectFactory {
		public:
			/** Create a new Object Factory
			 * @param fallout_version The Fallout file format version = 20 for fallout2
			 */
			ObjectFactory(unsigned int fallout_version);

			/** Loads an Object
			 * @param data The VFS file to read from.
			 * @return A newly created Object
			 *
			 * This reads an Object from a specific index in the VFS file.
			 * Thus the RawDataPtr must have it's index at a specific
			 * position.
			 */
			ObjectPtr createObject(RawDataPtr data);

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

			unsigned int m_fallout_version;

			ImageCache* m_imageCache;

			list m_lst_walls;
			list m_lst_items;
			list m_lst_scenery;
			list m_lst_misc;
			CritLST m_lst_critters;

			/** Dispatches object loading to more specific functions.
			 */
			ObjectPtr loadObject(RawDataPtr data, const s_objectinfo& info);

			/** Load an Item object
			 */
			void loadItem(RawDataPtr data, const s_objectinfo& info, ObjectPtr);

			/** Load a Critter object
			 */
			void loadCritter(RawDataPtr data, const s_objectinfo& info, ObjectPtr);
	
			/** Load a scenery object
			 */
			void loadScenery(RawDataPtr data, const s_objectinfo& info, ObjectPtr);

			/** Correct Z value for tile-like scenery objects
			 *  @bug Hardcoded list of frms to correct.
			 */
			void correctSceneryZValue(uint32_t frm_pid,ObjectPtr moi);

			void loadImages(const std::string& type, list& lst, const s_objectinfo& info, ObjectPtr);
			void loadCritterAnimation(const std::string& type, CritLST& lst, const s_objectinfo& info, ObjectPtr);


			uint32_t getProSubType(const std::string& path) const;

			// Not copyable.
			ObjectFactory(const ObjectFactory&);
			ObjectFactory& operator=(const ObjectFactory&);
	};

} } } } //FIFE

#endif

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
