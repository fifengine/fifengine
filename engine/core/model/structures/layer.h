/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_LAYER_H
#define FIFE_LAYER_H

// Standard C++ library includes
#include <algorithm>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"
#include "util/attributedclass.h"
#include "model/metamodel/object.h"

#include "instance.h"

namespace FIFE {

	class Map;
	class Selection;
	class CellGrid;
	class Object;
	class InstanceTree;

	/** Defines how pathing can be performed on this layer
	 *
	 * CELL_EDGES_ONLY allows pather to use only cell edges when moving instances from cell to cell on map
	 * CELL_EDGES_AND_DIAGONALS allows pather to use both cell edges and diagonals when moving instances from cell to cell on map
	 * FREEFORM allows pather to find shortest route regardless of cellgrid used on the layer 
	 */
	enum PathingStrategy {
		CELL_EDGES_ONLY,
		CELL_EDGES_AND_DIAGONALS,
		FREEFORM
	};
	
	/** A basic layer on a map
	 *
	 * @bug These comments are very outdated!
	 *
	 * This class represents a layer on the Map.
	 * This can be for example a Tile layer 
	 * as the roofs and floors of a fo2 map
	 * but can also just contain "objects"
	 * on Layer coords.
	 * 
	 * The tiles are *not* allways created only on
	 * a first "setTileGID".
	 * 
	 * The most important features of this class are
	 * "cellgrid", "shift" and "size":
	 *
	 * The cellgrid is used to position objects on this
	 * Layer and the Tiles too.
	 *
	 * The shift is added to all screen coords and
	 * will create the illusion of a height-difference :-)
	 *
	 * The size simply is the maximum allowd size in Layer
	 * coords this Layer covers.
	 * 
	 * @bug The parameter code is untested, be warned.
	 * @bug setTileGID and setParam behave differently on invalid positions.
	 *
	 * Attributes: 
	 * 
	 * Future:
	 * 	Connections between Layers to walk through (Elevators...)
	 * Grouping of Layers (These Layers are roofs ... etc)
	 */
	class Layer : public AttributedClass {
		public:
			/** Constructor
			 * Layers are created by calling addLayer from map, thus
			 * this method should really be called only by elevation or test code
			 */
			Layer(const std::string& identifier, Map* map, CellGrid* grid);

			/** Destructs a Layer instance
			 */
			~Layer();

			/** Get the map this layer is contained in
			 */
			Map* getMap() const { return m_map; }

			/** Get the Cellgrid as set in the constructor
			 * @return a valid cellgrid
			 */
			CellGrid* getCellGrid() const { return m_grid; }

			/** Get the instance tree. 
			 * @return this layers instance tree.
			 */
			InstanceTree* getInstanceTree(void) const { return m_instanceTree; }

			/** Check existance of objects on this layer
			 * @return True, if objects exist.
			 */
			bool hasInstances() const;

			/** Add an instance of an object at a specific position
			 */
			Instance* createInstance(Object* object, const ModelCoordinate& p, const std::string& id="");

			/** Add an instance of an object at a specific position
			 */
			Instance* createInstance(Object* object, const ExactModelCoordinate& p, const std::string& id="");
						
			/** Remove an instance from the layer
			 */
			void deleteInstance(Instance* object);

			/** Get the list of instances on this layer
			 */
			const std::vector<Instance*>& getInstances();

			/** Get a list of instances on this layer with a value.
			 * @param field the (string) field to search on
			 * @param value the value to be found in the field
			 *
			 *  Special field: 'loc', the location of the instance.
			 *
			 */
			std::vector<Instance*> getInstances(const std::string& field, const std::string& value);

			/** Set object visibility
			 */
			void setInstancesVisible(bool vis);

			/** Retrieves the minimum/maximum coordinates of instances on the layer.
			 * @param min A reference to a ModelCoordinate that will hold the minimum coordinate.
			 * @param max A reference to a ModelCoordinate that will hold the maximum coordinate.
			 * @param layer A pointer to another layer that can be used to cast coordinates bettween layers.
			 */
			void getMinMaxCoordinates(ModelCoordinate& min, ModelCoordinate& max, const Layer* layer = 0) const;

			/** Determines if a given cell on the layer contains a blocking instance
			 *
			 * @param cellCoordinate A const reference to a model coordinate of the cell in question.
			 * @return A boolean, true if it is blocked false otherwise.
			 */
			bool cellContainsBlockingInstance(const ModelCoordinate& cellCoordinate);

			/** Toggle object visibility
			 * @see setObjectsVisible
			 */
			void toggleInstancesVisible();

			/** Check object visibility
			 * @see setObjectsVisible
			 */
			bool areInstancesVisible() const { return m_instances_visibility; }

			/** Called periodically to update events on layer
			 */
			void update();
			
			/** Sets pathing strategy for the layer
			 * @see PathingStrategy
			 */
			void setPathingStrategy(PathingStrategy strategy) { m_pathingstrategy = strategy; }
			
			/** Gets pathing strategy for the layer
			 * @see PathingStrategy
			 */
			PathingStrategy getPathingStrategy() const { return m_pathingstrategy; }

		protected:
			Map* m_map;

			bool m_instances_visibility;

			// all the instances on this layer
			std::vector<Instance*> m_instances;
	
			//The instance tree
			InstanceTree* m_instanceTree;

			// layer's cellgrid
			CellGrid* m_grid;
			
			// pathing strategy for the layer
			PathingStrategy m_pathingstrategy;
	};

} // FIFE

#endif
