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

#ifndef FIFE_VIDEO_POINT_H
#define FIFE_VIDEO_POINT_H

// Standard C++ library includes
#include <iostream>

// Platform specific includes
#include "fife_math.h"
#include "fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	/** A Point
	 *
	 * This is a small helper class to aid in 2d vector arithmetics.
	 * @note Currently we are using @b int as value, maybe using a fixed
	 * size type is more appropiate, since then the value range is obvious.
	 * @see Rect
	 */
	class Point {
		public:
			/** The X coordinate.
			 */
			int32_t x;
			/** The Y coordinate
			 */
			int32_t y;

			/** Constructor
			 *
			 * Creates a with 0 as default values.
			 */
			explicit Point(int x = 0, int y = 0);

			/** Vector addition
			 */
			Point operator+(const Point& p) const;
			/** Vector substraction
			 */
			Point operator-(const Point& p) const;

			/** Vector inplace addition
			 */
			Point& operator+=(const Point& p);
			/** Vector inplace substraction
			 */
			Point& operator-=(const Point& p);

			/** Scalar multiplikation with an integer value
			 */
			Point operator*(const long& i) const;

			/** Scalar division with an integer value
			 */
			Point operator/(const long& i) const;

			/** Equality comparision
			 */
			bool operator==(const Point& p) const;

			/** Return length
			 */
			uint32_t length() const;
	};

	/** Print coords of the Point to a stream
	 */
	std::ostream& operator<<(std::ostream& os, const Point& p);

}

#endif
