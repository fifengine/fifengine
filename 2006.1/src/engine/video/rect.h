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

#ifndef FIFERECT_H
#define FIFERECT_H
#include <iostream>
#include "point.h"

namespace FIFE {
	/** A Rectangle on screen.
	 * 
	 * This is a small helper class used for screen coordinate arithmetics.
	 * The same thoughts reasong using @b int as value type as in Point apply
	 * @see Point
	 */
	class Rect {
		public:
			/** The X Coordinate
			 */
			int x;
			/** The Y Coordinate
			 */
			int y;
			/** The width of the rectangle
			 */
			int w;
			/** The height of the rectangle
			 */
			int h;

			/** Constructor
			 * 
			 * Creates a new Rect with the values defaulting to 0
			 */
			explicit Rect(int x = 0, int y = 0, unsigned int width = 0, unsigned int height = 0);

			/** The X coordinate of the right edge
			 */
			inline int right() const {
				return x + w;
			}

			/** The Y coordinate of the bottom edge
			 */
			inline int bottom() const {
				return y + h;
			}

			/** Equivalence operator
			 * @param rect The rectangle to which this is compared.
			 * @return True only if both rectangle  values are all equal
			 */
			inline bool operator==(const Rect& rect ) const {
				return
					x == rect.x && y == rect.y && w == rect.w && h == rect.h;
			}

			/** Check whether a rectangle contains a Point
			 * @param p The point that is checked
			 * @return True if the point lies inside the rectangle or on one of its borders.
			 */
			inline bool contains( const Point& point ) const {
				return
					(((point.x >= x) && (point.x <= x + w))
					 && ((point.y >= y) && (point.y <= y + h)));
			}

			// this might well be to verbose or still buggy,
			// but it did work right
			/** Check whether two rectangles share some area
			 * @param rect The other rectangle that is checked.
			 * @return True, if and only if both rectangles have some covered area in common.
			 * This includes edges that cover each other.
			 * @note This operation is commutative.
			 */
			inline bool intersects( const Rect& rect ) const {
				return
					contains( Point(rect.x, rect.y) )
					|| contains( Point(rect.x +rect.w ,rect.y+rect.h) )
					|| contains( Point(rect.x ,rect.y+rect.h) )
					|| contains( Point(rect.x+rect.w ,rect.y) )
					|| rect.contains( Point(x,y) )
					|| rect.contains( Point(x+w, y+h) );
			}

	};

	/** Stream output operator
	 * Useful for debugging purposes, this will output the coordinates
	 * of the rectangle to the stream.
	 */
	std::ostream& operator<<(std::ostream&, const Rect&);
}

#endif
