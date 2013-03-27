/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

namespace FIFE {

	class ITriggerListener {
	public:
		virtual ~ITriggerListener() {};

		virtual void onTriggered() = 0;
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
	class Trigger {
	public:

	// LIFECYCLE

		/** Default constructor.
		 *
		 * @fixme I'm not sure if I actually want to be able to call
		 * the default constructor.  Triggers should always be given
		 * a name.  Layers should manage the uniqueness of trigger names.
		 */
		Trigger();

		/** Constructor with name
		 *
		 * Triggers should be created with a name as that is how they will
		 * be referred to in the map file.
		 * 
		 */
		Trigger(const std::string& name) { m_name = name; };

		/** Destructor.
		 */
		~Trigger();

	// OPERATORS

	// OPERATIONS

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

	// ACCESS

		/** Gets the name of the trigger.
		 *
		 * @return name of the trigger.
		 */
		const std::string& getName() const { return m_name; };

	// INQUIRY
		/** Returns if the trigger has been triggered
		 *
		 *  Triggers will only trigger once unless they are reset.
		 * @return bool true if the trigger has been triggered, false
		 * otherwise.
		 */
		bool isTriggered() { return m_triggered; };

	protected:
	private:
		//! name of the trigger.  This should be unique per Layer.
		std::string m_name;

		//! true if this trigger has been triggered
		bool m_triggered;

		//! Vector of the listeners that get called
		std::vector<ITriggerListener*> m_triggerListeners;
	};

	// INLINE METHODS
	//

	// EXTERNAL REFERENCES
	//

} //FIFE

#endif
