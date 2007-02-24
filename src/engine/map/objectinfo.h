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
#include "video/renderable_location.h"

namespace FIFE { namespace map {

	/** Game Object Representation
	 *
	 *  @bug Missing documentation
	 */
	class ObjectInfo : public AttributedClass {
		public:
			typedef RenderableLocation type_visual_location;

			static const std::string NameParam;
			static const std::string ObjectTypeParam;
			static const std::string LocationParam;
			static const std::string TypeParam;
			static const std::string OrientationParam;

			/** Constructor
			 */
			ObjectInfo();

			/** Destructor
			 */
			~ObjectInfo();

			/** Get the Z Value
			 *
			 *  The Z value controls in which order objects
			 *  on the same position are drawn.
			 *  Lower Z objects are drawn first.
			 *
			 *  The Z value 0 has a special meaning, as the
			 *  object is drawn 'tile-like', that is before
			 *  any other objects, even if they are at
			 *  another position. This is useful for slime
			 *  pools for example.
			 *
			 */
			uint16_t getZValue() const;

			/** Set the Z Value
			 */
			void setZValue(uint16_t z);

			/** Get the STATIC flag
			 *
			 *  Static objects can not be accessed via normal ruleset scripts,
			 *  they won't receive any events - they are @b static.
			 *  This should be the default for walls, scenery and the like.
			 */
			bool isStatic() const;

			/** Set the STATIC flag
			 */
			void setStatic(bool s);

			Location& getLocation();
			void setLocation(const Location& loc);

			const type_visual_location& getVisualLocation() const;
			void setVisualLocation(const type_visual_location& visual);

			void setVisualId(size_t visualId);
			size_t getVisualId() const;

			void addToInventory(ObjectInfo* obj);
			void addToInventory(size_t objId);
			const std::vector<size_t>& listInventory() const;
			void removeFromInventory(size_t objId);
			bool isInInventory(size_t objId) const;

			void loadPrototype(size_t proto_id);
			void loadPrototype(const std::string& proto_name);
			const std::vector<size_t>& listPrototypes() const;

			void debugPrint();

		private:
			Location m_location;

			/// Indices into MapObjectManager
			std::vector<size_t> m_inventory;

			/// Prototype ids
			std::vector<size_t> m_protoid;;

			// XXX pointer ownership??? X_X
			typedef std::vector<ObjectInfo*> type_temporaryInventory;
			type_temporaryInventory m_temporaryInventory;


			size_t m_visualId;
			bool   m_isStatic;

			uint16_t m_zvalue;
			type_visual_location m_visual_location;

			friend class ObjectManager;
	};

	inline
	uint16_t ObjectInfo::getZValue() const {
		return m_zvalue;
	}

	inline
	void ObjectInfo::setZValue(uint16_t z) {
		m_zvalue = z; 
	}

	inline
	bool ObjectInfo::isStatic() const {
		return m_isStatic;
	}

	inline
	void ObjectInfo::setStatic(bool s) {
		m_isStatic = s;
	}

	inline
	const ObjectInfo::type_visual_location& ObjectInfo::getVisualLocation() const {
		return m_visual_location;
	}

	inline
	void ObjectInfo::setVisualLocation(const type_visual_location& loc) {
		m_visual_location = loc;
	}

	inline
	Location& ObjectInfo::getLocation() {
		return m_location;
	}

	inline
	void ObjectInfo::setLocation(const Location& loc) {
		m_location = loc;
	}

	inline
	void ObjectInfo::setVisualId(size_t visualId) { 
		m_visualId = visualId; 
	}

	inline
	size_t ObjectInfo::getVisualId() const {
		return m_visualId; 
	}

	inline
	void ObjectInfo::addToInventory(ObjectInfo* obj) { 
		m_temporaryInventory.push_back(obj); 
	}
	
	inline
	void ObjectInfo::addToInventory(size_t objId) {
		m_inventory.push_back(objId);
	}

	inline 
	const std::vector<size_t>& ObjectInfo::listInventory() const {
		return m_inventory;
	}

	inline 
	void ObjectInfo::removeFromInventory(size_t objId) {
		m_inventory.erase(std::find(m_inventory.begin(),m_inventory.end(), objId));
	}

	inline 
	bool ObjectInfo::isInInventory(size_t objId) const{
		return std::find(m_inventory.begin(),m_inventory.end(), objId) != m_inventory.end();
	}

} }

#endif // FIFE_MAPOBJECT_INFO_H
