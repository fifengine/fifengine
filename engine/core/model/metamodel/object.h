/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "util/base/resourceclass.h"
#include "util/math/angles.h"

namespace FIFE {

	class Action;
	class AbstractPather;
	class AbstractVisual;

	/** Object class
	 *
	 * Objects describe the properties of objects.
	 * Objects may inherit default values from another object.
	 *
	 */
	class Object : public ResourceClass {
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
		 * @param is_default if true, becomes default action for this object
		 * 	default objects are used e.g. when showing them on editor.
		 *      if multiple default actions are created, last one remains.
		 *      In case there's no explicit default action created, first
		 *      action created becomes the default
		 */
		Action* createAction(const std::string& identifier, bool is_default=false);

		/** Gets action with given id. If not found, returns NULL
		 */
		Action* getAction(const std::string& identifier) const;

		/** Gets all available action ids of the object and packs them into a list
		 */
		std::list<std::string> getActionIds() const;

		/** Gets default action assigned to this object. If none available, returns NULL
		 */
		Action* getDefaultAction() const { return m_defaultaction; }
		
		/** Sets pather used by instances created out of this object
		 */
		void setPather(AbstractPather* pather);

		/** Gets associated pather
		 */
		AbstractPather* getPather() const { return m_pather; }

		/** Gets an object where this object was inherited from
		 * @see inherited object
		 */
		Object* getInherited() const { return m_inherited; }

		/** Sets visualization to be used. Transfers ownership.
		 */
		void adoptVisual(AbstractVisual* visual) { m_visual = visual; }
		
		/** Gets used visualization
		 */
		template<typename T> T* getVisual() const { return reinterpret_cast<T*>(m_visual); }
		
		/** Sets if object blocks movement
		 */
		void setBlocking(bool blocking) { m_blocking = blocking; }

		/** Gets if object blocks movement
		 */
		bool isBlocking() const;
	
		/** Set to true, if object is such that it doesn't move
		 */
		void setStatic(bool stat) { m_static = stat; }

		/** Gets if object moves
		 */
		bool isStatic() const;
	
		bool operator==(const Object& obj) const;
		bool operator!=(const Object& obj) const;
		
	private:
		std::string m_id;
		std::string m_namespace;
		Object* m_inherited;
		std::map<std::string, Action*>* m_actions;
		bool m_blocking;
		bool m_static;
		AbstractPather* m_pather;
		AbstractVisual* m_visual;
		Action* m_defaultaction;
	};

} //FIFE
#endif

