/***************************************************************************
 *   Copyright (C) 2006-2012 by the FIFE team                              *
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

#ifndef FIFE_PROTOTYPE_H
#define FIFE_PROTOTYPE_H

// Standard C++ library includes
#include <string>
#include <map>
#include <list>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/resource/resource.h"
#include "util/math/angles.h"

namespace FIFE {

	class Action;
	class IPather;
	class IVisual;

	/** Object class
	 *
	 * Objects describe the properties of objects.
	 * Objects may inherit default values from another object.
	 *
	 */
	class Object {
	public:
		/** Constructor
		 * An object may optionally inherit default attributes
		 * from another object. This object may override these
		 * defaults, but it may not CHANGE the inherited values.
		 *
		 * Objects are created by calling Model::createObject, thus
		 * this method should really be called only by Model or test code
		 * @see Model in model/model.h for creation of objects.
		 */
		Object(const std::string& identifier, const std::string& name_space, Object* inherited=NULL);

		/** Destructor
		 */
		~Object();

		const std::string& getId() const { return m_id; }
		const std::string& getNamespace() const { return m_namespace; }

		/** Sets the identifier for this object.
		 */
		void setId(const std::string& id) { m_id = id; }

		/** Adds new action with given id. In case there is action already
		 *  with given id, returns it instead of new object
		 *  Action instances are managed by object
		 * @param identifier An identifier or name to give the action.
		 * @param is_default if true, becomes default action for this object
		 * 	default objects are used e.g. when showing them on editor.
		 *      if multiple default actions are created, last one remains.
		 *      In case there's no explicit default action created, first
		 *      action created becomes the default
		 */
		Action* createAction(const std::string& identifier, bool is_default=false);

		/** Gets action with given id. If not found, returns NULL
		 */
		Action* getAction(const std::string& identifier, bool deepsearch = true) const;

		/** Gets all available action ids of the object and packs them into a list
		 */
		std::list<std::string> getActionIds() const;

		/** Sets default action assigned to this object. If not available, then default action is not changed.
		 */
		void setDefaultAction(const std::string& identifier);

		/** Gets default action assigned to this object. If none available, returns NULL
		 */
		Action* getDefaultAction() const;

		/** Sets pather used by instances created out of this object
		 */
		void setPather(IPather* pather);

		/** Gets associated pather
		 */
		IPather* getPather() const;

		/** Gets an object where this object was inherited from
		 * @see inherited object
		 */
		Object* getInherited() const;

		/** Sets visualization to be used. Transfers ownership.
		 */
		void adoptVisual(IVisual* visual);

		/** Gets used visualization
		 */
		template<typename T> T* getVisual() const { return reinterpret_cast<T*>(m_visual); }

		/** Sets if object blocks movement
		 */
		void setBlocking(bool blocking);

		/** Gets if object blocks movement
		 */
		bool isBlocking() const;

		/** Set to true, if object is such that it doesn't move
		 */
		void setStatic(bool stat);

		/** Gets if object moves
		 */
		bool isStatic() const;

		void setFilename(const std::string& file);
		const std::string& getFilename() const;

		/** Sets the cell stack position.
		 *  Is used to determine which Instance is on top of a cell.
		 * @param position The stack position on a cell, range 0-255.
		 */
		void setCellStackPosition(uint8_t position);

		/** Returns cell stack position.
		 * @return The stack position on a cell, range 0-255.
		 */
		uint8_t getCellStackPosition() const;

		/** Gets if object uses special cost.
		 * @return A boolean, true if the object uses special cost, otherwise false.
		 */
		bool isSpecialCost() const;

		/** Sets the cost id.
		 * @param cost A const reference to a string which contains the identifier.
		 */
		void setCostId(const std::string& cost);

		/** Returns the cost id.
		 * @return A const reference to a string which contains the identifier.
		 */
		std::string getCostId() const;

		/** Sets the cost.
		 * @param cost A double which value is used as cost.
		 */
		void setCost(double cost);

		/** Returns the cost.
		 * @return A double which value is used as cost.
		 */
		double getCost() const;

		/** Gets if object uses special speed modifier.
		 * @return A boolean, true if the object uses special speed, otherwise false.
		 */
		bool isSpecialSpeed() const;

		/** Sets the speed modifier.
		 * @param cost A double which value is used as speed multiplier.
		 */
		void setSpeed(double cost);

		/** Returns the speed modifier.
		 * @return A double which value is used as speed multiplier.
		 */
		double getSpeed() const;

		/** Gets if object uses special cost.
		 * @return A boolean, true if the object uses special cost, otherwise false.
		 */
		bool isMultiObject() const;

		/** Adds a multi part identifier.
		 * @param partId A const reference to a string that holds the identifier.
		 */
		void addMultiPartId(const std::string& partId);

		/** Returns all multi part identifiers.
		 * @return A const reference to a list that holds the identifiers.
		 */
		std::list<std::string> getMultiPartIds() const;

		/** Removes a multi part identifier.
		 * @param partId A const reference to a string that holds the identifier.
		 */
		void removeMultiPartId(const std::string& partId);

		/** Removes all multi part identifiers.
		 */
		void removeAllMultiPartIds();

		/** Gets if object is a part of a multi object.
		 * @return A boolean, true if the object is a part of a multi object, otherwise false.
		 */
		bool isMultiPart() const;

		/** Sets the object as a part of a multi object.
		 * @param part A boolean, true if the object is a part of a multi object, otherwise false.
		 */
		void setMultiPart(bool part);

		/** Adds a object as a part of a multi object.
		 * @param obj A pointer to the multi part object.
		 */
		void addMultiPart(Object* obj);

		/** Returns all multi part objects.
		 * @return A const reference to a set that holds the objects.
		 */
		std::set<Object*> getMultiParts() const;

		/** Removes a multi part object.
		 * @param obj A pointer to the part object.
		 */
		void removeMultiPart(Object* obj);

		/** Removes all multi part objects.
		 */
		void removeMultiParts();

		/** Adds rotationally dependent coordinates for this object part.
		 * @param rotation A integer value for the angle.
		 * @param coord A ModelCoordinate as relative coordinate, 0,0 is always the multi object center.
		 */
		void addMultiPartCoordinate(int32_t rotation, ModelCoordinate coord);

		/** Returns all rotationally dependent coordinates from this object part.
		 * @return A const reference to a multimap which contains the coordinates per rotation.
		 */
		std::multimap<int32_t, ModelCoordinate> getMultiPartCoordinates() const;

		/** Returns all object part coordinates for the given rotation.
		 * @param rotation A integer value for the angle.
		 * @return A vector which contains the coordinates.
		 */
		std::vector<ModelCoordinate> getMultiPartCoordinates(int32_t rotation) const;

		/** Returns all multi object coordinates for the given rotation.
		 * @param rotation A integer value for the angle.
		 * @return A vector which contains the coordinates.
		 */
		std::vector<ModelCoordinate> getMultiObjectCoordinates(int32_t rotation) const;

		/** Sets the rotation anchor for this multi object.
		 * Is used to rotate the images from multi part objects around this relative point,
		 * default is 0.0, 0.0 the center of the multi object.
		 * @param anchor A const reference to a ExactModelCoordinate that holds the anchor coordinate.
		 */
		void setRotationAnchor(const ExactModelCoordinate& anchor);
		
		/** Returns the rotation anchor for this multi object.
		 * @return A const reference to a ExactModelCoordinate that holds the anchor coordinate.
		 */
		ExactModelCoordinate getRotationAnchor() const;

		/** Sets the rotation to restricted.
		 * If this is enabled the multi object uses only rotation values are which based on multi coordinates.
		 * @param restrict A boolean, if true the rotation will be restricted, false for free rotation.
		 */
		void setRestrictedRotation(bool restrict);

		/** Gets if object uses restricted rotations.
		 * @return A boolean, true if the object uses restricted rotations, otherwise false.
		 */
		bool isRestrictedRotation() const;

		/** Returns the most obvious rotation, based on multi coordinates.
		 * @param rotation A integer value for the original angle.
		 * @return A integer value for the obvious rotation.
		 */
		int32_t getRestrictedRotation(int32_t rotation);

		/** Sets z-step range for object.
		 *  0 means it can not climb, with a value of 1 it can climb a z-height of 1 and so on.
		 * @param zRange The z-step range as int.
		 */
		void setZStepRange(int32_t zRange);

		/** Returns z-step range from object. In case it is not limited -1 is returned.
		 * @return The z-step range as int.
		 */
		int32_t getZStepRange() const;

		/** Sets the area id that the instances of this object adds to their cells.
		 * @param id The area id, default is "".
		 */
		void setArea(const std::string& id);

		/** Gets the area id that the instances of this object adds to their cells.
		 * @return The area id, default is "".
		 */
		std::string getArea() const;

		/** Adds an area id to walkable area. The instances of this object
		 *  can only walk on cells that part of the given areas.
		 * @param id The area id.
		 */
		void addWalkableArea(const std::string& id);

		/** Removes an area id from walkable areas.
		 * @param id The area id.
		 */
		void removeWalkableArea(const std::string& id);

		/** Returns a list that contains all walkable area ids.
		 * @return A list that contains all walkable area ids as strings.
		 */
		std::list<std::string> getWalkableAreas() const;

		/** Compares equality of two objects
		 */
		bool operator==(const Object& obj) const;

		/** Compares unequality of two objects
		 */
		bool operator!=(const Object& obj) const;

	private:
		//! identifier
		std::string m_id;

		//! namespace
		std::string m_namespace;
		
		//! filename
		std::string m_filename;

		//! pointer to inherited object
		Object* m_inherited;

		//! pointer to object visual
		IVisual* m_visual;

		class BasicObjectProperty {
		public:
			//! Constructor
			BasicObjectProperty();
		
			//! Destructor
			~BasicObjectProperty();

			//! area id
			std::string m_area;

			//! holds action ids and assigned actions
			std::map<std::string, Action*>* m_actions;

			//! pointer to default action
			Action* m_defaultAction;

			//! indicates if object blocks
			bool m_blocking;

			//remove this with a if (MovableObjectProperty)
			//! indicates if object is static
			bool m_static;

			//! position on cellstack
			uint8_t m_cellStack;
		};

		class MovableObjectProperty {
		public:
			//! Constructor
			MovableObjectProperty();
		
			//! Destructor
			~MovableObjectProperty();

			//! pointer to pathfinder
			IPather* m_pather;

			//! cost identifier
			std::string m_costId;

			//! cost value, default 1.0
			double m_cost;

			//! speed modifier, default 1.0
			double m_speed;

			//! z range value
			int32_t m_zRange;
		
			//! list contains walkable area ids
			std::list<std::string> m_walkableAreas;
		};

		class MultiObjectProperty {
		public:
			//! Constructor
			MultiObjectProperty();
		
			//! Destructor
			~MultiObjectProperty();

			//! indicates if object is part of multi object
			bool m_multiPart;

			//! indicates if object uses only restricted rotations
			bool m_restrictedRotation;

			//! list with part identifiers
			std::list<std::string> m_multiPartIds;

			//! rotation anchor
			ExactModelCoordinate m_rotationAnchor;

			//! set contains part objects
			std::set<Object*> m_multiParts;

			//! part object angles
			type_angle2id m_partAngleMap;

			//! multi object angles
			type_angle2id m_multiAngleMap;

			//! part object coordinates
			std::multimap<int32_t, ModelCoordinate> m_multiPartCoordinates;

			//! multi object coordinates
			std::multimap<int32_t, ModelCoordinate> m_multiObjectCoordinates;
		};

		BasicObjectProperty* m_basicProperty;
		MovableObjectProperty* m_moveProperty;
		MultiObjectProperty* m_multiProperty;
	};

} //FIFE
#endif

