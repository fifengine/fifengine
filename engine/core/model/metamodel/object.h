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
#include "util/attributedclass.h"

namespace FIFE {

	class Dataset;
	class Action;
	class AbstractPather;

	/** Object class
	 *
	 * Objects describe the properties of objects. Objects may
	 * contain information such as graphics, z-values, and and
	 * other properties.
	 *
	 * Objects may inherit default values from another object.
	 *
	 * @note oget redefines AttributedClass's notion of get,
	 * forwarding requests for undefined fields to the inherited
	 * object. It must be a separate function since templates
	 * can't be virtual. C++ sucks. A better name than "oget"
	 * would be welcome.
	 *
	 * @see AttributedClass in util/attributedclass.h
	 */
	class Object : public AttributedClass {
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
			Object(const std::string& identifier, Object* inherited=NULL);

			/** Destructor
			 */
			~Object();
			
			template<typename T>
			const T& oget(const std::string& field) {
				
				if(hasField(field) || !m_inherited)
					return get<T>(field);

				return m_inherited->oget<T>(field);
			}

			/** Adds new action with given name. In case there is action already
			 *  with given name, returns it instead of new object
			 *  Action instances are managed by object
			 */
			Action* addAction(const std::string& identifier, const std::string& action_name);

			/** Gets action with given name. If not found, returns NULL
			 */
			Action* getAction(const std::string& action_name);

			/** Sets pather used by instances created out of this object
			 */
			void setPather(AbstractPather* pather);

			/** Gets associated pather
			 */
			AbstractPather* getPather() { return m_pather; }

			/** Returns an index to visual representation of object
			 * useful as an optimization for non-action instances
			 * and e.g. editor toolbox visulizations
			 */
			int getStaticImageId() { return m_static_img_id; }
			
			/** Sets an index to visual representation of instance.
			 * @see getStaticImageId
			 */
			void setStaticImageId(int img_id) { m_static_img_id = img_id; }

			/** Gets an object where this object was inherited from
			 * @see inherited object
			 */
			Object* getInherited() { return m_inherited; }


		private:
			Object* m_inherited;
			std::map<std::string, Action*>* m_actions;
			AbstractPather* m_pather;
			int m_static_img_id;
	};

	template <>
	inline
	const std::string& Object::oget<std::string>(const std::string& field) {

		if(field == "parent")
			return m_inherited->Id();

		if(hasField(field) || !m_inherited)
			return get<std::string>(field);

		return m_inherited->oget<std::string>(field);
	}


} //FIFE
#endif

