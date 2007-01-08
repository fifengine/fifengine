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

#ifndef FIFE_MAP_OBJECTINFO_H
#define FIFE_MAP_OBJECTINFO_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "attributedclass.h"

#include "location.h"

namespace FIFE { namespace map {

	/** Game Object Representation
	 *
	 *  @warning Marked for a redesign.
	 */
	class ObjectInfo : public AttributedClass {
		public:
			static const std::string VisualParam;
			static const std::string NameParam;
			static const std::string ObjectTypeParam;
			static const std::string LocationParam;
			static const std::string TypeParam;
			static const std::string OrientationParam;

			ObjectInfo();
			~ObjectInfo();



			uint16_t getZValue() const { return m_zvalue; }
			void setZValue(uint16_t z) { m_zvalue = z; }

			bool isStatic() const { return m_isStatic; }
			void setStatic(bool s) { m_isStatic = s; }

			Location& getLocation() { return m_location; }
			void setLocation(const Location& loc) { m_location = loc; }

			void setVisualId(size_t visualId) { m_visualId = visualId; }
			size_t getVisualId() const { return m_visualId; }

			void addToInventory(ObjectInfo* obj) { m_temporaryInventory.push_back(obj); }
			void addToInventory(size_t objId) { m_inventory.push_back(objId); }


			typedef std::map<std::string, std::string> type_params;
			type_params params;

			void debugPrint();
		private:
			Location m_location;

			/// Indices into MapObjectManager
			std::vector<size_t> m_inventory;

			// XXX pointer ownership??? X_X
			typedef std::vector<ObjectInfo*> type_temporaryInventory;
			type_temporaryInventory m_temporaryInventory;


			size_t m_visualId;
			bool   m_isStatic;

			uint16_t m_zvalue;

			friend class ObjectManager;
	};
} }

#endif // FIFE_MAPOBJECT_INFO_H
