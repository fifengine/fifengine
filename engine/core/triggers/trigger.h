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

		virtual void onTriggerTriggered() = 0;
	};

	// FORWARD REFERENCES

	/**  A one line description of the class.  
	 *
	 * A longer description.
	 *
	 * @see something
	 */
	class Trigger {
	public:

		// LIFECYCLE
		/** Default constructor.
		 */
		Trigger();
		Trigger(const std::string& name) { m_name = name; };

		/** Destructor.
		 */
		~Trigger();

	// OPERATORS

	// OPERATIONS
		void addTriggerListener(ITriggerListener* listener);
		void removeTriggerListener(ITriggerListener* listener);
		
		/** Reset trigger
		 *
		 * Resets the status of the trigger so it can be triggered again.
		 *
		 */
		void reset();

	// ACCESS

		/** Returns if the trigger has been triggered
		 *
		 *  Triggers will only trigger once unless they are reset.
		 * @return bool true if the trigger has been triggered, false
		 * otherwise.
		 */
		bool isTriggered() { return m_triggered; };

		const std::string& getName() const { return m_name; };

	// INQUIRY

	protected:
	private:
		std::string m_name;

		//! true if this trigger has been triggered
		bool m_triggered;

		std::vector<ITriggerListener*> m_triggerListeners;
	};

	// INLINE METHODS
	//

	// EXTERNAL REFERENCES
	//

} //FIFE

#endif
