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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/exception.h"

#include "objectinfo.h"
#include "factory.h"
#include "view.h"
#include "layer.h"

namespace FIFE { namespace map {

	const std::string ObjectInfo::NameParam        = "name";
	const std::string ObjectInfo::ObjectTypeParam  = "objtype";
	const std::string ObjectInfo::LocationParam    = "location";
	const std::string ObjectInfo::OrientationParam = "orientation";

	long ObjectInfo::m_count = 0;

	ObjectInfo::ObjectInfo() 
		: AttributedClass("GObject"),
		  m_visualId(0),
		  m_isStatic(true) {

		set<std::string>("objtype","undefined");
		set<std::string>("name","unnamed");

		m_zvalue = 2;
		m_inventory = 0;
		m_count += 1;
	}

	ObjectPtr ObjectInfo::create() {
		ObjectPtr object(new ObjectInfo());
		object->m_self = object;
		return object;
	}

	ObjectInfo::~ObjectInfo() {
		/**
			Make a test whether a MapVisual is still set for this
			object. On Deletion this will try to dereference an
			invalid pointer.

			We could use smart_ptr'd MapObjectInfos instead, but
			Then this would probably result in an untraceable leak.
		*/
		if (m_visualId != 0) {
			Warn("map_object")
				<< "Dangling MapVisual pointing to deleted MapObjectInfo";
			debugPrint();
		}
		m_count -= 1;

		if( m_inventory ) {
			delete m_inventory;
		}
	}

	void ObjectInfo::loadPrototype(size_t proto_id) {
		Factory::instance()->loadPrototype(this, proto_id);

		// Move the proto_id to the end of m_protoid
		// if it was used before, and otherwise append it.

		size_t proto_pos = m_protoid.size();
		for(size_t i = 0; i != m_protoid.size(); ++i) {
			if( m_protoid[i] == proto_id ) {
				proto_pos = i;
				break;
			}
		}
		if( proto_pos == m_protoid.size() ) {
			m_protoid.push_back(proto_id);
		} else {
			for(size_t i = proto_pos; i != m_protoid.size() - 1; ++i) {
				m_protoid[ i ] = m_protoid[ i + 1 ];
			}
			m_protoid.back() = proto_id;
		}

	}

	void ObjectInfo::loadPrototype(const std::string& proto_name) {
		loadPrototype( Factory::instance()->getPrototypeId(proto_name) );
	}

	const std::vector<size_t>& ObjectInfo::listPrototypes() const {
		return m_protoid;
	}


	struct owner_reset : public boost::static_visitor<> {
		ObjectWeakPtr m_self;
		owner_reset(ObjectWeakPtr self) : m_self(self) {}

		void operator()(LayerWeakPtr& my_layer) const {
			if( my_layer.expired() ) {
				return;
			}
			LayerPtr layer(my_layer.lock());
			layer->removeObject( m_self.lock() );
			my_layer.reset();
		}

		void operator()(ObjectWeakPtr& my_object) const {
			if( my_object.expired() ) {
				return;
			}
			ObjectPtr object(my_object.lock());
			object->getInventory().erase( m_self.lock() );
			my_object.reset();
		}

	};


	void ObjectInfo::setOwner(LayerPtr layer) {
		if( hasOwner() ) {
			boost::apply_visitor( owner_reset(m_self), m_owner);
		}
		m_owner = LayerWeakPtr(layer);
	}

	void ObjectInfo::setOwner(ObjectPtr object) {
		if( hasOwner() ) {
			boost::apply_visitor( owner_reset(m_self), m_owner);
		}
		m_owner = ObjectWeakPtr(object);
	}

	void ObjectInfo::resetOwner() {
		setOwner(ObjectPtr());
	}

	struct owner_expired : public boost::static_visitor<bool> {

		template<typename T>
		bool operator()(const T& my_owner) const { return my_owner.expired(); }

	};

	bool ObjectInfo::hasOwner() const {
		return !boost::apply_visitor( owner_expired(), m_owner );
	}

	template<typename T>
	bool is_owner(const ObjectInfo::type_owner& owner, const boost::shared_ptr<T>& object) {
		const boost::weak_ptr<T>* p = boost::get<boost::weak_ptr<T> >(&owner);
		if( !p || p->expired() ) {
			return false;
		}
		return p->lock() == object;
	}


	bool ObjectInfo::isOwner(LayerPtr  owner) const {
		return is_owner<Layer>(m_owner, owner);
	}

	bool ObjectInfo::isOwner(ObjectPtr owner) const {
		return is_owner<ObjectInfo>(m_owner, owner);
	}

	bool ObjectInfo::isIndirectOwner(ObjectPtr owner) const {
		const ObjectWeakPtr* p = boost::get<ObjectWeakPtr>(&m_owner);
		if( !p || p->expired() ) {
			return false;
		} else {
			ObjectPtr obj(p->lock());
			if( obj == owner ) {
				return true;
			} else {
				return obj->isIndirectOwner( owner );
			}
		}
	}

	LayerPtr ObjectInfo::getLayer() const {
		const LayerWeakPtr* p = boost::get<LayerWeakPtr>(&m_owner);
		if( !p ) {
			return LayerPtr();
		}
		return p->lock();
	}

	void ObjectInfo::addToInventory(ObjectPtr obj) {
		if( obj ) {
			if( isIndirectOwner( obj ) ) {
				throw Duplicate("Inventory ... This is not a Klein Bottle");
			}
			obj->setOwner( m_self.lock() );
			getInventory().append(obj); 
		}
	}

	void ObjectInfo::debugPrint() {
		Log log("object");

		log	<< "name " << get<std::string>("name")
			<< " at " << m_location
			<< " prototypes=";

		for(size_t i=0; i!= m_protoid.size(); ++i) {
			log <<  Factory::instance()->getPrototypeName( m_protoid[i] );
			if( i != m_protoid.size() - 1) {
				log << ",";
			}
		}
	}

	long ObjectInfo::globalCount() { 
		return m_count;
	}


} } //FIFE::map
