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

			~Object() 
			{ }
			
			template<typename T>
			const T& oget(const std::string& field) {
				if(hasField(field) || !m_inherited)
					return get<T>(field);

				return m_inherited->oget<T>(field);
			}

		private:

			/** An object may optionally inherit default attributes
			 * from another object. This object may override these
			 * defaults, but it may not CHANGE the inherited values.
			 *
			 * @see Dataset in model/metamodel/dataset.h for creation
			 * of objects.
			 */
			Object(Object* inherited)
				: AttributedClass("Object"), m_inherited(inherited)
			{ }

			Object* m_inherited;

			friend class Dataset;
	};

}} //FIFE::model
#endif

