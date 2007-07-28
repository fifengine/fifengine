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

#ifndef FIFE_GEOMETRY_TYPE_H
#define FIFE_GEOMETRY_TYPE_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/point.h"

namespace FIFE { namespace model {

	class GeometryType {
		public:

		GeometryType() {}

		GeometryType(size_t _id, const std::string& _g,
		             const Point& _size,
		             const Point& _transform,
		             const Point& _offset,
		             int _flags)
			: id(_id), geometry(_g), size(_size), transform(_transform),
			  offset(_offset), flags(_flags) {}

		// id
		size_t id;

		// name of this geometry type
		std::string geometry;

		// base size
		Point size;

		// topmost left border intersection point
		Point transform;

		// extra offset
		Point offset;

		int flags;

	};

}}; //FIFE::model

#endif
