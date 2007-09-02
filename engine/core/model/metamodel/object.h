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
#include "util/angles.h"

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

			/** Adds new static image with given angle (degrees)
			 * Static images are used in case there are no actions active in the instance
			 * There can be several static images for different angles, that are used in
			 * case view / layer is rotated
			 * In case there are no exact matches for current view angles, closest one is used
			 * @param angle angle for image. 0 degrees starts from right and turns counter-clockwise
			 *              (normal math notation)
			 * @param image_index index of image to use for given degress
			 */
			void addStaticImage(unsigned int angle, int image_index);
			
			/** Returns closest matching static image for given angle
			 * @return id for static image
			 */
			int getStaticImageIndexByAngle(unsigned int angle);

			/** Gets an object where this object was inherited from
			 * @see inherited object
			 */
			Object* getInherited() { return m_inherited; }


		private:
			Object* m_inherited;
			std::map<std::string, Action*>* m_actions;
			type_angle2id m_angle2img;
					
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

