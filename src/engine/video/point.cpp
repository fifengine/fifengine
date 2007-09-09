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

#include "point.h"

namespace FIFE {

	Point::Point(int x, int y) : x(x), y(y) {
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

	Point Point::operator*(const int& p) const {
		return Point(x * p,y * p);
	}


	std::ostream& operator<<(std::ostream& os, const Point& p) {
		return os << "(" << p.x << "," << p.y << ")";
	}

}
