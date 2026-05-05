// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_TRIGGER_CONTROLLER_H
#define FIFE_TRIGGER_CONTROLLER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "model/metamodel/modelcoords.h"
#include "util/base/fifeclass.h"
#include "util/structures/rect.h"

namespace FIFE
{
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
    class /*FIFE_API*/ TriggerController : public FifeClass
    {
        public:
            /** Constructor.
             *
             * @param map A pointer to the map that the controller will interact with.
             */
            explicit TriggerController(Map* map);

            /** Destructor.
             */
            virtual ~TriggerController();

            /** Creates a trigger.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             */
            Trigger* createTrigger(std::string const & triggerName);

            /** Creates and adds a trigger to the cell on the specified layer and coordinate.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param layer A pointer to the layer in which to add the Trigger to
             * @param pt The ModelCoordinate where the Trigger should be added.
             */
            Trigger* createTriggerOnCoordinate(
                std::string const & triggerName, Layer* layer, ModelCoordinate const & pt);

            /** Creates and adds a trigger to the cells on the specified layer and coordinates.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param layer A pointer to the layer in which to add the Trigger to
             * @param coords Vector with ModelCoordinates where the Trigger should be added.
             */
            Trigger* createTriggerOnCoordinates(
                std::string const & triggerName, Layer* layer, std::vector<ModelCoordinate> const & coords);

            /** Creates and adds a trigger to the cells on the specified layer and rect.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param layer A pointer to the layer in which to add the Trigger to
             * @param rec a Rect with ModelCoordinates where the Trigger should be added.
             */
            Trigger* createTriggerOnRect(std::string const & triggerName, Layer* layer, Rect const & rec);

            /** Creates and adds a trigger to the cell on the specified location.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param loc The Location where the Trigger should be added.
             */
            Trigger* createTriggerOnLocation(std::string const & triggerName, Location const & loc);

            /** Creates and adds a trigger to the cell on the specified locations.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param locs Vector with Locations where the Trigger should be added.
             */
            Trigger* createTriggerOnLocations(std::string const & triggerName, std::vector<Location> const & locs);

            /** Creates and adds a trigger to the specified cell.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param cell The Cell where the Trigger should be added.
             */
            Trigger* createTriggerOnCell(std::string const & triggerName, Cell* cell);

            /** Creates and adds a trigger to the specified cells.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param cells Vector with Cell pointers where the Trigger should be added.
             */
            Trigger* createTriggerOnCells(std::string const & triggerName, std::vector<Cell*> const & cells);

            /** Creates and attach a trigger to the specified instance. But not to the underlaying cell.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param instance The Instance where the Trigger is attached to.
             */
            Trigger* createTriggerOnInstance(std::string const & triggerName, Instance* instance);

            /** Returns a pointer to the trigger with the given name.
             * Note: Returns Null if the trigger does not exists.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             */
            Trigger* getTrigger(std::string const & triggerName);

            /** Deletes a trigger with the given name.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             */
            void deleteTrigger(std::string const & triggerName);

            /** Removes a trigger from the cell on the specified layer and coordinate.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param layer A pointer to the layer in which to remove the Trigger from.
             * @param pt The ModelCoordinate where the Trigger should be removed.
             */
            void removeTriggerFromCoordinate(std::string const & triggerName, Layer* layer, ModelCoordinate const & pt);

            /** Removes a trigger from the cell on the specified layer and coordinates.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param layer A pointer to the layer in which to remove the Trigger from.
             * @param coords Vector with ModelCoordinates where the Trigger should be removed.
             */
            void removeTriggerFromCoordinates(
                std::string const & triggerName, Layer* layer, std::vector<ModelCoordinate> const & coords);

            /** Removes a trigger from the cell on the specified layer and coordinates.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param layer A pointer to the layer in which to remove the Trigger from.
             * @param rec Rect with ModelCoordinates where the Trigger should be removed.
             */
            void removeTriggerFromRect(std::string const & triggerName, Layer* layer, Rect const & rec);

            /** Removes a trigger from the cell on the specified location.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param loc Location where the Trigger should be removed.
             */
            void removeTriggerFromLocation(std::string const & triggerName, Location const & loc);

            /** Removes a trigger from the cell on the specified locations.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param locs Vector with Locations where the Trigger should be removed.
             */
            void removeTriggerFromLocations(std::string const & triggerName, std::vector<Location> const & locs);

            /** Removes a trigger from the specified cell.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param cell The Cell pointer where the Trigger should be removed.
             */
            void removeTriggerFromCell(std::string const & triggerName, Cell* cell);

            /** Removes a trigger from the specified cells.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param cells Vector with Cell pointers where the Trigger should be removed.
             */
            void removeTriggerFromCells(std::string const & triggerName, std::vector<Cell*> const & cells);

            /** Detach a trigger from the specified instance.
             *
             * @param triggerName The name of the trigger.  This must be unique per map.
             * @param instance The instance pointer where the Trigger is detached from.
             */
            void removeTriggerFromInstance(std::string const & triggerName, Instance* instance);

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
            bool exists(std::string const & name);

            //! Pointer to the map this controller is associated with.
            Map* m_map;

            using TriggerNameMap              = std::map<std::string, Trigger*>;
            using TriggerNameMapIterator      = std::map<std::string, Trigger*>::iterator;
            using TriggerNameMapConstIterator = std::map<std::string, Trigger*>::const_iterator;
            using TriggerNameMapPair          = std::pair<std::string, Trigger*>;
            //! Trigger name map
            TriggerNameMap m_triggerNameMap;
    };
} // namespace FIFE

#endif
