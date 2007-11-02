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

#include <vector>

#include "util/quadtree.h"
#include "model/metamodel/modelcoords.h"

namespace FIFE {

	class Instance;

	class InstanceTree {
	public:
		/** Constructor
		 *
		 */
		InstanceTree(void);

		/** Destructor
		 *
		 */
		~InstanceTree(void);

		/** Adds an instance to the quad tree.
		 *
		 * Adds an instance to the quad tree based upon it's location on the layer and it's
		 * area. 
		 *
		 * @param instance A pointer to the instance to add.
		 */
		void addInstance(const Instance* instance);

		/** Removes an instance from the quad tree.
		 *
		 * Locates an instance in the quad tree then removes it.
		 *
		 * @param instance A pointer to the instance to find and remove.
		 */
		void removeInstance(const Instance* instance);

		/** Find all instances in a given area.
		 * 
		 * Takes a box as an area then returns a vector filled with all instances that intersect
		 * with that box.
		 *
		 * @param point A ModelCoordinate representing the upper left part of the search area.
		 * @param w The width of the search area in Model Units.
		 * @param h The height of the search area in Model Units.
		 * @return A vector containing all instances within that space.
		 */
		std::vector<Instance*> findAreaInstances(const ModelCoordinate& point, int w, int h);
	private:
		// The quadtree containing instances.
		QuadTree<Instance*> m_tree;
	};

}

#endif