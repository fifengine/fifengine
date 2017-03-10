/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
#include "util/log/logger.h"
#include "model/structures/instance.h"
#include "util/structures/rect.h"

#include "instancetree.h"


namespace FIFE {
	static Logger _log(LM_STRUCTURES);

	InstanceTree::InstanceTree(): FifeClass() {
	}

	InstanceTree::~InstanceTree() {
	}

	void InstanceTree::addInstance(Instance* instance) {
		ModelCoordinate coords = instance->getLocationRef().getLayerCoordinates();
		InstanceTreeNode * node = m_tree.find_container(coords.x,coords.y,0,0);
		InstanceList& list = node->data();
		list.push_back(instance);
		if( m_reverse.find(instance) != m_reverse.end() ) {
			FL_WARN(_log, "InstanceTree::addInstance() - Duplicate Instance.  Ignoring.");
			return;
		}
		m_reverse[instance] = node;
	}

	void InstanceTree::removeInstance(Instance* instance) {
		InstanceTreeNode * node = m_reverse[instance];
		if( !node ) {
			FL_WARN(_log, "InstanceTree::removeInstance() - Instance not part of tree.");
			return;
		}
		m_reverse.erase(instance);
		InstanceList& list = node->data();
		for(InstanceList::iterator i = list.begin(); i != list.end(); ++i) {
			if((*i) == instance) {
				list.erase(i);
				return;
			}
		}
		FL_WARN(_log, "InstanceTree::removeInstance() - Instance part of tree but not found in the expected tree node.");
	}

	class InstanceListCollector {
		public:
			InstanceTree::InstanceList& instanceList;
			Rect searchRect;
			InstanceListCollector(InstanceTree::InstanceList& a_instanceList, const Rect& rect)
			: instanceList(a_instanceList), searchRect(rect) {
			}
			bool visit(InstanceTree::InstanceTreeNode* node, int32_t d);
	};

	bool InstanceListCollector::visit(InstanceTree::InstanceTreeNode* node, int32_t d) {
		InstanceTree::InstanceList& list = node->data();
		for(InstanceTree::InstanceList::const_iterator it(list.begin()); it != list.end(); ++it) {
			ModelCoordinate coords = (*it)->getLocationRef().getLayerCoordinates();
			if( searchRect.contains(Point(coords.x,coords.y)) ) {
				instanceList.push_back(*it);
			}
		}
		return true;
	}

	void InstanceTree::findInstances(const ModelCoordinate& point, int32_t w, int32_t h, InstanceTree::InstanceList& list) {
		list.clear();
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
