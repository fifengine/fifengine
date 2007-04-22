/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef FIFEPOINT_H
#define FIFEPOINT_H

#include <iostream>

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
			int x;
			/** The Y coordinate
			 */
			int y;

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
			Point operator*(const int& i) const;
	};

	/** Print coords of the Point to a stream
	 */
	std::ostream& operator<<(std::ostream& os, const Point& p);

}

#endif
