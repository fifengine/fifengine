/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

%{
#include "util/structures/point.h"
#include "util/structures/rect.h"
%}

namespace FIFE {

	template <typename T> class PointType2D {
	public:
		T x;
		T y;
		explicit PointType2D(T _x = 0, T _y = 0);
		PointType2D<T> operator+(const PointType2D<T>& p) const;
		PointType2D<T> operator-(const PointType2D<T>& p) const;
		PointType2D<T>& operator+=(const PointType2D<T>& p);
		PointType2D<T>& operator-=(const PointType2D<T>& p);
		PointType2D<T> operator*(const T& i) const;
		PointType2D<T> operator/(const T& i) const;
		bool operator==(const PointType2D<T>& p) const;
		bool operator!=(const PointType2D<T>& p) const;
		T length() const;
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType2D<T>& p);

	typedef PointType2D<int> Point;
	typedef PointType2D<int> IntPoint;
	typedef PointType2D<double> DoublePoint;
	typedef PointType2D<int> IntPoint2D;
	typedef PointType2D<double> DoublePoint2D;

	%template(Point) PointType2D<int>;
	%template(DoublePoint) PointType2D<double>;

	template <typename T> class PointType3D {
	public:
		T x;
		T y;
		T z;
		explicit PointType3D(T _x = 0, T _y = 0, T _z = 0);
		PointType3D<T> operator+(const PointType3D<T>& p) const;
		PointType3D<T> operator-(const PointType3D<T>& p) const;
		PointType3D<T>& operator+=(const PointType3D<T>& p);
		PointType3D<T>& operator-=(const PointType3D<T>& p);
		PointType3D<T> operator*(const T& i) const;
		PointType3D<T> operator/(const T& i) const;
		bool operator==(const PointType3D<T>& p) const;
		bool operator!=(const PointType3D<T>& p) const;
		T length() const;
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType3D<T>& p);

	typedef PointType3D<int> Point3D;
	typedef PointType3D<int> IntPoint3D;
	typedef PointType3D<double> DoublePoint3D;

	%template(Point3D) PointType3D<int>;
	%template(DoublePoint3D) PointType3D<double>;
	
	class Rect {
	public:
		int x;
		int y;
		int w;
		int h;
		explicit Rect(int x = 0, int y = 0, unsigned int width = 0, unsigned int height = 0);
		int right() const;
		int bottom() const;
		bool operator==(const Rect& rect ) const;
		bool contains( const Point& point ) const;
		bool intersects( const Rect& rect ) const;
		bool intersectInplace( const Rect& rect );
	};
	std::ostream& operator<<(std::ostream&, const Rect&);
}
