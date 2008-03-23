/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_PROTOTYPE_H
#define FIFE_PROTOTYPE_H

// Standard C++ library includes
#include <string>
#include <map>

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
		 * Objects are created by calling addObject from dataset, thus
		 * this method should really be called only by dataset or test code
		 * @see Dataset in model/metamodel/dataset.h for creation
		 * of objects.
		 */
		Object(const std::string& identifier, const std::string& name_space, Object* inherited=NULL);

		/** Destructor
		 */
		~Object();
		
		const std::string& getId() { return m_id; }
		const std::string& getNamespace() { return m_namespace; }

		/** Adds new action with given id. In case there is action already
		 *  with given id, returns it instead of new object
		 *  Action instances are managed by object
		 */
		Action* createAction(const std::string& identifier);

		/** Gets action with given id. If not found, returns NULL
		 */
		Action* getAction(const std::string& identifier);

		/** Sets pather used by instances created out of this object
		 */
		void setPather(AbstractPather* pather);

		/** Gets associated pather
		 */
		AbstractPather* getPather() { return m_pather; }

		/** Gets an object where this object was inherited from
		 * @see inherited object
		 */
		Object* getInherited() { return m_inherited; }

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
		bool isBlocking();
	
		/** Set to true, if object is such that it doesn't move
		 */
		void setStatic(bool stat) { m_static = stat; }

		/** Gets if object moves
		 */
		bool isStatic();
	
	
	private:
		std::string m_id;
		std::string m_namespace;
		Object* m_inherited;
		std::map<std::string, Action*>* m_actions;
		bool m_blocking;
		bool m_static;
		AbstractPather* m_pather;
		AbstractVisual* m_visual;
	};

	class ObjectLoader : public ResourceLoader {
	public:
		Object* load(const ResourceLocation& location) { return dynamic_cast<Object*>(load(location)); }
		Object* load(const std::string& filename) { return load(ResourceLocation(filename)); }
	};

} //FIFE
#endif

