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
#include "model/structures/instance.h"

#include "instancetree.h"


namespace FIFE {

	InstanceTree::InstanceTree(): FifeClass() {
	}

	InstanceTree::~InstanceTree() {
	}

	bool InstanceTree::addInstance(Instance* instance) {
		ModelCoordinate coords = instance->getLocation().getLayerCoordinates();
		InstanceList& list = m_tree.find_container(coords.x,coords.y,0,0)->data();
		list.push_back(instance);
		return true;
	}

	bool InstanceTree::removeInstance(Instance* instance) {
		ModelCoordinate coords = instance->getLocation().getLayerCoordinates();
		InstanceList& list = m_tree.find_container(coords.x,coords.y, 0, 0)->data();

		for(InstanceList::iterator i = list.begin(); i != list.end(); ++i) {
			if((*i) == instance) {
				list.erase(i);
				return true;
			}
		}

		return false;
	}

	class InstanceListCollector {
		public:
			InstanceTree::InstanceList& instanceList;
			InstanceListCollector(InstanceTree::InstanceList& a_instanceList) : instanceList(a_instanceList) {
			}
			bool visit(InstanceTree::InstanceTreeNode* node, int d);
	};

	bool InstanceListCollector::visit(InstanceTree::InstanceTreeNode* node, int d) {
		InstanceTree::InstanceList& list = node->data();
		std::copy(list.begin(),list.end(),std::back_inserter(instanceList));
		return true;
	}

	void InstanceTree::findInstances(const ModelCoordinate& point, int w, int h, InstanceTree::InstanceList& list) {
		InstanceTreeNode * node = m_tree.find_container(point.x, point.y, w, h);
		InstanceListCollector collector(list);
		node->apply_visitor(collector);

		node = node->parent();
		while( node ) {
			std::copy(node->data().begin(),node->data().end(),std::back_inserter(list));
			node = node->parent();
		}
	}

}
