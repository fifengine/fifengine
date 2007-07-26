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

	class Object;

	template<typename T>
	struct MaybeData {
		MaybeData() : has_data(false) {}

		const MaybeData& operator=(const T& d) {
			data = d;
			has_data = true;
			return *this;
		}

		T data;
		bool has_data;
	};

	/** Prototype class
	 *
	 * Prototypes describe the properties of objects. Prototypes may
	 * contain information such as graphics, z-values, and and other
	 * properties.
	 *
	 * Prototypes do NOT contain object-specific properties (such as x/y
	 * coordinates). 
	 *
	 * Objects may override the value of the Prototypes they use, but
	 * they may not CHANGE a Prototype's values.
	 *
	 * Prototypes should be thought of as sets of properties. For
	 * example: the color "red" is a property. Many objects exhibit
	 * the property red (specifically, anything that is red). However,
	 * red itself does not exist anywhere in the world; it would be
	 * very surprising to encounter the property red itself. Therefore,
	 * prototypes are a meta concept.
	 *
	 * If an object exhibits a property, we say that the object
	 * instantiates that property.
	 *
	 */
	class Prototype : public AttributedClass {
		public:

			Prototype()
				: AttributedClass("Prototype")
			{ }

			/** Create an object from this Prototype
			 */
			Object* createInstance();

			const std::string& getName();

		private:

			std::string m_name;

			MaybeData<int>   m_zvalue;
			MaybeData<bool>  m_isstatic;
			MaybeData<Point> m_position;
	};

}} //FIFE::model
#endif

