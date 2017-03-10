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

#ifndef FIFE_INSTANCE_H
#define FIFE_INSTANCE_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"

#include "model/metamodel/object.h"
#include "model/metamodel/ivisual.h"
#include "view/visual.h"

#include "location.h"


namespace FIFE {

	class Layer;
	class Action;
	class Instance;
	class ActionInfo;
	class SayInfo;
	class TimeProvider;
	class Route;

	class InstanceActionListener {
	public:
		virtual ~InstanceActionListener() {};
		virtual void onInstanceActionFinished(Instance* instance, Action* action) = 0;
		virtual void onInstanceActionCancelled(Instance* instance, Action* action) = 0;
		virtual void onInstanceActionFrame(Instance* instance, Action* action, int32_t frame) = 0;
	};

	enum InstanceChangeType {
		ICHANGE_NO_CHANGES = 0x0000,
		ICHANGE_LOC = 0x0001,
		ICHANGE_ROTATION = 0x0002,
		ICHANGE_SPEED = 0x0004,
		ICHANGE_ACTION = 0x0008,
		ICHANGE_TIME_MULTIPLIER = 0x0010,
		ICHANGE_SAYTEXT = 0x0020,
		ICHANGE_BLOCK = 0x0040,
		ICHANGE_CELL = 0x0080,
		ICHANGE_TRANSPARENCY = 0x0100,
		ICHANGE_VISIBLE = 0x0200,
		ICHANGE_STACKPOS = 0x0400,
		ICHANGE_VISUAL = 0x0800
	};
	typedef uint32_t InstanceChangeInfo;

	class InstanceChangeListener {
	public:
		virtual ~InstanceChangeListener() {};
		virtual void onInstanceChanged(Instance* instance, InstanceChangeInfo info) = 0;
	};

	class InstanceDeleteListener {
	public:
		virtual ~InstanceDeleteListener() {};
		virtual void onInstanceDeleted(Instance* instance) =0;
	};

	enum VisitorShapeType {
		ITYPE_NO_SHAPE = 0,
		ITYPE_QUAD_SHAPE,
		ITYPE_CIRCLE_SHAPE
	};
	typedef uint8_t VisitorShapeInfo;

	/** An Instance is an "instantiation" of an Object at a Location.
	 *
	 */
	class Instance : public FifeClass, public InstanceDeleteListener {
	public:

		/** Constructor
		 * Instances are created by calling addInstance from layer, thus
		 * this method should really be called only by layer or test code
		 */
		Instance(Object* object, const Location& location, const std::string& identifier="");

		/** Destructor
		 */
		virtual ~Instance();

		/** Get the identifier for this instance; possibly null.
		 */
		const std::string& getId();

		/** Set the identifier for this instance.
		 */
		void setId(const std::string& identifier="");

		/** Gets object where this instance is instantiated from
		 */
		Object* getObject();

		/** Sets location of the instance
		 *  @param loc new location
		 */
		void setLocation(const Location& loc);

		/** Gets current location of instance
		 *  @note does not return const Location&, since swig wont be const correct
		 *  @return current location
		 */
		Location getLocation() const;

		/** Gets reference of current location of instance
		 *  @return reference to current location
		 */
		Location& getLocationRef();

		/** Gets movement target in case instance is moving. In case not, returns current location
		 *  To move target location, call move-method
		 *  @see move
		 *  @note does not return const Location&, since swig wont be const correct
		 *  @return Movement target location
		 */
		Location getTargetLocation() const;

		/** Sets the direction where instance is heading. Useful e.g. with static
		 * instances which don't "move" or "act"
		 */
		void setFacingLocation(const Location& loc);

		/** Returns the direction where instance is heading
		*  @note does not return const Location&, since swig wont be const correct
		 * @return the direction of instance.
		 */
		Location getFacingLocation();

		/** Gets reference of old location of instance
		 *  @return reference to old location
		 */
		Location& getOldLocationRef();

		/** Set the rotation offset of this instance
		 */
		void setRotation(int32_t rotation);

		/** Get the rotation offset of this instance
		 *  Returns direction where instance is heading
		 */
		int32_t getRotation() const;

		/** Get the old rotation offset of this instance
		 *  Returns direction where instance was heading
		 */
		int32_t getOldRotation() const;

		/** Sets if instance blocks movement
		 */
		void setBlocking(bool blocking);

		/** Gets if instance blocks movement
		 */
		bool isBlocking() const;

		/** Sets if instance blocking can overriden
		 */
		void setOverrideBlocking(bool overblock);

		/** Gets if instance blocking can overriden
		 */
		bool isOverrideBlocking() const;

		/** Auxiliary function to inform ActionListeners about the active ActionFrame.
		 *  @param action pointer to the action
		 *  @param frame frame index number of the animation
		 */
		void callOnActionFrame(Action* action, int32_t frame);

		/** Adds new instance action listener
		 * @param listener to add
		 */
		void addActionListener(InstanceActionListener* listener);

		/** Removes associated instance action listener
		 * @param listener to remove
		 */
		void removeActionListener(InstanceActionListener* listener);

		/** Adds new instance change listener
		 * @param listener to add
		 */
		void addChangeListener(InstanceChangeListener* listener);

		/** Removes associated instance change listener
		 * @param listener to remove
		 */
		void removeChangeListener(InstanceChangeListener* listener);

		/** Adds new instance delete listener
		 * @param listener to add
		 */
		void addDeleteListener(InstanceDeleteListener* listener);

		/** Removes associated instance delete listener
		 * @param listener to remove
		 */
		void removeDeleteListener(InstanceDeleteListener* listener);

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

		/** Gets the time in milliseconds how long action has been active
		 *  In case there is no current action, returns -1
		 * @return action runtime
		 */
		uint32_t getActionRuntime();

		/** Sets the time in milliseconds how long an action has been active
		*  This was requested in Ticket #373.  This way the state
		*  of the action can be saved and restored at a later time
		*  @param time_offset The action time offset that should be applied
		*/
		void setActionRuntime(uint32_t time_offset);

		/** Performs given named action to the instance. While performing the action
		 *  moves instance to given target with given speed
		 *  @param actionName name of the action
		 *  @param target place where to move this instance
		 *  @param speed speed used for movement. Units = distance 1 in layer coordinates per second
		 *  @param costId id for special costs which is be used as extra multiplier.
		 */
		void move(const std::string& actionName, const Location& target, const double speed, const std::string& costId = "");

		/** Performs given named action to the instance, once only. Performs no movement
		 *  @param actionName name of the action
		 *  @param direction coordinates for cell towards instance is heading to when performing the action
		 */
		void actOnce(const std::string& actionName, const Location& direction);

		/** Performs given named action to the instance, once only. Performs no movement
		 *  @param actionName name of the action
		 *  @param rotation rotation which the instance use when performing the action
		 */
		void actOnce(const std::string& actionName, int32_t rotation);

		/** Performs given named action to the instance, once only. Performs no movement and use current rotation
		 *  @param actionName name of the action
		 */
		void actOnce(const std::string& actionName);

		/** Performs given named action to the instance, repeated. Performs no movement
		 *  @param actionName name of the action
		 *  @param direction coordinates for cell towards instance is heading to when performing the action
		 */
		void actRepeat(const std::string& actionName, const Location& direction);

		/** Performs given named action to the instance, repeated Performs no movement
		 *  @param actionName name of the action
		 *  @param rotation rotation which the instance use when performing the action
		 */
		void actRepeat(const std::string& actionName, int32_t rotation);

		/** Performs given named action to the instance, repeated. Performs no movement and use current rotation
		 *  @param actionName name of the action
		 */
		void actRepeat(const std::string& actionName);

		/** Causes instance to "say" given text (shown on screen next to the instance)
		 *  @param text text to say. If "" given, clear the text
		 *  @param duration duration to show the text (in ms). If 0, shows forever
		 */
		void say(const std::string& text, uint32_t duration=0);

		/** Performs given named action to the instance. While performing the action
		 *  follows given instance with given speed
		 *  @param actionName name of the action
		 *  @param leader followed instance
		 *  @param speed speed used for movement. Units = distance 1 in layer coordinates per second
		 */
		void follow(const std::string& actionName, Instance* leader, const double speed);

		/** Performs given named action to the instance. While performing the action
		 *  follows given route with given speed. Note: In this case route isn't deleted or resetted at the end.
		 *  @param actionName name of the action
		 *  @param route followed route
		 *  @param speed speed used for movement. Units = distance 1 in layer coordinates per second
		 */
		void follow(const std::string& actionName, Route* route, const double speed);

		/** Cancel movement after a given length.
		 *  If no length is set then 1 is used. This means that the instance
		 *  stops at the center of the next cell (can be the same as the current).
		 */
		void cancelMovement(uint32_t length = 1);

		/** Returns pointer to currently set saytext. In case no text is set, returns NULL
		 */
		const std::string* getSayText() const;

		/** Updates the instance related to the current action
		 * @note call this only once in engine update cycle, so that tracking between
		 *  current position and previous position keeps in sync.
		 * @returns marked changes
		 */
		InstanceChangeInfo update();

		/** If this returns true, the instance needs to be updated
		 */
		bool isActive() const;

		/** Sets visualization to be used. Transfers ownership.
		 */
		void setVisual(IVisual* visual) { m_visual = visual; }

		/** Gets used visualization
		 */
		template<typename T> T* getVisual() const { return reinterpret_cast<T*>(m_visual); }

		void callOnTransparencyChange();
		void callOnVisibleChange();
		void callOnStackPositionChange();

		/** Sets speed for the map. See Model::setTimeMultiplier.
		*/
		void setTimeMultiplier(float multip);

		/** Gets instance speed. @see setTimeMultiplier.
		*/
		float getTimeMultiplier();

		/** Gets instance speed, considering also model and map speeds. @see setTimeMultiplier.
		*/
		float getTotalTimeMultiplier();

		/** Gets the scaled runtime in milliseconds
		 * @return runtime
		 */
		uint32_t getRuntime();

		/** Refreshes instance e.g. in case location is updated directly (not via setLocation)
		 * In this case e.g. instance's master time provider is changed, so it needs to be updated
		 */
		void refresh();

		/** Returns a bitmask of changes of the last update.
		 */
		InstanceChangeInfo getChangeInfo();

		/** callback so other instances we depend on can notify us if they go away
		*/
		void onInstanceDeleted(Instance* instance);

		/** Returns a pointer to the route, in case there is no, it returns NULL.
		 */
		Route* getRoute();

		/** Marks this instance as a visitor.
		 */
		void setVisitor(bool visit);

		/** If instance is a visitor it returns true otherwise false.
		 */
		bool isVisitor();

		/** Sets the shape type for a visitor.
		*/
		void setVisitorShape(VisitorShapeInfo info);

		/** Gets the shape type for a visitor.
		*/
		VisitorShapeInfo getVisitorShape();

		/** Sets the range for a visitor.
		*/
		void setVisitorRadius(uint16_t radius);

		/** Gets the visitor range.
		*/
		uint16_t getVisitorRadius();

		/** Sets the cell stack position.
		*/
		void setCellStackPosition(uint8_t stack);

		/** Gets the cell stack position.
		*/
		uint8_t getCellStackPosition();

		/** Returns true if instance or object have special cost otherwise false.
		 */
		bool isSpecialCost();

		/** Sets for the given cost id a cost.
		*  @param id name of the cost id
		*  @param cost value for the cost
		*/
		void setCost(const std::string& id, double cost);

		/** Resets cost.
		 */
		void resetCost();

		/** Returns cost value. In case there is no it returns the object cost.
		 */
		double getCost();

		/** Returns cost id. In case there is no it returns the object cost id.
		 */
		std::string getCostId();

		/** Returns speed modifier.
		 */
		double getSpeed();

		/** Returns true if instance or object have special speed modifier otherwise false.
		 */
		bool isSpecialSpeed();

		/** Returns true if it is multi cell otherwise false
		 */
		bool isMultiCell();

		/** Returns true if it is multi object otherwise false
		 */
		bool isMultiObject();

		/** Updates the visual positions of all instances in case this is a multi object
		 */
		void updateMultiInstances();

		/** Returns a vector that contains all instances of a multi object
		 */
		const std::vector<Instance*>& getMultiInstances();

		/** Sets a instance to the main multi instance of this instance.
		 */
		void setMainMultiInstance(Instance* main);

		/** Returns a pointer to the main mulit instance or Null if the instance is not part of a multi instance object.
		 */
		Instance* getMainMultiInstance();

		/** Adds new static color overlay with given angle (degrees).
		 */
		void addStaticColorOverlay(uint32_t angle, const OverlayColors& colors);

		/** Returns closest matching static color overlay for given angle
		 * @return pointer to OverlayColor class
		 */
		OverlayColors* getStaticColorOverlay(int32_t angle);

		/** Removes a static color overlay with given angle (degrees).
		 */
		void removeStaticColorOverlay(int32_t angle);

		/** Indicates if there exists a static color overlay.
		 */
		bool isStaticColorOverlay();

		/** Adds new color overlay with given angle (degrees) to given action.
		 */
		void addColorOverlay(const std::string& actionName, uint32_t angle, const OverlayColors& colors);

		/** Returns closest matching color overlay for given angle and action.
		 * @return pointer to OverlayColor class
		 */
		OverlayColors* getColorOverlay(const std::string& actionName, uint32_t angle);

		/** Removes a color overlay with given angle (degrees) from given action.
		 */
		void removeColorOverlay(const std::string& actionName, int32_t angle);

		/** Adds new animation overlay with given angle (degrees) and order to given action.
		 */
		void addAnimationOverlay(const std::string& actionName, uint32_t angle, int32_t order, const AnimationPtr& animationptr);

		/** Gets map with animations closest to given angle.
		 * @return ordered animation map
		 */
		std::map<int32_t, AnimationPtr> getAnimationOverlay(const std::string& actionName, int32_t angle);

		/** Removes animation overlay with given angle (degrees) and order from action.
		 */
		void removeAnimationOverlay(const std::string& actionName, uint32_t angle, int32_t order);

		/** Adds new color overlay with given angle (degrees) and order to given action animation overlay.
		 */
		void addColorOverlay(const std::string& actionName, uint32_t angle, int32_t order, const OverlayColors& colors);

		/** Returns closest matching color overlay for given angle, order and action animation overlay.
		 * @return pointer to OverlayColor class
		 */
		OverlayColors* getColorOverlay(const std::string& actionName, uint32_t angle, int32_t order);

		/** Removes a color overlay with given angle (degrees), order from given action animation overlay.
		 */
		void removeColorOverlay(const std::string& actionName, int32_t angle, int32_t order);

		/** If the action have base animation and optional color overlay it gets converted to animation overlay.
		 */
		void convertToOverlays(const std::string& actionName, bool color);

		/** Indicates if there exists a animation overlay for given action.
		 */
		bool isAnimationOverlay(const std::string& actionName);

		/** Indicates if there exists a color overlay for given action or animation overlay.
		 */
		bool isColorOverlay(const std::string& actionName);
		
	private:
		std::string m_id;

		//! The rotation offset of this instance. This is in addition to possible camera rotation and
		//! intended for setting, for example, a rotation of a tile.
		int32_t m_rotation;

		/** InstanceActivity gets allocated in case there is some runtime
		 * activity related to the instance. Keeping activity related variables
		 * in separate class keeps memory consumption lower e.g. for large tile
		 * areas.
		 * Class also keeps track of changes since the previous update call.
		 * With this bookkeeping, it is possible to optimize several spots in
		 * the engine, basically only reacting to changes instead of polling.
		 */
		class InstanceActivity {
		public:
			InstanceActivity(Instance& source);
			~InstanceActivity();

			// ----- Fields related to change tracking -----
			//! updates cached variables, marks changes
			void update(Instance& source);
			//! location on previous round
			Location m_location;
			//! location on previous cell
			Location m_oldLocation;
			//! rotation on previous round
			int32_t m_rotation;
			//! rotation on previous round
			int32_t m_oldRotation;
			//! action on previous round. NOTE: might become invalid, only used for address comparison
			Action* m_action;
			//! speed on previous round
			double m_speed;
			//! time multiplier on previous round
			float m_timeMultiplier;
			//! say text on previous round
			std::string m_sayText;
			//! listeners for changes
			std::vector<InstanceChangeListener*> m_changeListeners;

			// ----- Fields related to generic activity -----
			//! listeners for action related events
			std::vector<InstanceActionListener*> m_actionListeners;
			//! action information, allocated when actions are bind
			ActionInfo* m_actionInfo;
			//! text to say + duration, allocated when something is said
			SayInfo* m_sayInfo;
			//! time scaler for this instance
			TimeProvider* m_timeProvider;
			//! blocking status on previous round
			bool m_blocking;
			//! additional change info, used for visual class (transparency, visible, stackpos)
			InstanceChangeInfo m_additional;
		};
		InstanceActivity* m_activity;
		//! bitmask stating current changes
		InstanceChangeInfo m_changeInfo;
		//! listeners for deletion of the instance
		std::vector<InstanceDeleteListener*> m_deleteListeners;

		//! object where instantiated from
		Object* m_object;
		//! indicates if m_object is customized
		bool m_ownObject;
		//! current location
		Location m_location;
		//! instance visualization
		IVisual* m_visual;
		//! instance blocking info
		bool m_blocking;
		//! allow to override the blocking property
		bool m_overrideBlocking;
		//! is instance a visitor (FoW)
		bool m_isVisitor;
		//! visitor shape type
		VisitorShapeInfo m_visitorShape;
		//! visitor radius (FoW)
		uint16_t m_visitorRadius;
		//! position on cell stack
		uint8_t m_cellStackPos;
		//! indicates special cost
		bool m_specialCost;
		//! holds cost value
		double m_cost;
		//! holds cost id
		std::string m_costId;
		//! vector that holds all multi instances
		std::vector<Instance*> m_multiInstances;
		//! pointer to the main multi instance
		Instance* m_mainMultiInstance;

		Instance(const Instance&);
		Instance& operator=(const Instance&);
		//! Finalize current action
		void finalizeAction();
		//! Cancel current action
		void cancelAction();
		//! Initialize action for use
		void initializeAction(const std::string& actionName);
		//! Moves instance. Returns true if finished
		bool processMovement();
		//! Calculates movement based current location and speed
		void calcMovement();
		//! rebinds time provider based on new location
		void bindTimeProvider();
		//! called when instance has been changed. Causes instance to create InstanceActivity
		void initializeChanges();
		//! called to prepare the instance for an update
		void prepareForUpdate();

		//! Creates an own object for the instance to allow visual customization.
		void createOwnObject();
		//! Returns pointer to action visual, can also create it.
		ActionVisual* getActionVisual(const std::string& actionName, bool create);
	};
} // FIFE

#endif
