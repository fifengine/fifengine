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

#ifndef FIFE_INSTANCETREE_H
#define FIFE_INSTANCETREE_H

// Standard C++ library includes
#include <list>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src
#include "util/base/fifeclass.h"

#include "util/structures/quadtree.h"
#include "model/metamodel/modelcoords.h"

namespace FIFE {

	class Instance;

	class InstanceTree: public FifeClass {
	public:
		static const int32_t MIN_TREE_SIZE = 2;

		typedef std::list<Instance*> InstanceList;
		typedef QuadTree< InstanceList, MIN_TREE_SIZE > InstanceQuadTree;
		typedef InstanceQuadTree::Node InstanceTreeNode;

		/** Constructor
		 *
		 */
		InstanceTree();

		/** Destructor
		 *
		 */
		virtual ~InstanceTree();

		/** Adds an instance to the quad tree.
		 *
		 * Adds an instance to the quad tree based upon it's location on the layer and it's
		 * area.
		 *
		 * @param instance A pointer to the instance to add.
		 * @note In case you added the instance before this will throw.
		 */
		void addInstance(Instance* instance);

		/** Removes an instance from the quad tree.
		 *
		 * Locates an instance in the quad tree then removes it.
		 *
		 * @param instance A pointer to the instance to find and remove.
		 * @note In case you did @b not add the instance before this will throw.
		 */
		void removeInstance(Instance* instance);

		/** Find all instances in a given area.
		 *
		 * Takes a box as an area then returns a vector filled with all instances that intersect
		 * with that box.
		 *
		 * @param point A ModelCoordinate representing the upper left part of the search area.
		 * @param w The width of the search area in Model Units.
		 * @param h The height of the search area in Model Units.
		 * @param list vector reference that will be filled with all instances within that space.
		 */
		void findInstances(const ModelCoordinate& point, int32_t w, int32_t h, InstanceList& list);

		/** See QuadNode::apply_visitor
		 */
		template<typename Visitor> void applyVisitor(Visitor& visitor) {
			m_tree.apply_visitor(visitor);
		}


	private:
		InstanceQuadTree m_tree;
		std::map<Instance*,InstanceTreeNode*> m_reverse;
	};

}

#endif
