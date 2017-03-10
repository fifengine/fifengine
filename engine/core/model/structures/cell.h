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

#ifndef FIFE_CELL_H
#define FIFE_CELL_H

// Standard C++ library includes
#include <vector>
#include <set>
#include <list>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"
#include "model/metamodel/modelcoords.h"


namespace FIFE {

	class Instance;
	class Layer;
	class Cell;
	class Zone;

	static const double MIN_CELL_Z = -9999999;

	/** Defines different blocker types.
	 *
	 * CTYPE_NO_BLOCKER means there is no blocker.
	 * CTYPE_STATIC_BLOCKER means there is at least one blocking instance which is static.
	 * CTYPE_DYNAMIC_BLOCKER means there is at least one blocking instance which is not static.
	 * CTYPE_CELL_NO_BLOCKER means there will never be a blocker. Regardless of the instances on this cell.
	 * CTYPE_CELL_BLOCKER means there will always be a blocker. Regardless of the instances on this cell.
	 */
	enum CellType {
		CTYPE_NO_BLOCKER = 0,
		CTYPE_CELL_NO_BLOCKER = 1,
		CTYPE_DYNAMIC_BLOCKER = 2,
		CTYPE_STATIC_BLOCKER = 3,
		CTYPE_CELL_BLOCKER = 4
	};
	typedef uint8_t CellTypeInfo;

	/** Defines different cell visuals which are used for Fog of War.
	 *
	 * CELLV_CONCEALED mark the cell as hidden.
	 * CELLV_REVEALED mark the cell as explored and in visible range.
	 * CELLV_MASKED mark the cell as explored but currently not in visible range.
	 */
	enum CellVisualType {
		CELLV_CONCEALED = 0,
		CELLV_REVEALED,
		CELLV_MASKED
	};
	typedef uint8_t CellVisualEffect;

	/** Simple class to hold the data for transistions.
	 */
	class TransitionInfo {
		public:
			TransitionInfo(Layer* layer) { m_layer = layer; m_difflayer = false; m_immediate = true;}
			~TransitionInfo() {};
			//! target layer
			Layer* m_layer;
			//! target coordinates
			ModelCoordinate m_mc;
			//! is target on another layer
			bool m_difflayer;
			//! use immediate
			bool m_immediate;
	};

	/** Listener interface for deletions happening on a cell, used for transistions.
	 */
	class CellDeleteListener {
	public:
		virtual ~CellDeleteListener() {};
		
		/** Called when a cell gets deleted.
		 * @param cell which will be deleted.
		 */
		virtual void onCellDeleted(Cell* cell) = 0;
	};
	
	/** Listener interface for changes happening on a cell.
	 */
	class CellChangeListener {
	public:
		virtual ~CellChangeListener() {};
		
		/** Called when some instance entered the cell.
		 * @param cell where enter occurred.
		 * @param instance which enter the cell.
		 */
		virtual void onInstanceEnteredCell(Cell* cell, Instance* instance) = 0;

		/** Called when some instance exited the cell.
		 * @param cell where exit occurred.
		 * @param instance which exit the cell.
		 */
		virtual void onInstanceExitedCell(Cell* cell, Instance* instance) = 0;

		/** Called when some instance changed its blocking property.
		 * @param cell where exit occurred.
		 * @param type blocking type @see CellType.
		 * @param blocks true if the CellType indicates the cell as a blocker, otherwise false.
		 */
		virtual void onBlockingChangedCell(Cell* cell, CellTypeInfo type, bool blocks) = 0;
	};

	/** A basic cell on a CellCache.
	 */
	class Cell: public FifeClass, public CellDeleteListener {
		public:
			/** Constructor
			 * @param coordint A integer value that represents the cell identifier. Based on coordinates.
			 * @param coordinate A ModelCoordinate that specifies the coordinates of the cell.
			 * @param layer A pointer to the layer which is associated with the cell.
			 */
			Cell(int32_t coordint, ModelCoordinate coordinate, Layer* layer);
			
			/** Destructor
			 */
			~Cell();

			/** Adds instances to this cell.
			 * @param instances A const reference to list that contains instances.
			 */
			void addInstances(const std::list<Instance*>& instances);
			
			/** Adds a instance to this cell.
			 * @param instance A pointer to the instance.
			 */
			void addInstance(Instance* instance);

			/** Changes a instance on this cell.
			 * @param instance A pointer to the instance.
			 */
			void changeInstance(Instance* instance);

			/** Removes a instance from this cell.
			 * @param instance A pointer to the instance.
			 */
			void removeInstance(Instance* instance);

			/** Called to check if given cell is a neighbor.
			 * @param cell A pointer to cell.
			 * @return True if cell is a neighbor, otherwise false.
			 */
			bool isNeighbor(Cell* cell);

			/** Called to update cell data.
			 * @return True if cell was changed since the last update, false otherwise.
			 */
			void updateCellInfo();

			/** Returns if cell use default cost.
			 * @return True if the cell use the default cost, otherwise false.
			 */
			bool defaultCost();

			/** Changes the cell cost.
			 * @param multi A double its value is used instead of the default cost.
			 */
			void setCostMultiplier(double multi);

			/** Returns the current cell cost.
			 * @return The currently used cost value as a double.
			 */
			double getCostMultiplier();

			/** Resets the cell cost to default, 1.0.
			 */
			void resetCostMultiplier();

			/** Returns if cell use default speed.
			 * @return True if the cell use the default speed, otherwise false.
			 */
			bool defaultSpeed();

			/** Changes the cell speed.
			 * @param multi A double its value is used instead of the default speed.
			 */
			void setSpeedMultiplier(double multi);

			/** Returns the current cell speed.
			 * @return The currently used speed value as a double.
			 */
			double getSpeedMultiplier();

			/** Resets the cell speed to default, 1.0.
			 */
			void resetSpeedMultiplier();

			/** Returns zone.
			 * @return A pointer to the zone.
			 */
			Zone* getZone();

			/** Sets zone.
			 * @param zone A pointer to the zone.
			 */
			void setZone(Zone* zone);

			/** Resets zone.
			 * Zone pointer is NULL and isInserted() returns false.
			 */
			void resetZone();

			/** Returns whether the cell is part of a zone.
			 * @return True if the cell is inserted into a zone, otherwise false.
			 */
			bool isInserted();

			/** Mark cell as inserted. 
			 * @param inserted A boolean, true marks the cell as inserted, false as not inserted.
			 */
			void setInserted(bool inserted);

			/** Returns whether the zone on this cell is protected.
			 * @return True if the zone is protected, otherwise false.
			 */
			bool isZoneProtected();

			/** Mark zone on this cell as protected.
			 * @param protect A boolean, true marks the cell as inserted.
			 */
			void setZoneProtected(bool protect);

			/** Returns blocker type.
			 * @see CellType
			 */
			CellTypeInfo getCellType();

			/** Sets blocker type.
			 * @see CellType
			 */
			void setCellType(CellTypeInfo type);

			/** Returns all instances on this cell.
			 * @return A const reference to a set that refer to the instances on this cell.
			 */
			const std::set<Instance*>& getInstances();

			/** Sets the cell identifier.
			 * @param id A unique int value that is used as identifier. Based on the cell position.
			 */
			void setCellId(int32_t id);

			/** Returns the cell identifier.
			 * @return A unique int value that is used as identifier. Based on the cell position.
			 */
			int32_t getCellId();

			/** Returns the layer coordinates of this cell.
			 * @return A const reference to a ModelCoordinate.
			 */
			const ModelCoordinate getLayerCoordinates() const;

			/** Adds a neighbor cell to this cell.
			 * @param cell A pointer to the cell that should added as neighbor.
			 */
			void addNeighbor(Cell* cell);

			/** Returns the layer coordinates of this cell.
			 * @return A const reference to a set of all neighbor cells.
			 */
			const std::vector<Cell*>& getNeighbors();

			/** Removes all neighbors from cell.
			 */
			void resetNeighbors();

			/** Returns the current layer.
			 * @return A pointer to the currently used layer.
			 */
			Layer* getLayer();

			/** Creates a transistion from this cell to the given layer and coordinates.
			 * @param layer A pointer to the layer whereto the transition takes.
			 * @param mc A const reference to a ModelCoordinate whereto the transition takes.
			 * @param immediate A boolean, true means the transition is always used,
				if false the transistion is only used if it is a part of the path e.g. as an abbreviation.
			 */
			void createTransition(Layer* layer, const ModelCoordinate& mc, bool immediate = false);

			/** Removes the transistion from Cell and CellCache.
			 */
			void deleteTransition();

			/** Returns the transition.
			 * @return A pointer to the transition.
			 */
			TransitionInfo* getTransition();
			
			/** Sets the cell visual.
			 * @see CellVisualType
			 */
			void setFoWType(CellVisualEffect type);

			/** Returns the cell visual.
			 * @see CellVisualType
			 */
			CellVisualEffect getFoWType();

			/** Adds a instance as visitor to this cell. Instance visitor radius is then used e.g. for Fog of War.
			 * @param instance A pointer to the instance.
			 */
			void addVisitorInstance(Instance* instance);

			/** Removes the visitor instance from this cell.
			 * @param instance A pointer to the instance.
			 */
			void removeVisitorInstance(Instance* instance);

			/** Returns all visitor instances on this cell.
			 * @return A const reference to a set of all visitor instances.
			 */
			const std::vector<Instance*>& getVisitorInstances();

			/** Adds new cell delete listener.
			 * @param listener A pointer to the listener.
			 */
			void addDeleteListener(CellDeleteListener* listener);

			/** Removes cell delete listener.
			 * @param listener A pointer to the listener.
			 */
			void removeDeleteListener(CellDeleteListener* listener);

			/** Called when a cell gets deleted on this cell. If a delete listener was added.
			 * @param cell A pointer to the cell which will be deleted.
			 * @see CellDeleteListener
			 */
			void onCellDeleted(Cell* cell);

			/** Adds new cell change listener.
			 * @param listener A pointer to the listener.
			 * @see CellChangeListener
			 */
			void addChangeListener(CellChangeListener* listener);

			/** Removes cell change listener.
			 * @param listener A pointer to the listener.
			 * @see CellChangeListener
			 */
			void removeChangeListener(CellChangeListener* listener);

			/** Called when a instance entered this cell.
			 * @param instance A pointer to the instance which will be entered.
			 * @see CellChangeListener
			 */
			void callOnInstanceEntered(Instance* instance);

			/** Called when a instance exited this cell.
			 * @param instance A pointer to the instance which will be exited.
			 * @see CellChangeListener
			 */
			void callOnInstanceExited(Instance* instance);

			/** Called when the blocking property of this cell changed.
			 * @param blocks A boolean, true mark the cell as blocker.
			 * @see CellChangeListener
			 */
			void callOnBlockingChanged(bool blocks);

		private:

			void updateCellBlockingInfo();
			void updateCellFowInfo();

			//! holds coordinate as a unique integer id
			int32_t m_coordId;
			
			//! holds coordinate
			ModelCoordinate m_coordinate;

			//! parent layer
			Layer* m_layer;

			//! parent Zone
			Zone* m_zone;

			//! Pointer to Transistion
			TransitionInfo* m_transition;

			//! already inserted
			bool m_inserted;

			//! protected
			bool m_protect;

			//! CellType
			CellTypeInfo m_type;

			//! visual cell status
			CellVisualEffect m_fowType;

			// contained Instances
			std::set<Instance*> m_instances;

			//! contains visitor instances
			std::vector<Instance*> m_visitors;

			//! neighbor cells
			std::vector<Cell*> m_neighbors;

			//! delete listener
			std::vector<CellDeleteListener*> m_deleteListeners;

			//! change listener
			std::vector<CellChangeListener*> m_changeListeners;
	};

} // FIFE

#endif
