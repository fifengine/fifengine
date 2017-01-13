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

#ifndef FIFE_TRIGGER_CONTROLLER_H
#define FIFE_TRIGGER_CONTROLLER_H

// Standard C++ library includes
#include <vector>
#include <string>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"
#include "util/structures/rect.h"
#include "model/metamodel/modelcoords.h"

namespace FIFE {

	// FORWARD REFERENCES
	class Trigger;
	class Map;
	class Layer;
	class Location;
	class Instance;
	class Cell;

	/** This class serves as a central place to manage triggers for a Map.
	 *
	 *  You should never instantiate this class directly as Map does it
	 *  when you create a new map.
	 */
	class TriggerController : public FifeClass {
	public:

	// LIFECYCLE

		/** Constructor.
		 *
		 * @param map A pointer to the map that the controller will interact with.
		 */
		TriggerController(Map* map);

		/** Destructor.
		 */
		virtual ~TriggerController();

		/** Creates a trigger.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 */
		Trigger* createTrigger(const std::string& triggerName);

		/** Creates and adds a trigger to the cell on the specified layer and coordinate.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param layer A pointer to the layer in which to add the Trigger to
		 * @param pt The ModelCoordinate where the Trigger should be added.
		 */
		Trigger* createTriggerOnCoordinate(const std::string& triggerName, Layer* layer, const ModelCoordinate& pt);

		/** Creates and adds a trigger to the cells on the specified layer and coordinates.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param layer A pointer to the layer in which to add the Trigger to
		 * @param coords Vector with ModelCoordinates where the Trigger should be added.
		 */
		Trigger* createTriggerOnCoordinates(const std::string& triggerName, Layer* layer, const std::vector<ModelCoordinate>& coords);

		/** Creates and adds a trigger to the cells on the specified layer and rect.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param layer A pointer to the layer in which to add the Trigger to
		 * @param rec a Rect with ModelCoordinates where the Trigger should be added.
		 */
		Trigger* createTriggerOnRect(const std::string& triggerName, Layer* layer, const Rect& rec);

		/** Creates and adds a trigger to the cell on the specified location.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param loc The Location where the Trigger should be added.
		 */
		Trigger* createTriggerOnLocation(const std::string& triggerName, const Location& loc);

		/** Creates and adds a trigger to the cell on the specified locations.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param locs Vector with Locations where the Trigger should be added.
		 */
		Trigger* createTriggerOnLocations(const std::string& triggerName, const std::vector<Location>& locs);

		/** Creates and adds a trigger to the specified cell.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param cell The Cell where the Trigger should be added.
		 */
		Trigger* createTriggerOnCell(const std::string& triggerName, Cell* cell);

		/** Creates and adds a trigger to the specified cells.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param cells Vector with Cell pointers where the Trigger should be added.
		 */
		Trigger* createTriggerOnCells(const std::string& triggerName, const std::vector<Cell*>& cells);

		/** Creates and attach a trigger to the specified instance. But not to the underlaying cell.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param instance The Instance where the Trigger is attached to.
		 */
		Trigger* createTriggerOnInstance(const std::string& triggerName, Instance* instance);

		/** Returns a pointer to the trigger with the given name.
		 * Note: Returns Null if the trigger does not exists.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 */
		Trigger* getTrigger(const std::string& triggerName);
		
		/** Deletes a trigger with the given name.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 */
		void deleteTrigger(const std::string& triggerName);

		/** Removes a trigger from the cell on the specified layer and coordinate.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param layer A pointer to the layer in which to remove the Trigger from.
		 * @param pt The ModelCoordinate where the Trigger should be removed.
		 */
		void removeTriggerFromCoordinate(const std::string& triggerName, Layer* layer, const ModelCoordinate& pt);

		/** Removes a trigger from the cell on the specified layer and coordinates.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param layer A pointer to the layer in which to remove the Trigger from.
		 * @param coords Vector with ModelCoordinates where the Trigger should be removed.
		 */
		void removeTriggerFromCoordinates(const std::string& triggerName, Layer* layer, const std::vector<ModelCoordinate>& coords);

		/** Removes a trigger from the cell on the specified layer and coordinates.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param layer A pointer to the layer in which to remove the Trigger from.
		 * @param rec Rect with ModelCoordinates where the Trigger should be removed.
		 */
		void removeTriggerFromRect(const std::string& triggerName, Layer* layer, const Rect& rec);

		/** Removes a trigger from the cell on the specified location.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param loc Location where the Trigger should be removed.
		 */
		void removeTriggerFromLocation(const std::string& triggerName, const Location& loc);

		/** Removes a trigger from the cell on the specified locations.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param locs Vector with Locations where the Trigger should be removed.
		 */
		void removeTriggerFromLocations(const std::string& triggerName, const std::vector<Location>& locs);

		/** Removes a trigger from the specified cell.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param cell The Cell pointer where the Trigger should be removed.
		 */
		void removeTriggerFromCell(const std::string& triggerName, Cell* cell);

		/** Removes a trigger from the specified cells.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param cells Vector with Cell pointers where the Trigger should be removed.
		 */
		void removeTriggerFromCells(const std::string& triggerName, const std::vector<Cell*>& cells);

		/** Detach a trigger from the specified instance.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param instance The instance pointer where the Trigger is detached from.
		 */
		void removeTriggerFromInstance(const std::string& triggerName, Instance* instance);

		/** Returns a vector with all trigger pointers.
		 */
		std::vector<Trigger*> getAllTriggers();

		/** Returns a vector with all trigger names.
		 */
		std::vector<std::string> getAllTriggerNames();

	private:
		/** Checks to see if the trigger name already exists.
		 *
		 * @param name The name of the trigger to check
		 *
		 * @return true if the name already exists, false otherwise.
		 */
		bool exists(const std::string& name);

		//! Pointer to the map this controller is associated with.
		Map* m_map;

		typedef std::map< std::string, Trigger* > TriggerNameMap;
		typedef std::map< std::string, Trigger*  >::iterator TriggerNameMapIterator;
		typedef std::map< std::string, Trigger*  >::const_iterator TriggerNameMapConstIterator;
		typedef std::pair< std::string, Trigger*  > TriggerNameMapPair;

		//! Trigger name map
		TriggerNameMap m_triggerNameMap;
	};
} //FIFE

#endif
