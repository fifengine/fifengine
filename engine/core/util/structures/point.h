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

#ifndef FIFE_VIDEO_POINT_H
#define FIFE_VIDEO_POINT_H

// Standard C++ library includes
#include <iostream>
#include <cassert>
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"
#include "util/math/fife_math.h"

namespace FIFE {

	/** A 2D Point
	 *
	 * This is a small helper class to aid in 2d vector arithmetics.
	 * @see Rect
	 */
	template <typename T> class PointType2D {
	public:
		union {
			T val[2];
			struct {
				T x,y;
			};
		};

		/** Constructor
		 *
		 * Creates a with 0 as default values.
		 */
		explicit PointType2D(T _x = 0, T _y = 0): x(_x), y(_y) {
		}

		/** Copy Constructor
		 */
		PointType2D(const PointType2D<T>& rhs): x(rhs.x), y(rhs.y) {
		}

		/** Vector addition
		 */
		PointType2D<T> operator+(const PointType2D<T>& p) const {
			return PointType2D<T>(x + p.x, y + p.y);
		}

		/** Vector substraction
		 */
		PointType2D<T> operator-(const PointType2D<T>& p) const {
			return PointType2D<T>(x - p.x, y - p.y);
		}

		/** Vector inplace addition
		 */
		PointType2D<T>& operator+=(const PointType2D<T>& p) {
			x += p.x;
			y += p.y;
			return *this;
		}

		/** Vector inplace substraction
		 */
		PointType2D<T>& operator-=(const PointType2D<T>& p) {
			x -= p.x;
			y -= p.y;
			return *this;
		}

		/** Scalar multiplication with an integer value
		 */
		PointType2D<T> operator*(const T& i) const {
			return PointType2D<T>(x * i, y * i);
		}

		/** Scalar division with an integer value
		 */
		PointType2D<T> operator/(const T& i) const {
			return PointType2D<T>(x / i, y / i);
		}

		/** Equality comparision
		 */
		bool operator==(const PointType2D<T>& p) const {
			return x == p.x && y == p.y;
		}

		/** Equality comparision
		 */
		bool operator!=(const PointType2D<T>& p) const {
			return !(x == p.x && y == p.y);
		}

		/** Return length
		 */
		T length() const {
			double sq;
			sq = x*x + y*y;
			return static_cast<T>(Mathd::Sqrt(sq));
		}

		/** Normalizes the point
		 */
		void normalize() {
			T invLength = static_cast<T>(1.0/length());

			//TODO: get rid of this static cast
			if (invLength > static_cast<T>(Mathd::zeroTolerance())) {
				x = x * invLength;
				y = y * invLength;
			}
			else {
				x = 0;
				y = 0;
			}
		}

		/** Rotates the point around the origin
		 */
		void rotate(T angle){
			//TODO: get rid of this static cast
			T theta = (angle * static_cast<T>(Mathd::pi()))/180;
			T costheta = static_cast<T>(Mathd::Cos(theta));
			T sintheta = static_cast<T>(Mathd::Sin(theta));

			T nx = x;
			T ny = y;

			x = costheta * nx - sintheta * ny;
			y = sintheta * nx + costheta * ny;
		}

		/** Rotates the point around an origin
		 */
		void rotate(const PointType2D<T>& origin, T angle){
			//TODO: get rid of this static cast
			T theta = (angle * static_cast<T>(Mathd::pi()))/180;
			T costheta = static_cast<T>(Mathd::Cos(theta));
			T sintheta = static_cast<T>(Mathd::Sin(theta));

			T nx = x - origin.x;
			T ny = y - origin.y;

			x = costheta * nx - sintheta * ny;
			y = sintheta * nx + costheta * ny;
		}

		/** Sets the x and y coordinate of the 2D point
		 */
		void set(T _x, T _y) {
			x = _x;
			y = _y;
		}

		inline T& operator[] (int32_t ind) {
			assert(ind > -1 && ind < 2);
			return val[ind];
		}
	};

	/** Print coords of the Point to a stream
	 */
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType2D<T>& p) {
		return os << "(" << p.x << ":" << p.y << ")";
	}

	typedef PointType2D<int32_t> Point;
	typedef PointType2D<double> DoublePoint;
	typedef std::vector<Point> PointVector;
	typedef std::vector<DoublePoint> DoublePointVector;

	/** A 3D Point
	 *
	 * This is a small helper class to aid in 3d vector arithmetics.
	 * @see Rect
	 */
	template <typename T> class PointType3D {
	public:
		union {
			T val[3];
			struct {
				T x,y,z;
			};
		};

		/** Constructor
		 *
		 * Creates a with 0 as default values.
		 */
		explicit PointType3D(T _x = 0, T _y = 0, T _z = 0): x(_x), y(_y), z(_z) {
		}

		/** Copy Constructor
		 */
		PointType3D(const PointType3D<T>& rhs): x(rhs.x), y(rhs.y), z(rhs.z) {
		}

		/** Vector addition
		 */
		PointType3D<T> operator+(const PointType3D<T>& p) const {
			return PointType3D<T>(x + p.x, y + p.y, z + p.z);
		}

		/** Vector substraction
		 */
		PointType3D<T> operator-(const PointType3D<T>& p) const {
			return PointType3D<T>(x - p.x, y - p.y, z - p.z);
		}

		/** Vector inplace addition
		 */
		PointType3D<T>& operator+=(const PointType3D<T>& p) {
			x += p.x;
			y += p.y;
			z += p.z;
			return *this;
		}

		/** Vector inplace substraction
		 */
		PointType3D<T>& operator-=(const PointType3D<T>& p) {
			x -= p.x;
			y -= p.y;
			z -= p.z;
			return *this;
		}

		/** Scalar multiplication with an integer value
		 */
		PointType3D<T> operator*(const T& i) const {
			return PointType3D<T>(x * i, y * i, z * i);
		}

		/** Scalar division with an integer value
		 */
		PointType3D<T> operator/(const T& i) const {
			return PointType3D<T>(x / i, y / i, z / i);
		}

		/** Equality comparision
		 */
		bool operator==(const PointType3D<T>& p) const {
			/*return x == p.x && y == p.y && z == p.z;*/
			return Mathd::Equal(x, p.x) && Mathd::Equal(y, p.y) && Mathd::Equal(z, p.z);
		}

		/** Equality comparision
		 */
		bool operator!=(const PointType3D<T>& p) const {
			return !(Mathd::Equal(x, p.x) && Mathd::Equal(y, p.y) && Mathd::Equal(z, p.z));
		}

		/** Return length
		 */
		T length() const {
			double sq;
			sq = x*x + y*y + z*z;
			return static_cast<T>(Mathd::Sqrt(sq));
		}

		/** Normalizes the point
		 */
		void normalize() {
			T invLength = static_cast<T>(1.0/length());

			//TODO: get rid of this static cast
			if (invLength > static_cast<T>(Mathd::zeroTolerance())) {
				x = x * invLength;
				y = y * invLength;
				z = z * invLength;
			}
			else {
				x = 0;
				y = 0;
				z = 0;
			}
		}

		/** Sets the x, y and z coordinates of the 3D point
		 */
		void set(T _x, T _y, T _z) {
			x = _x;
			y = _y;
			z = _z;
		}

		inline T& operator[] (int32_t ind) {
			assert(ind > -1 && ind < 3);
			return val[ind];
		}
	};

	/** Print coords of the Point to a stream
	 */
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType3D<T>& p) {
		return os << "(" << p.x << ":" << p.y << ":" << p.z << ")";
	}

	typedef PointType3D<int32_t> Point3D;
	typedef PointType3D<double> DoublePoint3D;
	typedef std::vector<Point3D> Point3DVector;
	typedef std::vector<DoublePoint3D> DoublePoint3DVector;

	/** Convert from 2D double point to 2D int32_t point
	 */
	inline Point doublePt2intPt(DoublePoint pt) {
		Point tmp(static_cast<int32_t>(round(pt.x)), static_cast<int32_t>(round(pt.y)));
		return tmp;
	}

	/** Convert from 3D double point to 3D int32_t point
	 */
	inline Point3D doublePt2intPt(DoublePoint3D pt) {
		Point3D tmp(static_cast<int32_t>(round(pt.x)), static_cast<int32_t>(round(pt.y)), static_cast<int32_t>(round(pt.z)));
		return tmp;
	}

	/** Convert from 2D int32_t point to 2D double point
	 */
	inline DoublePoint intPt2doublePt(Point pt) {
		DoublePoint tmp(static_cast<double>(pt.x), static_cast<double>(pt.y));
		return tmp;
	}

	/** Convert from 3D int32_t point to 3D double point
	 */
	inline DoublePoint3D intPt2doublePt(Point3D pt) {
		DoublePoint3D tmp(static_cast<double>(pt.x), static_cast<double>(pt.y), static_cast<double>(pt.z));
		return tmp;
	}

}

#endif
