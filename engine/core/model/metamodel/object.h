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

	/** Object class
	 *
	 * Objects describe the properties of objects. Objects may
	 * contain information such as graphics, z-values, and and
	 * other properties.
	 *
	 * Objects may inherit values from other objects. Objects 
	 * may override the value of the Objects they use, but they
	 * may not CHANGE an inherited object's values.
	 *
	 */
	class Object : public AttributedClass {
		public:

			Object()
				: AttributedClass("Object")
			{ }

			const std::string& getName();

		private:

			std::string m_name;
	};

	inline
	const std::string& Object::getName() {
		return m_name;
	}

}} //FIFE::model
#endif

