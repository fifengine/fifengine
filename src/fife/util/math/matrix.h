// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

/***************************************************************************
 * Includes some heavy copying from mathgl-pp project                      *
 * (http://sourceforge.net/projects/mathgl-pp/)                            *
 ***************************************************************************/

#ifndef FIFE_UTIL_MATRIX_H
#define FIFE_UTIL_MATRIX_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cassert>
#include <iostream>
// 3rd party library includes

// FIFE includes
#include "fife_math.h"
#include "util/base/fife_stdint.h"
#include "util/structures/point.h"

namespace FIFE
{

    /** Minimal matrix class to assist in view 3d calculations
     */
    template <typename T>
    class FIFE_API Matrix
    {
        public:
            Matrix() = default;
            template <typename U>
            friend class Matrix;
            template <typename U>
            explicit Matrix(Matrix<U> const & mat)
            {
                for (int32_t index = 0; index < 16; ++index) {
                    (*this)[index] = static_cast<T>(mat[index]);
                }
            }

            Matrix(Matrix const & other)            = default;
            Matrix& operator=(Matrix const & other) = default;

            ~Matrix() = default;

            /** Adjoint method inverse, constant time inversion implementation
             */
            Matrix inverse() const
            {
                Matrix ret(adjoint());

                T determinant = (m0 * ret[0]) + (m1 * ret[4]) + (m2 * ret[8]) + (m3 * ret[12]);
                assert(determinant != 0 && "Singular matrix has no inverse");

                ret /= determinant;
                return ret;
            }

            /** Divide this matrix by a scalar
             */
            Matrix& operator/=(T val)
            {
                for (int32_t i = 0; i < 16; ++i) {
                    (*this)[i] /= val;
                }
                return *this;
            }

            /** Get the adjoint matrix
             */
            Matrix adjoint() const
            {
                Matrix ret;

                ret[0] = cofactorm0();
                ret[1] = -cofactorm4();
                ret[2] = cofactorm8();
                ret[3] = -cofactorm12();

                ret[4] = -cofactorm1();
                ret[5] = cofactorm5();
                ret[6] = -cofactorm9();
                ret[7] = cofactorm13();

                ret[8]  = cofactorm2();
                ret[9]  = -cofactorm6();
                ret[10] = cofactorm10();
                ret[11] = -cofactorm14();

                ret[12] = -cofactorm3();
                ret[13] = cofactorm7();
                ret[14] = -cofactorm11();
                ret[15] = cofactorm15();

                return ret;
            }

            /** Make this a rotation matrix
             */

            Matrix& loadRotate(T angle, T x, T y, T z)
            {
                T magSqr = (x * x) + (y * y) + (z * z);
                if (magSqr != 1.0) {
                    T mag = Math<T>::Sqrt(magSqr);
                    x /= mag;
                    y /= mag;
                    z /= mag;
                }
                T c = Math<T>::Cos(angle * Math<T>::pi() / 180);
                T s = Math<T>::Sin(angle * Math<T>::pi() / 180);
                m0  = x * x * (1 - c) + c;
                m1  = y * x * (1 - c) + z * s;
                m2  = z * x * (1 - c) - y * s;
                m3  = 0;

                m4 = x * y * (1 - c) - z * s;
                m5 = y * y * (1 - c) + c;
                m6 = z * y * (1 - c) + x * s;
                m7 = 0;

                m8  = x * z * (1 - c) + y * s;
                m9  = y * z * (1 - c) - x * s;
                m10 = z * z * (1 - c) + c;
                m11 = 0;

                m12 = 0;
                m13 = 0;
                m14 = 0;
                m15 = 1;

                return *this;
            }

            /** Apply scale into this matrix
             */
            Matrix& applyScale(T x, T y, T z)
            {
                static Matrix<T> temp;
                temp.loadScale(x, y, z);
                *this = temp.mult4by4(*this);
                return *this;
            }

            /** Make this a scale matrix
             */
            Matrix& loadScale(T x, T y, T z = 1)
            {
                m0  = x;
                m4  = 0;
                m8  = 0;
                m12 = 0;
                m1  = 0;
                m5  = y;
                m9  = 0;
                m13 = 0;
                m2  = 0;
                m6  = 0;
                m10 = z;
                m14 = 0;
                m3  = 0;
                m7  = 0;
                m11 = 0;
                m15 = 1;

                return *this;
            }

            /** Apply translation into this matrix
             */
            Matrix& applyTranslate(T x, T y, T z)
            {
                static Matrix<T> temp;
                temp.loadTranslate(x, y, z);
                *this = temp.mult4by4(*this);
                return *this;
            }

            /** Make this a translation matrix
             */
            Matrix& loadTranslate(T const x, T const y, T const z)
            {
                m0  = 1;
                m4  = 0;
                m8  = 0;
                m12 = x;
                m1  = 0;
                m5  = 1;
                m9  = 0;
                m13 = y;
                m2  = 0;
                m6  = 0;
                m10 = 1;
                m14 = z;
                m3  = 0;
                m7  = 0;
                m11 = 0;
                m15 = 1;

                return *this;
            }

            /** Transform given point using this matrix
             */
            PointType3D<T> operator*(PointType3D<T> const & vec)
            {
                return PointType3D<T>(
                    (vec.x * m0) + (vec.y * m4) + (vec.z * m8) + m12,
                    (vec.x * m1) + (vec.y * m5) + (vec.z * m9) + m13,
                    (vec.x * m2) + (vec.y * m6) + (vec.z * m10) + m14);
            }

            /** Direct access to the matrix elements, just remember they are in column major format!!
             */
            T& operator[](int32_t ind)
            {
                assert(ind > -1 && ind < 16);
                switch (ind) {
                case 0:
                    return m0;
                case 1:
                    return m1;
                case 2:
                    return m2;
                case 3:
                    return m3;
                case 4:
                    return m4;
                case 5:
                    return m5;
                case 6:
                    return m6;
                case 7:
                    return m7;
                case 8:
                    return m8;
                case 9:
                    return m9;
                case 10:
                    return m10;
                case 11:
                    return m11;
                case 12:
                    return m12;
                case 13:
                    return m13;
                case 14:
                    return m14;
                case 15:
                    return m15;
                default:
                    return m0;
                }
            }
            T const & operator[](int32_t ind) const
            {
                assert(ind > -1 && ind < 16);
                switch (ind) {
                case 0:
                    return m0;
                case 1:
                    return m1;
                case 2:
                    return m2;
                case 3:
                    return m3;
                case 4:
                    return m4;
                case 5:
                    return m5;
                case 6:
                    return m6;
                case 7:
                    return m7;
                case 8:
                    return m8;
                case 9:
                    return m9;
                case 10:
                    return m10;
                case 11:
                    return m11;
                case 12:
                    return m12;
                case 13:
                    return m13;
                case 14:
                    return m14;
                case 15:
                    return m15;
                default:
                    return m0;
                }
            }

            /** Apply the matrix dot product to this matrix
             */
            Matrix& mult3by3(Matrix const & mat)
            {
                Matrix temp(*this);
                m0 = temp.m0 * mat.m0 + temp.m4 * mat.m1 + temp.m8 * mat.m2;
                m4 = temp.m0 * mat.m4 + temp.m4 * mat.m5 + temp.m8 * mat.m6;
                m8 = temp.m0 * mat.m8 + temp.m4 * mat.m9 + temp.m8 * mat.m10;

                m1 = temp.m1 * mat.m0 + temp.m5 * mat.m1 + temp.m9 * mat.m2;
                m5 = temp.m1 * mat.m4 + temp.m5 * mat.m5 + temp.m9 * mat.m6;
                m9 = temp.m1 * mat.m8 + temp.m5 * mat.m9 + temp.m9 * mat.m10;

                m2  = temp.m2 * mat.m0 + temp.m6 * mat.m1 + temp.m10 * mat.m2;
                m6  = temp.m2 * mat.m4 + temp.m6 * mat.m5 + temp.m10 * mat.m6;
                m10 = temp.m2 * mat.m8 + temp.m6 * mat.m9 + temp.m10 * mat.m10;

                m3  = temp.m3 * mat.m0 + temp.m7 * mat.m1 + temp.m11 * mat.m2;
                m7  = temp.m3 * mat.m4 + temp.m7 * mat.m5 + temp.m11 * mat.m6;
                m11 = temp.m3 * mat.m8 + temp.m7 * mat.m9 + temp.m11 * mat.m10;
                return *this;
            }

            /** this->Rmult4by4(temp) == [temp] X [*this] **/
            /** also equal to temp->mult4by4(*this) **/
            Matrix<T>& Rmult4by4(Matrix<T> const & mat)
            {
                Matrix temp(*this);

                m0  = mat.m0 * temp.m0 + mat.m4 * temp.m1 + mat.m8 * temp.m2 + mat.m12 * temp.m3;
                m4  = mat.m0 * temp.m4 + mat.m4 * temp.m5 + mat.m8 * temp.m6 + mat.m12 * temp.m7;
                m8  = mat.m0 * temp.m8 + mat.m4 * temp.m9 + mat.m8 * temp.m10 + mat.m12 * temp.m11;
                m12 = mat.m0 * temp.m12 + mat.m4 * temp.m13 + mat.m8 * temp.m14 + mat.m12 * temp.m15;

                m1  = mat.m1 * temp.m0 + mat.m5 * temp.m1 + mat.m9 * temp.m2 + mat.m13 * temp.m3;
                m5  = mat.m1 * temp.m4 + mat.m5 * temp.m5 + mat.m9 * temp.m6 + mat.m13 * temp.m7;
                m9  = mat.m1 * temp.m8 + mat.m5 * temp.m9 + mat.m9 * temp.m10 + mat.m13 * temp.m11;
                m13 = mat.m1 * temp.m12 + mat.m5 * temp.m13 + mat.m9 * temp.m14 + mat.m13 * temp.m15;

                m2  = mat.m2 * temp.m0 + mat.m6 * temp.m1 + mat.m10 * temp.m2 + mat.m14 * temp.m3;
                m6  = mat.m2 * temp.m4 + mat.m6 * temp.m5 + mat.m10 * temp.m6 + mat.m14 * temp.m7;
                m10 = mat.m2 * temp.m8 + mat.m6 * temp.m9 + mat.m10 * temp.m10 + mat.m14 * temp.m11;
                m14 = mat.m2 * temp.m12 + mat.m6 * temp.m13 + mat.m10 * temp.m14 + mat.m14 * temp.m15;

                m3  = mat.m3 * temp.m0 + mat.m7 * temp.m1 + mat.m11 * temp.m2 + mat.m15 * temp.m3;
                m7  = mat.m3 * temp.m4 + mat.m7 * temp.m5 + mat.m11 * temp.m6 + mat.m15 * temp.m7;
                m11 = mat.m3 * temp.m8 + mat.m7 * temp.m9 + mat.m11 * temp.m10 + mat.m15 * temp.m11;
                m15 = mat.m3 * temp.m12 + mat.m7 * temp.m13 + mat.m11 * temp.m14 + mat.m15 * temp.m15;
                return *this;
            }

            Matrix<T>& mult4by4(Matrix<T> const & mat)
            {
                Matrix temp(*this);

                m0  = temp.m0 * mat.m0 + temp.m4 * mat.m1 + temp.m8 * mat.m2 + temp.m12 * mat.m3;
                m4  = temp.m0 * mat.m4 + temp.m4 * mat.m5 + temp.m8 * mat.m6 + temp.m12 * mat.m7;
                m8  = temp.m0 * mat.m8 + temp.m4 * mat.m9 + temp.m8 * mat.m10 + temp.m12 * mat.m11;
                m12 = temp.m0 * mat.m12 + temp.m4 * mat.m13 + temp.m8 * mat.m14 + temp.m12 * mat.m15;

                m1  = temp.m1 * mat.m0 + temp.m5 * mat.m1 + temp.m9 * mat.m2 + temp.m13 * mat.m3;
                m5  = temp.m1 * mat.m4 + temp.m5 * mat.m5 + temp.m9 * mat.m6 + temp.m13 * mat.m7;
                m9  = temp.m1 * mat.m8 + temp.m5 * mat.m9 + temp.m9 * mat.m10 + temp.m13 * mat.m11;
                m13 = temp.m1 * mat.m12 + temp.m5 * mat.m13 + temp.m9 * mat.m14 + temp.m13 * mat.m15;

                m2  = temp.m2 * mat.m0 + temp.m6 * mat.m1 + temp.m10 * mat.m2 + temp.m14 * mat.m3;
                m6  = temp.m2 * mat.m4 + temp.m6 * mat.m5 + temp.m10 * mat.m6 + temp.m14 * mat.m7;
                m10 = temp.m2 * mat.m8 + temp.m6 * mat.m9 + temp.m10 * mat.m10 + temp.m14 * mat.m11;
                m14 = temp.m2 * mat.m12 + temp.m6 * mat.m13 + temp.m10 * mat.m14 + temp.m14 * mat.m15;

                m3  = temp.m3 * mat.m0 + temp.m7 * mat.m1 + temp.m11 * mat.m2 + temp.m15 * mat.m3;
                m7  = temp.m3 * mat.m4 + temp.m7 * mat.m5 + temp.m11 * mat.m6 + temp.m15 * mat.m7;
                m11 = temp.m3 * mat.m8 + temp.m7 * mat.m9 + temp.m11 * mat.m10 + temp.m15 * mat.m11;
                m15 = temp.m3 * mat.m12 + temp.m7 * mat.m13 + temp.m11 * mat.m14 + temp.m15 * mat.m15;
                return *this;
            }

            Matrix& applyRotate(T angle, T x, T y, T z)
            {
                static Matrix<T> temp;
                temp.loadRotate(angle, x, y, z);
                *this = temp.mult4by4(*this);
                return *this;
            }

        private:
            // TODO cleanup
            static constexpr T cofactorMaker(T f1, T mj1, T mi1, T f2, T mj2, T mi2, T f3, T mj3, T mi3)
            {
                return ((f1) * (((mj1) * (mi1)) - ((mj2) * (mi3)))) + ((f2) * (((mj2) * (mi2)) - ((mj3) * (mi1)))) +
                       ((f3) * (((mj3) * (mi3)) - ((mj1) * (mi2))));
            }

            T cofactorm0() const
            {
                return cofactorMaker(m5, m10, m15, m6, m11, m13, m7, m9, m14);
            }
            T cofactorm1() const
            {
                return cofactorMaker(m6, m11, m12, m7, m8, m14, m4, m10, m15);
            }
            T cofactorm2() const
            {
                return cofactorMaker(m7, m8, m13, m4, m9, m15, m5, m11, m12);
            }
            T cofactorm3() const
            {
                return cofactorMaker(m4, m9, m14, m5, m10, m12, m6, m8, m13);
            }
            T cofactorm4() const
            {
                return cofactorMaker(m9, m14, m3, m10, m15, m1, m11, m13, m2);
            }
            T cofactorm5() const
            {
                return cofactorMaker(m10, m15, m0, m11, m12, m2, m8, m14, m3);
            }
            T cofactorm6() const
            {
                return cofactorMaker(m11, m12, m1, m8, m13, m3, m9, m15, m0);
            }
            T cofactorm7() const
            {
                return cofactorMaker(m8, m13, m2, m9, m14, m0, m10, m12, m1);
            }
            T cofactorm8() const
            {
                return cofactorMaker(m13, m2, m7, m14, m3, m5, m15, m1, m6);
            }
            T cofactorm9() const
            {
                return cofactorMaker(m14, m13, m4, m15, m0, m6, m12, m2, m7);
            }
            T cofactorm10() const
            {
                return cofactorMaker(m15, m0, m5, m12, m1, m7, m13, m3, m4);
            }
            T cofactorm11() const
            {
                return cofactorMaker(m12, m1, m6, m13, m2, m4, m14, m0, m5);
            }
            T cofactorm12() const
            {
                return cofactorMaker(m1, m6, m11, m2, m7, m9, m3, m5, m10);
            }
            T cofactorm13() const
            {
                return cofactorMaker(m2, m7, m8, m3, m4, m10, m10, m6, m11);
            }
            T cofactorm14() const
            {
                return cofactorMaker(m3, m4, m9, m0, m5, m11, m1, m7, m8);
            }
            T cofactorm15() const
            {
                return cofactorMaker(m0, m5, m10, m1, m6, m8, m2, m4, m9);
            }

        public:
            T m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15;
    };

    using DoubleMatrix = Matrix<double>;
    using IntMatrix    = Matrix<int32_t>;
    /** Print coords of the Matrix to a stream
     */
    template <typename T>
    std::ostream& operator<<(std::ostream& os, Matrix<T> const & m)
    {

        return os << "\n|" << m[0] << "," << m[4] << "," << m[8] << "," << m[12] << "|\n"
                  << "|" << m[1] << "," << m[5] << "," << m[9] << "," << m[13] << "|\n"
                  << "|" << m[2] << "," << m[6] << "," << m[10] << "," << m[14] << "|\n"
                  << "|" << m[3] << "," << m[7] << "," << m[11] << "," << m[15] << "|\n";
    }

} // namespace FIFE

#endif
