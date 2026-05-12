// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_POINT_H
#define FIFE_VIDEO_POINT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cassert>
#include <iostream>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"
#include "util/math/fife_math.h"

namespace FIFE
{

    /** A 2D Point
     *
     * This is a small helper class to aid in 2d vector arithmetics.
     * @see Rect
     */
    template <typename T>
    class PointType2D
    {
        public:
            T x;
            T y;

            /** Constructor
             *
             * Creates a with 0 as default values.
             */
            explicit PointType2D(T _x = 0, T _y = 0) : x(_x), y(_y)
            {
            }

            /** Copy Constructor
             */
            PointType2D(PointType2D<T> const & rhs) : x(rhs.x), y(rhs.y)
            {
            }

            PointType2D<T>& operator=(PointType2D<T> const &) = default;

            /** Vector addition
             */
            PointType2D<T> operator+(PointType2D<T> const & p) const
            {
                return PointType2D<T>(x + p.x, y + p.y);
            }

            /** Vector substraction
             */
            PointType2D<T> operator-(PointType2D<T> const & p) const
            {
                return PointType2D<T>(x - p.x, y - p.y);
            }

            /** Vector inplace addition
             */
            PointType2D<T>& operator+=(PointType2D<T> const & p)
            {
                x += p.x;
                y += p.y;
                return *this;
            }

            /** Vector inplace substraction
             */
            PointType2D<T>& operator-=(PointType2D<T> const & p)
            {
                x -= p.x;
                y -= p.y;
                return *this;
            }

            /** Scalar multiplication with an integer value
             */
            PointType2D<T> operator*(T const & i) const
            {
                return PointType2D<T>(x * i, y * i);
            }

            /** Scalar division with an integer value
             */
            PointType2D<T> operator/(T const & i) const
            {
                return PointType2D<T>(x / i, y / i);
            }

            /** Equality comparision
             */
            bool operator==(PointType2D<T> const & p) const
            {
                return x == p.x && y == p.y;
            }

            /** Equality comparision
             */
            bool operator!=(PointType2D<T> const & p) const
            {
                return !(x == p.x && y == p.y);
            }

            /** Return length
             */
            T length() const
            {
                double sq = 0.0;
                sq        = x * x + y * y;
                return static_cast<T>(Mathd::Sqrt(sq));
            }

            /** Normalizes the point
             */
            void normalize()
            {
                T invLength = static_cast<T>(1.0 / length());

                // TODO: get rid of this static cast
                if (invLength > static_cast<T>(Mathd::zeroTolerance())) {
                    x = x * invLength;
                    y = y * invLength;
                } else {
                    x = 0;
                    y = 0;
                }
            }

            /** Rotates the point around the origin
             */
            void rotate(T angle)
            {
                // TODO: get rid of this static cast
                T theta    = (angle * static_cast<T>(Mathd::pi())) / 180;
                T costheta = static_cast<T>(Mathd::Cos(theta));
                T sintheta = static_cast<T>(Mathd::Sin(theta));

                T nx = x;
                T ny = y;

                x = costheta * nx - sintheta * ny;
                y = sintheta * nx + costheta * ny;
            }

            /** Rotates the point around an origin
             */
            void rotate(PointType2D<T> const & origin, T angle)
            {
                // TODO: get rid of this static cast
                T theta    = (angle * static_cast<T>(Mathd::pi())) / 180;
                T costheta = static_cast<T>(Mathd::Cos(theta));
                T sintheta = static_cast<T>(Mathd::Sin(theta));

                T nx = x - origin.x;
                T ny = y - origin.y;

                x = costheta * nx - sintheta * ny;
                y = sintheta * nx + costheta * ny;
            }

            /** Sets the x and y coordinate of the 2D point
             */
            void set(T _x, T _y)
            {
                x = _x;
                y = _y;
            }

            T& operator[](int32_t ind)
            {
                assert(ind > -1 && ind < 2);
                switch (ind) {
                case 0:
                    return x;
                case 1:
                    return y;
                default:
                    return x;
                }
            }

            T const & operator[](int32_t ind) const
            {
                assert(ind > -1 && ind < 2);
                switch (ind) {
                case 0:
                    return x;
                case 1:
                    return y;
                default:
                    return x;
                }
            }
    };

    /** Print coords of the Point to a stream
     */
    template <typename T>
    std::ostream& operator<<(std::ostream& os, PointType2D<T> const & p)
    {
        return os << "(" << p.x << ":" << p.y << ")";
    }

    using Point             = PointType2D<int32_t>;
    using DoublePoint       = PointType2D<double>;
    using PointVector       = std::vector<Point>;
    using DoublePointVector = std::vector<DoublePoint>;
    /** A 3D Point
     *
     * This is a small helper class to aid in 3d vector arithmetics.
     * @see Rect
     */
    template <typename T>
    class PointType3D
    {
        public:
            T x;
            T y;
            T z;

            /** Constructor
             *
             * Creates a with 0 as default values.
             */
            explicit PointType3D(T _x = 0, T _y = 0, T _z = 0) : x(_x), y(_y), z(_z)
            {
            }

            /** Copy Constructor
             */
            PointType3D(PointType3D<T> const & rhs) : x(rhs.x), y(rhs.y), z(rhs.z)
            {
            }

            /** Assignment operator (explicitly defaulted to avoid MSVC C5267 warning) */
            PointType3D<T>& operator=(PointType3D<T> const &) = default;

            /** Vector addition
             */
            PointType3D<T> operator+(PointType3D<T> const & p) const
            {
                return PointType3D<T>(x + p.x, y + p.y, z + p.z);
            }

            /** Vector substraction
             */
            PointType3D<T> operator-(PointType3D<T> const & p) const
            {
                return PointType3D<T>(x - p.x, y - p.y, z - p.z);
            }

            /** Vector inplace addition
             */
            PointType3D<T>& operator+=(PointType3D<T> const & p)
            {
                x += p.x;
                y += p.y;
                z += p.z;
                return *this;
            }

            /** Vector inplace substraction
             */
            PointType3D<T>& operator-=(PointType3D<T> const & p)
            {
                x -= p.x;
                y -= p.y;
                z -= p.z;
                return *this;
            }

            /** Scalar multiplication with an integer value
             */
            PointType3D<T> operator*(T const & i) const
            {
                return PointType3D<T>(x * i, y * i, z * i);
            }

            /** Scalar division with an integer value
             */
            PointType3D<T> operator/(T const & i) const
            {
                return PointType3D<T>(x / i, y / i, z / i);
            }

            /** Equality comparision
             */
            bool operator==(PointType3D<T> const & p) const
            {
                /*return x == p.x && y == p.y && z == p.z;*/
                return Mathd::Equal(x, p.x) && Mathd::Equal(y, p.y) && Mathd::Equal(z, p.z);
            }

            /** Equality comparision
             */
            bool operator!=(PointType3D<T> const & p) const
            {
                return !(Mathd::Equal(x, p.x) && Mathd::Equal(y, p.y) && Mathd::Equal(z, p.z));
            }

            /** Return length
             */
            T length() const
            {
                double sq = 0.0;
                sq        = x * x + y * y + z * z;
                return static_cast<T>(Mathd::Sqrt(sq));
            }

            /** Normalizes the point
             */
            void normalize()
            {
                T invLength = static_cast<T>(1.0 / length());

                // TODO: get rid of this static cast
                if (invLength > static_cast<T>(Mathd::zeroTolerance())) {
                    x = x * invLength;
                    y = y * invLength;
                    z = z * invLength;
                } else {
                    x = 0;
                    y = 0;
                    z = 0;
                }
            }

            /** Sets the x, y and z coordinates of the 3D point
             */
            void set(T _x, T _y, T _z)
            {
                x = _x;
                y = _y;
                z = _z;
            }

            T& operator[](int32_t ind)
            {
                assert(ind > -1 && ind < 3);
                switch (ind) {
                case 0:
                    return x;
                case 1:
                    return y;
                case 2:
                    return z;
                default:
                    return x;
                }
            }

            T const & operator[](int32_t ind) const
            {
                assert(ind > -1 && ind < 3);
                switch (ind) {
                case 0:
                    return x;
                case 1:
                    return y;
                case 2:
                    return z;
                default:
                    return x;
                }
            }
    };

    /** Print coords of the Point to a stream
     */
    template <typename T>
    std::ostream& operator<<(std::ostream& os, PointType3D<T> const & p)
    {
        return os << "(" << p.x << ":" << p.y << ":" << p.z << ")";
    }

    using Point3D             = PointType3D<int32_t>;
    using DoublePoint3D       = PointType3D<double>;
    using Point3DVector       = std::vector<Point3D>;
    using DoublePoint3DVector = std::vector<DoublePoint3D>;
    /** Convert from 2D double point to 2D int32_t point
     */
    inline Point doublePt2intPt(DoublePoint const & pt)
    {
        Point const tmp(static_cast<int32_t>(round(pt.x)), static_cast<int32_t>(round(pt.y)));
        return tmp;
    }

    /** Convert from 3D double point to 3D int32_t point
     */
    inline Point3D doublePt2intPt(DoublePoint3D const & pt)
    {
        Point3D const tmp(
            static_cast<int32_t>(round(pt.x)), static_cast<int32_t>(round(pt.y)), static_cast<int32_t>(round(pt.z)));
        return tmp;
    }

    /** Convert from 2D int32_t point to 2D double point
     */
    inline DoublePoint intPt2doublePt(Point const & pt)
    {
        DoublePoint const tmp(static_cast<double>(pt.x), static_cast<double>(pt.y));
        return tmp;
    }

    /** Convert from 3D int32_t point to 3D double point
     */
    inline DoublePoint3D intPt2doublePt(Point3D const & pt)
    {
        DoublePoint3D const tmp(static_cast<double>(pt.x), static_cast<double>(pt.y), static_cast<double>(pt.z));
        return tmp;
    }

} // namespace FIFE

#endif
