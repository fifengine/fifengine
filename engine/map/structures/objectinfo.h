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
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/variant.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/attributedclass.h"

#include "object_list.h"
#include "location.h"
#include "loaders/renderable_location.h"

namespace FIFE { namespace map {

	class View;

	class ObjectInfo;
	typedef boost::shared_ptr<ObjectInfo> ObjectPtr;
	typedef boost::weak_ptr<ObjectInfo> ObjectWeakPtr;

	class Layer;
	typedef boost::shared_ptr<Layer> LayerPtr;
	typedef boost::weak_ptr<Layer> LayerWeakPtr;

	/** Game Object Representation
	 *
	 *  @bug Missing documentation
	 */
	class ObjectInfo : public AttributedClass {
		public:
			typedef RenderableLocation type_visual_location;
			typedef boost::variant<LayerWeakPtr,ObjectWeakPtr> type_owner;

			static const std::string NameParam;
			static const std::string ObjectTypeParam;
			static const std::string LocationParam;
			static const std::string TypeParam;
			static const std::string OrientationParam;

			/** Factory method.
			 */
			static ObjectPtr create();

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

			void setPosition(const Point& p);
			const Point& getPosition() const;

			const Location& getLocation() const;
			size_t getLayerNumber() const;
			void setLayer(size_t);

			LayerPtr getLayer() const;

			size_t getElevation() const;
			void setElevation(size_t);

			// INVENTORY

			void addToInventory(ObjectPtr obj);
			void removeFromInventory(ObjectPtr obj);
			ObjectList& getInventory();
			const ObjectList& getInventory() const;

			// INTERNAL USE

			// OWNERSHIP (MOSTLY INTERNAL USE)

			void setOwner(LayerPtr  owner);
			void setOwner(ObjectPtr owner);
			void resetOwner();

			bool isOwner(LayerPtr  owner) const;
			bool isOwner(ObjectPtr owner) const;
			bool isIndirectOwner(ObjectPtr owner) const;

			bool hasOwner() const;

			type_owner& getOwner() { return m_owner; }
			const type_owner& getOwner() const { return m_owner; }

			const type_visual_location& getVisualLocation() const;
			void setVisualLocation(const type_visual_location& visual);

			void setVisualId(size_t visualId);
			size_t getVisualId() const;

			/** Load a prototype by it's id
			 */
			void loadPrototype(size_t proto_id);

			/** Load a prototype by it's name
			 */
			void loadPrototype(const std::string& proto_name);

			/** Get the list of prototypes, applied to this object
			 *  The returned list is in order of the loadPrototype
			 *  calls.
			 */
			const std::vector<size_t>& listPrototypes() const;

			void debugPrint();

			/** Get total number of objects
			 */
			static long globalCount();
		private:
			/** Constructor
			 */
			ObjectInfo();

			static long m_count;

			Location      m_location;
			type_owner    m_owner;
			ObjectWeakPtr m_self;

			/// Prototype ids
			std::vector<size_t> m_protoid;;

			ObjectList* m_inventory;

			size_t m_visualId;
			bool   m_isStatic;

			uint16_t m_zvalue;
			type_visual_location m_visual_location;

			friend struct owner_reset;

			ObjectInfo(const ObjectInfo&);
			ObjectInfo& operator=(const ObjectInfo&);
	};

	inline
	void ObjectInfo::setPosition(const Point& p) {
		m_location.position = p;
	}

	inline
	const Point& ObjectInfo::getPosition() const {
		return m_location.position;
	}

	inline
	const Location& ObjectInfo::getLocation() const {
		return m_location;
	}

	inline
	size_t ObjectInfo::getLayerNumber() const {
		return m_location.layer;
	}

	inline
	void ObjectInfo::setLayer(size_t layer) {
		m_location.layer = layer;
	}

	inline
	size_t ObjectInfo::getElevation() const {
		return m_location.elevation;
	}

	inline
	void ObjectInfo::setElevation(size_t elevation) {
		m_location.elevation = elevation;
	}

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
	void ObjectInfo::setVisualId(size_t visualId) { 
		m_visualId = visualId; 
	}

	inline
	size_t ObjectInfo::getVisualId() const {
		return m_visualId; 
	}

	
	inline 
	const ObjectList& ObjectInfo::getInventory() const {
		static ObjectList static_inventory;
		if( !m_inventory ) {
			return static_inventory;
		} else {
			return *m_inventory;
		}
	}

	inline 
	ObjectList& ObjectInfo::getInventory() {
		if( !m_inventory ) {
			m_inventory = new ObjectList();
		}
		return *m_inventory;
	}

} }

#endif // FIFE_MAPOBJECT_INFO_H
