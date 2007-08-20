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

namespace FIFE { namespace model {

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

			/** Sets instance position to p
 			 */
			void setPosition(const Point& p) { m_location.position = p; }

			/** Gets current instance position
 			 */
			const Point& getPosition() const { return m_location.position; }

			/** Gets current layer where instance is located
 			 */
			Layer* getLayer() const { return m_location.layer; }

			/** Gets current location of instance
 			 */
			const Location& getLocation() const { return m_location; }

			/** Adds new instance listener
 			 */
			void addListener(InstanceListener* listener);

			/** Removes associated instance listener
 			 */
			void removeListener(InstanceListener* listener);

			/** Performs given named action to the instance. While performing the action
			 *  moves instance to given target with given speed
 			 */
			void act(const std::string& action_name, const Location target, const float speed);

			/** Performs given named action to the instance. Performs no movement
 			 */
			void act(const std::string& action_name);

			/** Updates the instance related to current action
 			 */
			void update();

		private:
			// object where instantiated from
			Object* m_object;
			// current location
			Location m_location;
			// static image index, optimization e.g. for tiles
			int m_static_img_ind;
			// action information, allocated when actions are bind
			ActionInfo* m_actioninfo;
			// instance listeners. Not allocated right away to save space
			std::vector<InstanceListener*>* m_listeners;

			Instance(const Instance&);
			Instance& operator=(const Instance&);
			void finalizeAction();
	};

} } // FIFE::model

#endif
