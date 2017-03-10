/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

#ifndef FIFE_CLASS_H
#define FIFE_CLASS_H

// Standard C++ library includes
#include <cassert>
#include <cstddef>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
namespace FIFE {

	typedef std::size_t fifeid_t;

	/** Base class for all fife classes
	 * Used e.g. to track instances over swig conversion
	 */
	class FifeClass {
	public:
		FifeClass(): m_fifeid(m_curid++) { }

		virtual ~FifeClass() { }

		/** Gets unique id of this instance inside the engine
		 */
		fifeid_t getFifeId() { return m_fifeid; }

	private:
		fifeid_t m_fifeid;
		static fifeid_t m_curid;
	};
}

#endif
