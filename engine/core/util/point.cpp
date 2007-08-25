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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "point.h"

namespace FIFE {

	Point::Point(int _x, int _y) : x(_x), y(_y) {
	}

	Point Point::operator+(const Point& p) const {
		return Point(x + p.x, y + p.y);
	}

	Point Point::operator-(const Point& p) const {
		return Point(x - p.x, y - p.y);
	}

	Point& Point::operator+=(const Point& p) {
		x += p.x;
		y += p.y;
		return *this;
	}

	Point& Point::operator-=(const Point& p) {
		x -= p.x;
		y -= p.y;
		return *this;
	}

	Point Point::operator*(const long& p) const {
		return Point(x * p,y * p);
	}

	Point Point::operator/(const long& p) const {
		return Point(x / p,y / p);
	}

	bool Point::operator==(const Point& p) const {
		return x == p.x && y == p.y;
	}

	bool Point::operator!=(const Point& p) const {
		return !(x == p.x && y == p.y);
	}

	std::ostream& operator<<(std::ostream& os, const Point& p) {
		return os << "(" << p.x << "," << p.y << ")";
	}

	uint32_t Point::length() const {
		double sq;
		sq = x*x + y*y;
		return uint32_t(sqrt(sq) + 0.5);
	}

}
