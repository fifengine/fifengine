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

#ifndef FIFE_MAP_OBJECT_ITERATOR_H
#define FIFE_MAP_OBJECT_ITERATOR_H

// Standard C++ library includes

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE { namespace map {

	class Elevation;
	typedef boost::shared_ptr<Elevation> ElevationPtr;

	class ObjectInfo;
	typedef boost::shared_ptr<ObjectInfo> ObjectPtr;

	/** Helper class for looping over all objects on an elevation
	 */
	class ObjectIterator {
		public:
			/** Constructor
			 * Construct an iterator to loop over all objects
			 * in the given elevation.
			 */
			ObjectIterator(ElevationPtr elevation);

			/** Return next Object or an invalid Pointer
			 */
			ObjectPtr next();
		protected:
			ElevationPtr m_elevation;
			size_t m_layer;
			Point  m_gridpos;
			size_t m_object;
			
	};

} } //FIFE::map

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
