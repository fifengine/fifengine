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

#ifndef FIFE_TRIGGER_H
#define FIFE_TRIGGER_H

// Standard C++ library includes
#include <vector>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"

namespace FIFE {
	class Cell;
	class Layer;
	class Instance;
	class TriggerChangeListener;

	class ITriggerListener {
	public:
		virtual ~ITriggerListener() {};

		virtual void onTriggered() = 0;
	};

	enum TriggerCondition {
		// cell conditions
		CELL_TRIGGER_ENTER = 0,
		CELL_TRIGGER_EXIT,
		CELL_TRIGGER_BLOCKING_CHANGE,
		// instance conditions
		INSTANCE_TRIGGER_LOCATION,
		INSTANCE_TRIGGER_ROTATION,
		INSTANCE_TRIGGER_SPEED,
		INSTANCE_TRIGGER_ACTION,
		INSTANCE_TRIGGER_TIME_MULTIPLIER,
		INSTANCE_TRIGGER_SAYTEXT,
		INSTANCE_TRIGGER_BLOCK,
		INSTANCE_TRIGGER_CELL,
		INSTANCE_TRIGGER_TRANSPARENCY,
		INSTANCE_TRIGGER_VISIBLE,
		INSTANCE_TRIGGER_STACKPOS,
		INSTANCE_TRIGGER_VISUAL,
		INSTANCE_TRIGGER_DELETE
	};

	// FORWARD REFERENCES

	/**  Trigger get triggered when a specific set of criteria are met.
	 *
	 * Currently these can be added directly to Layers.  In order to extend
	 * their use we might consider abstracting them from the Layer and adding
	 * a trigger manager of some sort which will then add the appropriate
	 * listeners to the layer and any other object that might need to
	 * trip a trigger.
	 *
	 * @see Layer
	 */
	class Trigger : public FifeClass {
	public:

	// LIFECYCLE

		/** Default constructor.
		 *
		 * TODO (fixme)
         * I'm not sure if I actually want to be able to call the default constructor.
         * Triggers should always be given a name.
         * The maps trigger controller should guarantee the uniqueness of the name.
		 */
		Trigger();

		/** Constructor with name
		 *
		 * Triggers should be created with a name as that is how they will
		 * be referred to in the map file.
		 *
		 */
		Trigger(const std::string& name);

		/** Destructor.
		 */
		virtual ~Trigger();

		/** Add a listener to the trigger.
		 *
		 * When a trigger gets triggered it will call the onTriggered()
		 * function of the listener.
		 *
		 * The Trigger does NOT take ownership of the listener so clients
		 * must be sure to free their memory after the trigger has been
		 * deleted.
		 *
		 * @see ITriggerListener
		 */
		void addTriggerListener(ITriggerListener* listener);

		/** Removes a listener from the trigger.
		 *
		 * This listener will no longer get called.  The Trigger does
		 * NOT free the listener so you must be sure to do this.
		 */
		void removeTriggerListener(ITriggerListener* listener);

		/** Reset trigger
		 *
		 * Resets the status of the trigger so it can be triggered again.
		 *
		 */
		void reset();

		/** Gets the name of the trigger.
		 *
		 * @return name of the trigger.
		 */
		const std::string& getName() const { return m_name; };

		/** Returns if the trigger has been triggered
		 *
		 *  Triggers will only trigger once unless they are reset.
		 * @return bool true if the trigger has been triggered, false
		 * otherwise.
		 */
		bool isTriggered() { return m_triggered; };

		/** Sets the trigger to triggered and calls ITriggerListener->onTriggered()
		 */
		void setTriggered();

		/** Adds trigger condition.
		 *
		 * @param type The trigger condition.
		 */
		void addTriggerCondition(TriggerCondition type);

		/** Returns trigger conditions in an vector.
		 */
		const std::vector<TriggerCondition>& getTriggerConditions();

		/** Removes trigger condition.
		 *
		 * @param type The trigger condition.
		 */
		void removeTriggerCondition(TriggerCondition type);

		/** Enables trigger for given instance.
		 *
		 * @param instance The instance which is enabled for the trigger.
		 */
		void enableForInstance(Instance* instance);

		/** Returns instance which the trigger is enabled for.
		 */
		const std::vector<Instance*>& getEnabledInstances();

		/** Disables trigger for given instance.
		 *
		 * @param instance The instance which is disabled for the trigger.
		 */
		void disableForInstance(Instance* instance);

		/** Enables trigger for all instances.
		 */
		void enableForAllInstances();

		/** Returns if trigger is enabled for all instances.
		 */
		bool isEnabledForAllInstances();

		/** Disables trigger for all instances.
		 */
		void disableForAllInstances();

		/** Assigns trigger on given layer and position.
		 *
		 * @param layer A pointer to the layer in which to add the Trigger to.
		 * @param pt The ModelCoordinate where the Trigger should be added.
		 */
		void assign(Layer* layer, const ModelCoordinate& pt);

		/** Removes trigger from given layer and position.
		 *
		 * @param layer A pointer to the layer in which to remove the Trigger from.
		 * @param pt The ModelCoordinate where the Trigger should be removed.
		 */
		void remove(Layer* layer, const ModelCoordinate& pt);

		/** Assigns trigger on given cell.
		 *
		 * @param cell A pointer to the cell in which to add the Trigger to.
		 */
		void assign(Cell* cell);

		/** Removes trigger from given cell.
		 *
		 * @param cell A pointer to the cell in which to remove the Trigger from.
		 */
		void remove(Cell* cell);

		/** Returns vector with the cells where the trigger is assigned to.
		 */
		const std::vector<Cell*>& getAssignedCells();

		/** Attaches the trigger to the given instance. So the trigger moves with the instance.
		 *
		 * @param instance A pointer to the instance which the Trigger is attached to.
		 */
		void attach(Instance* instance);

		/** Detaches trigger from instance.
		 */
		void detach();

		/** Returns pointer to instance where the trigger is attached to.
		 * Note: Returns Null if no instance is attached.
		 */
		Instance* getAttached() { return m_attached; }

		/** Callback for TriggerChangeListener.
		 */
		void move();

		/** Moves the trigger from the old position to the new position.
		 *
		 * @param newPos The old position as ModelCoordinate.
		 * @param oldPos The old position as ModelCoordinate.
		 */
		void moveTo(const ModelCoordinate& newPos, const ModelCoordinate& oldPos);

	private:
		//! name of the trigger.  This should be unique per Map.
		std::string m_name;

		//! true if this trigger has been triggered
		bool m_triggered;

		//! true if the trigger is enabled for all instances
		bool m_enabledAll;

		//! Vector of the listeners that get called
		std::vector<ITriggerListener*> m_triggerListeners;

		//! main change listener (cell and instance listener)
		TriggerChangeListener* m_changeListener;
		
		//! cells in which the trigger is assigned
		std::vector<Cell*> m_assigned;

		//! all trigger conditions
		std::vector<TriggerCondition> m_triggerConditions;

		//! all enabled instances
		std::vector<Instance*> m_enabledInstances;

		//! instance where the trigger is attached to
		Instance* m_attached;
	};
} //FIFE

#endif
