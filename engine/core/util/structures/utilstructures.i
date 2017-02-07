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
		PointType2D(const PointType2D<T>& rhs);
		PointType2D<T> operator+(const PointType2D<T>& p) const;
		PointType2D<T> operator-(const PointType2D<T>& p) const;
		PointType2D<T>& operator+=(const PointType2D<T>& p);
		PointType2D<T>& operator-=(const PointType2D<T>& p);
		PointType2D<T> operator*(const T& i) const;
		PointType2D<T> operator/(const T& i) const;
		bool operator==(const PointType2D<T>& p) const;
		bool operator!=(const PointType2D<T>& p) const;
		T length() const;
		void normalize();
		void rotate(T angle);
		void rotate(const PointType2D<T>& origin, T angle);
		void set(T _x, T _y);
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType2D<T>& p);

	typedef PointType2D<int32_t> Point;
	typedef PointType2D<int32_t> IntPoint;
	typedef PointType2D<double> DoublePoint;
	typedef PointType2D<int32_t> IntPoint2D;
	typedef PointType2D<double> DoublePoint2D;

	%template(Point) PointType2D<int32_t>;
	%template(DoublePoint) PointType2D<double>;

	template <typename T> class PointType3D {
	public:
		T x;
		T y;
		T z;
		explicit PointType3D(T _x = 0, T _y = 0, T _z = 0);
		PointType3D(const PointType3D<T>& rhs);
		PointType3D<T> operator+(const PointType3D<T>& p) const;
		PointType3D<T> operator-(const PointType3D<T>& p) const;
		PointType3D<T>& operator+=(const PointType3D<T>& p);
		PointType3D<T>& operator-=(const PointType3D<T>& p);
		PointType3D<T> operator*(const T& i) const;
		PointType3D<T> operator/(const T& i) const;
		bool operator==(const PointType3D<T>& p) const;
		bool operator!=(const PointType3D<T>& p) const;
		T length() const;
		void normalize();
		void set(T _x, T _y, T _z);
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType3D<T>& p);

	typedef PointType3D<int32_t> Point3D;
	typedef PointType3D<int32_t> IntPoint3D;
	typedef PointType3D<double> DoublePoint3D;

	%template(Point3D) PointType3D<int32_t>;
	%template(DoublePoint3D) PointType3D<double>;

	template <typename T> class RectType {
	public:
		T x;
		T y;
		T w;
		T h;
		explicit RectType(T x = 0, T y = 0, T width = 0, T height = 0);
		T right() const;
		T bottom() const;
		bool operator==(const RectType<T>& rect ) const;
		bool contains( const PointType2D<T>& point ) const;
		bool intersects( const RectType<T>& rect ) const;
		bool intersectInplace( const RectType<T>& rect );
	};
	
	template<typename T>
	std::ostream& operator<<(std::ostream&, const RectType<T>&);
	
	typedef RectType<int32_t> Rect;
	typedef RectType<float> FloatRect;
	typedef RectType<double> DoubleRect;
	
	%template(Rect) RectType<int32_t>;
	%template(FloatRect) RectType<float>;
	%template(DoubleRect) RectType<double>;
}

namespace std {
	%template(FifePointVector) vector<FIFE::Point>;
}
