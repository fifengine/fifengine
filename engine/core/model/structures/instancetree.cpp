/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes


// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "model/structures/instance.h"
#include "util/structures/rect.h"

#include "instancetree.h"


namespace FIFE {

	InstanceTree::InstanceTree(): FifeClass() {
	}

	InstanceTree::~InstanceTree() {
	}

	void InstanceTree::addInstance(Instance* instance) {
		ModelCoordinate coords = instance->getLocationRef().getLayerCoordinates();
		InstanceTreeNode * node = m_tree.find_container(coords.x,coords.y,0,0);
		InstanceList& list = node->data();
		list.push_back(instance);
		if( m_reverse.find(instance) != m_reverse.end() )
			throw new InconsistencyDetected("Duplicate Instance.");
		m_reverse[instance] = node;
	}

	void InstanceTree::removeInstance(Instance* instance) {
		ModelCoordinate coords = instance->getLocationRef().getLayerCoordinates();
		InstanceTreeNode * node = m_reverse[instance];
		if( !node )
			throw new InconsistencyDetected("Removing Ghost Instance.");
		m_reverse.erase(instance);
		InstanceList& list = node->data();
		for(InstanceList::iterator i = list.begin(); i != list.end(); ++i) {
			if((*i) == instance) {
				list.erase(i);
				return;
			}
		}
		throw new InconsistencyDetected("Removing Ghost Instance (not in list?).");
	}

	class InstanceListCollector {
		public:
			InstanceTree::InstanceList& instanceList;
			Rect searchRect;
			InstanceListCollector(InstanceTree::InstanceList& a_instanceList, const Rect& rect)
			: instanceList(a_instanceList), searchRect(rect) {
			}
			bool visit(InstanceTree::InstanceTreeNode* node, int d);
	};

	bool InstanceListCollector::visit(InstanceTree::InstanceTreeNode* node, int d) {
		InstanceTree::InstanceList& list = node->data();
		for(InstanceTree::InstanceList::const_iterator it(list.begin()); it != list.end(); ++it) {
			ModelCoordinate coords = (*it)->getLocationRef().getLayerCoordinates();
			if( searchRect.contains(Point(coords.x,coords.y)) ) {
				instanceList.push_back(*it);
			}
		}
		return true;
	}

	void InstanceTree::findInstances(const ModelCoordinate& point, int w, int h, InstanceTree::InstanceList& list) {
		InstanceTreeNode * node = m_tree.find_container(point.x, point.y, w, h);
		Rect rect(point.x, point.y, w, h);
		InstanceListCollector collector(list,rect);

		node->apply_visitor(collector);

		node = node->parent();
		while( node ) {
			for(InstanceList::const_iterator it(node->data().begin()); it != node->data().end(); ++it) {
				ModelCoordinate coords = (*it)->getLocationRef().getLayerCoordinates();
				if( rect.contains(Point(coords.x,coords.y)) ) {
					list.push_back(*it);
				}
			}
			node = node->parent();
		}
	}

}
