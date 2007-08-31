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
	 * @see Rect
	 */
	template <typename T> class PointType {
		public:
			/** The X coordinate.
			 */
			T x;
			/** The Y coordinate
			 */
			T y;

			/** Constructor
			 *
			 * Creates a with 0 as default values.
			 */
			explicit PointType(T _x = 0, T _y = 0): x(_x), y(_y) {
			}

			/** Vector addition
			 */
			PointType<T> operator+(const PointType<T>& p) const {
				return PointType<T>(x + p.x, y + p.y);
			}

			/** Vector substraction
			 */
			PointType<T> operator-(const PointType<T>& p) const {
				return PointType<T>(x - p.x, y - p.y);
			}

			/** Vector inplace addition
			 */
			PointType<T>& operator+=(const PointType<T>& p) {
				x += p.x;
				y += p.y;
				return *this;
			}

			/** Vector inplace substraction
			 */
			PointType<T>& operator-=(const PointType<T>& p) {
				x -= p.x;
				y -= p.y;
				return *this;
			}

			/** Scalar multiplication with an integer value
			 */
			PointType<T> operator*(const long& i) const {
				return PointType<T>(x * i, y * i);
			}

			/** Scalar division with an integer value
			 */
			PointType<T> operator/(const long& i) const {
				return PointType<T>(x / i, y / i);
			}

			/** Equality comparision
			 */
			bool operator==(const PointType<T>& p) const {
				return x == p.x && y == p.y;
			}

			/** Equality comparision
			 */
			bool operator!=(const PointType<T>& p) const {
				return !(x == p.x && y == p.y);
			}

			/** Return length
			 */
			T length() const {
				double sq;
				sq = x*x + y*y;
				return static_cast<T>(sqrt(sq) + 0.5);
			}
	};

	/** Print coords of the Point to a stream
	 */
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType<T>& p) {
		return os << "(" << p.x << "," << p.y << ")";
	}

	typedef PointType<int> Point;
	typedef PointType<double> DoublePoint;
}

#endif
