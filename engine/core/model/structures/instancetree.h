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

#ifndef FIFE_INSTANCETREE_H
#define FIFE_INSTANCETREE_H

// Standard C++ library includes
#include <list>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src
#include "util/fifeclass.h"

#include "util/quadtree.h"
#include "model/metamodel/modelcoords.h"

namespace FIFE {

	class Instance;

	class InstanceTree: public FifeClass {
	public:
		typedef std::list<Instance*> InstanceList;
		
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
		 */
		bool addInstance(Instance* instance);

		/** Removes an instance from the quad tree.
		 *
		 * Locates an instance in the quad tree then removes it.
		 *
		 * @param instance A pointer to the instance to find and remove.
		 */
		bool removeInstance(Instance* instance);

		/** Find all instances in a given area.
		 * 
		 * Takes a box as an area then returns a vector filled with all instances that intersect
		 * with that box.
		 *
		 * @param point A ModelCoordinate representing the upper left part of the search area.
		 * @param w The width of the search area in Model Units.
		 * @param h The height of the search area in Model Units.
		 * @param lst vector reference that will be filled with all instances within that space.
		 * @return A boolean to signify whether the instance container was found.
		 */
		bool getInstanceList(const ModelCoordinate& point, int w, int h, InstanceList& lst);
	
	private:
		typedef QuadTree< InstanceList > InstanceQuadTree;
		typedef InstanceQuadTree::Node InstanceTreeNode;
		
		/** Find all instances in a given area.
		 * 
		 * Takes a box as an area then returns a vector filled with all instances that intersect
		 * with that box.
		 *
		 * @param point A ModelCoordinate representing the upper left part of the search area.
		 * @param w The width of the search area in Model Units.
		 * @param h The height of the search area in Model Units.
		 * @return A vector pointer to a vector of the contents of the node.
		 */
		InstanceList* getInstanceList(const ModelCoordinate& point, int w, int h);
		
		// The quadtree containing instances.
		InstanceQuadTree m_tree;
	};

}

#endif
