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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/attributedclass.h"

namespace FIFE { namespace model { 

	class Dataset;

	/** Object class
	 *
	 * Objects describe the properties of objects. Objects may
	 * contain information such as graphics, z-values, and and
	 * other properties.
	 *
	 * Objects may inherit default values from another object.
	 *
	 * @note All public methods in this class override
	 * AttributedClass: see that class for documentation of
	 * these methods. The semantics of Object's overrides
	 * differ only because they check for inherited values.
	 *
	 * @see AttributedClass in util/attributedclass.h
	 */
	class Object : public AttributedClass {
		public:

			~Object() 
			{ }
			
			template<typename T>
			const T& get(const std::string& field, const T& value = T()) const {
				if(hasField(field) || !m_inherited)
					return AttributedClass::get<T>(field, value);

				return m_inherited->get<T>(field, value);
			}

		private:

			/** An object may optionally inherit default attributes
			 * from another object. This object may override these
			 * defaults, but it may not CHANGE the inherited values.
			 *
			 * @see Dataset in model/metamodel/dataset.h for creation
			 * of objects.
			 */
			Object(const Object* inherited)
				: AttributedClass("Object"), m_inherited(inherited)
			{ }

			const Object* m_inherited;

			friend class Dataset;
	};

}} //FIFE::model
#endif

