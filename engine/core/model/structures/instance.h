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

#ifndef FIFE_OBJECT_H
#define FIFE_OBJECT_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/object.h"

#include "location.h"

namespace FIFE {

	class Layer;
	class Action;
	class Instance;
	class ActionInfo;

	class InstanceListener {
	public:
		virtual ~InstanceListener() {};
		virtual void OnActionFinished(Instance* instance, Action* action) = 0;
	};



	/**
	 *  An Instance is an "instantiation" of an Object at a Location.
	 */
	class Instance {
		public:

			/** Constructor
			 * Instances are created by calling addInstance from layer, thus
			 * this method should really be called only by layer or test code
			 */
			Instance(Object* object, const Location& location);

			/** Destructor
			 */
			virtual ~Instance();

			/** Gets object where this instance is instantiated from
			 */
			Object* getObject() { return m_object; }

			/** Gets associated metadata field with given field name
 			 */
			template<typename T>
			const T& get(const std::string& field) { return m_object->oget<T>(field); }

			/** Sets location of the instance
			 *  @param loc new location
 			 */
			void setLocation(const Location& loc) { m_location = loc; }

			/** Gets current location of instance
			 *  @return current location
 			 */
			const Location& getLocation() const { return m_location; }
			
			/** Sets stack position of the instance
			 *  Stack position is used to define the order in which instances residing
			 *  in the same location are drawn
			 *  @param loc new stack position
 			 */
			void setStackPosition(int stackpos) { m_stackpos = stackpos; }
			
			/** Gets current stack position of instance
			 *  @return current stack position
 			 */
			int getStackPosition() { return m_stackpos; }

			/** Gets movement target in case instance is moving. In case not, returns current location
			 *  @return Movement target location
			 */
			const Location& getTargetLocation() const;
			
			/** Adds new instance listener
			 * @param listener to add
 			 */
			void addListener(InstanceListener* listener);

			/** Removes associated instance listener
			 * @param listener to remove
 			 */
			void removeListener(InstanceListener* listener);

			/** Gets the currently active action. This is owned by
			 *  the instance's object, so don't delete it!
			 * @return current action, NULL in case there is none
 			 */
			Action* getCurrentAction() const;

			/** Gets the speed in case instance is moving
			 *  otherwise returns 0
			 * @return instance speed. Value 1 means distance 1 in layer coordinates / second
 			 */
			double getMovementSpeed() const;

			/** Gets the locations where instance is about to be moved next
			 *  using this info with current location gives the direction of instance during movement
			 * @return next locations of movement. In case of no movement, returns NULL
 			 */
			std::vector<Location>* getNextLocations() const;

			/** Returns the direction where instance is heading
			 * @return the direction of instance.
			 */
			const Location& getFacingLocation() const;

			/** Gets the time in milliseconds how long action has been active
			 *  In case there is no current action, returns -1
			 * @return action runtime
 			 */
			int getActionRuntime() const;

			/** Performs given named action to the instance. While performing the action
			 *  moves instance to given target with given speed
			 *  @param action_name name of the action
			 *  @param target place where to move this instance
			 *  @param speed speed used for movement. Units = distance 1 in layer coordinates per second
 			 */
			void act(const std::string& action_name, const Location& target, const double speed);

			/** Performs given named action to the instance. Performs no movement
			 *  @param action_name name of the action
			 *  @param direction coordinates for cell towards instance is heading to when performing the action
			 *  @param repeating in case true, keeps repeating this action
 			 */
			void act(const std::string& action_name, const Location& direction, bool repeating=false);

			/** Returns closest matching static image for given angle
			 * @return id for static image
			 * @see Object::getStaticImageIndexByAngle
			 */
			int getStaticImageIndexByAngle(unsigned int angle);

			/** Updates the instance related to the current action
			 * @param curticks current tick count of the system
 			 */
			void update(unsigned int curticks=0);
			
		private:
			// object where instantiated from
			Object* m_object;
			// current location
			Location m_location;
			// stack position defines the order used when drawing images on same location
			int m_stackpos;
			// action information, allocated when actions are bind
			ActionInfo* m_actioninfo;
			// static image that is read from object. Used for fast access when drawing images
			int m_cached_static_img_id;
			int m_cached_static_img_angle;
			// action information, for pending action. In case objects are moved
			// and some other action comes on top of that, movement must be finalized
			// in such way that instances are stopped to the cell center point
			ActionInfo* m_pending_actioninfo;
			// instance listeners. Not allocated right away to save space
			std::vector<InstanceListener*>* m_listeners;

			Instance(const Instance&);
			Instance& operator=(const Instance&);
			// Finalize current action
			void finalizeAction();
			// Initialize a new action (either pending or current) and return the info for it.
			ActionInfo* initalizeAction(const std::string& action_name);
			// Moves instance. Returns true if finished
			bool move();
			// Calculates movement based current location and speed
			void calcMovement();
	};

} // FIFE

#endif
