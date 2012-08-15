/***************************************************************************
 *   Copyright (C) 2005-2012 by the FIFE team                              *
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

#ifndef FIFE_LAYER_H
#define FIFE_LAYER_H

// Standard C++ library includes
#include <algorithm>
#include <string>
#include <vector>
#include <set>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"
#include "util/structures/rect.h"
#include "model/metamodel/modelcoords.h"
#include "model/metamodel/object.h"

#include "instance.h"

namespace FIFE {

	class Map;
	class CellGrid;
	class Object;
	class InstanceTree;
	class CellCache;

	/** Defines how pathing can be performed on this layer
	 *
	 * CELL_EDGES_ONLY allows pather to use only cell edges when moving instances from cell to cell on map
	 * CELL_EDGES_AND_DIAGONALS allows pather to use both cell edges and diagonals when moving instances from cell to cell on map
	 */
	enum PathingStrategy {
		CELL_EDGES_ONLY,
		CELL_EDGES_AND_DIAGONALS
	};

	/** Listener interface for changes happening on a layer
	 */
	class LayerChangeListener {
	public:
		virtual ~LayerChangeListener() {};

		/** Called when some instance is changed on layer. @see InstanceChangeType
		 * @param layer where change occurred
		 * @param changedInstances list of instances containing some changes
		 * @note Does not report creations and deletions
		 */
		virtual void onLayerChanged(Layer* layer, std::vector<Instance*>& changedInstances) = 0;

		/** Called when some instance gets created on layer
		 * @param layer where change occurred
		 * @param instance which got created
		 */
		virtual void onInstanceCreate(Layer* layer, Instance* instance) = 0;

		/** Called when some instance gets deleted on layer
		 * @param layer where change occurred
		 * @param instance which will be deleted
		 * @note right after this call, instance actually gets deleted!
		 */
		virtual void onInstanceDelete(Layer* layer, Instance* instance) = 0;
	};


	/** A basic layer on a map
	 */
	class Layer : public FifeClass {
		public:
			/** Constructor
			 * Layers are created by calling addLayer from map, thus
			 * this method should really be called only by map or test code.
			 */
			Layer(const std::string& identifier, Map* map, CellGrid* grid);

			/** Destructs a Layer instance
			 */
			~Layer();

			/** Get the id of this layer.
			 */
			const std::string& getId() const;

			/** Sets the identifier for this layer.
			 */
			void setId(const std::string& id);

			/** Get the map this layer is contained in
			 */
			Map* getMap() const;

			/** Get the Cellgrid
			 * @return pointer to a valid cellgrid
			 */
			CellGrid* getCellGrid() const;

			/** Set the Cellgrid
			 */
			void setCellGrid(CellGrid* grid);

			/** Get the instance tree.
			 * @return this layers instance tree.
			 */
			InstanceTree* getInstanceTree(void) const;

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

			/** Add a valid instance at a specific position. This is temporary. It will be moved to a higher level
			later so that we can ensure that each Instance only lives in one layer.
			 */
			bool addInstance(Instance* instance, const ExactModelCoordinate& p);
			
			/** Remove an instance from the layer
			 */
			void removeInstance(Instance* instance);
			
			/** Remove an instance from the layer and delete it
			 */
			void deleteInstance(Instance* instance);

			/** Get the list of instances on this layer
			 */
			const std::vector<Instance*>& getInstances() const;

			/** Get the list of instances on this layer with the given identifier.
			 */
			std::vector<Instance*> getInstances(const std::string& id);

			/** Returns instances that match given location.
			 * @param loc location where to fetch instances from
			 * @param use_exactcoordinates if true, comparison is done using exact coordinates. if not, cell coordinates are used
			 */
			std::vector<Instance*> getInstancesAt(Location& loc, bool use_exactcoordinates=false);

			/** Returns instances that match given rect.
			 * @param rec rect where to fetch instances from
			 */
			std::list<Instance*> getInstancesIn(Rect& rec);

			/** Get the first instance on this layer with the given identifier.
			 */
			Instance* getInstance(const std::string& identifier);

			/** Set object visibility
			 */
			void setInstancesVisible(bool vis);

			/** Sets the transparency of all instances on the layer.  0=opaque, 255=transparent
			 * @param transparency Transparency value from 0-255.
			*/
			void setLayerTransparency(uint8_t transparency);

			/** Returns the layer's transparency value
			*/
			uint8_t getLayerTransparency();

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

			/** Returns instances that blocks on given cell
			 *
			 * @param cellCoordinate A const reference to a model coordinate of the cell in question.
			 * @return A vector that contains instances.
			 */
			std::vector<Instance*> getBlockingInstances(const ModelCoordinate& cellCoordinate);

			/** Toggle object visibility
			 * @see setObjectsVisible
			 */
			void toggleInstancesVisible();

			/** Check object visibility
			 * @see setObjectsVisible
			 */
			bool areInstancesVisible() const;

			/** Called periodically to update events on layer
			 * @returns true if layer was changed since the last update, false otherwise
			 */
			bool update();

			/** Sets pathing strategy for the layer
			 * @see PathingStrategy
			 */
			void setPathingStrategy(PathingStrategy strategy);

			/** Gets pathing strategy for the layer
			 * @see PathingStrategy
			 */
			PathingStrategy getPathingStrategy() const;

			/** Sets walkable for the layer. Only a walkable layer, can create a CellCache and
			 *  only on a walkable, instances can move. Also interact layer can only be added to walkables.
			 * @param walkable A boolean that mark a layer as walkable.
			 */
			void setWalkable(bool walkable);
			
			/** Returns if a layer is walkable.
			 * @return A boolean, true if the layer is walkable otherwise false.
			 */
			bool isWalkable();
			
			/** Sets interact for the layer. The data(size, instances) from all interact layers
			 *  and the walkable layer will merged into one CellCache.
			 * @param interact A boolean that mark a layer as interact.
			 * @param id A const reference to a string that should refer to the id of the walkable layer.
			 */
			void setInteract(bool interact, const std::string& id);
			
			/** Returns if a layer is interact.
			 * @return A boolean, true if the layer is interact otherwise false.
			 */
			bool isInteract();

			/** Returns the id of the walkable layer if this is a interact layer otherwise the string is empty.
			 * @return A const reference to a string that refer to the id of the walkable layer.
			 */
			const std::string& getWalkableId();

			/** Adds a interact layer to the walkable layer.
			 * @param layer A pointer to the interact layer that should be added.
			 */
			void addInteractLayer(Layer* layer);

			/** Returns all assigned interact layer.
			 * @return A const reference to a vector with pointers to interact layers.
			 */
			const std::vector<Layer*>& getInteractLayers();

			/** Removes a interact layer from the walkable layer.
			 * @param layer A pointer to the interact layer that should be removed.
			 */
			void removeInteractLayer(Layer* layer);

			/** Called from Map to create a CellCache. Only walkable layers can create one CellCache.
			 */
			void createCellCache();

			/** Returns the CellCache of this layer. In case there is no it returns NULL.
			 * @return A pointer to the CellCache.
			 */
			CellCache* getCellCache();

			/** Adds new change listener
			* @param listener to add
			*/
			void addChangeListener(LayerChangeListener* listener);

			/** Removes associated change listener
			* @param listener to remove
			*/
			void removeChangeListener(LayerChangeListener* listener);

			/** Returns true, if layer information was changed during previous update round
			*/
			bool isChanged();

			/** Returns instances that were changed during previous update round.
			 * @note does not contain created or deleted instances
			 */
			std::vector<Instance*>& getChangedInstances();

			/** Sets the activity status for given instance on this layer.
			 * @param instance A pointer to the Instance whose activity is to be changed.
			 * @param active A boolean, true if the instance should be set active otherwise false.
			 */
			void setInstanceActivityStatus(Instance* instance, bool active);

		protected:
			//! string identifier
			std::string m_id;
			//! pointer to map
			Map* m_map;
			//! if true the instances are visibility otherwise they are skipped during rendering
			bool m_instancesVisibility;
			//! transparency, value 0 means total visible, 128 semi-transparent and 255 invisibility
			uint8_t m_transparency;
			//! all the instances on this layer
			std::vector<Instance*> m_instances;
			//! all the active instances on this layer
			std::set<Instance*> m_activeInstances;
			//! The instance tree
			InstanceTree* m_instanceTree;
			//! layer's cellgrid
			CellGrid* m_grid;
			//! pathing strategy for the layer
			PathingStrategy m_pathingStrategy;
			//! is walkable true/false
			bool m_walkable;
			//! is interact true/false
			bool m_interact;
			//! walkable id
			std::string m_walkableId;
			//! all assigned interact layers
			std::vector<Layer*> m_interacts;
			//! pointer to cellcache
			CellCache* m_cellCache;
			//! listeners for layer changes
			std::vector<LayerChangeListener*> m_changeListeners;
			//! holds changed instances after each update
			std::vector<Instance*> m_changedInstances;
			//! true if layer (or it's instance) information was changed during previous update round
			bool m_changed;
	};

} // FIFE

#endif
