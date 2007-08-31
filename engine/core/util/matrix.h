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
/***************************************************************************
 * Includes some heavy copying from mathgl-pp project                      *
 * (http://sourceforge.net/projects/mathgl-pp/)                            *
 ***************************************************************************/

#ifndef FIFE_UTIL_MATRIX_H
#define FIFE_UTIL_MATRIX_H

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
#include "point.h"

namespace FIFE {


	/** Minimal matrix class to assist in view 3d calculations
	 */
	template <typename T>
	class Matrix {
	public:
    		Matrix() {}
		~Matrix() {}


		/** Make this a rotation matrix
		 */
		inline Matrix& loadRotate(T angle, T x, T y, T z)
		{
			register T magSqr = x*x + y*y + z*z;
			if(magSqr != 1.0) {
				register T mag = sqrt(magSqr);
				x/=mag;
				y/=mag;
				z/=mag;
			}
			T c = cos(angle*M_PI/180);
			T s = sin(angle*M_PI/180);
			m0 = x*x*(1-c)+c;
			m1 = y*x*(1-c)+z*s;
			m2 = z*x*(1-c)-y*s;
			m3 = 0;
		
			m4 = x*y*(1-c)-z*s;
			m5 = y*y*(1-c)+c;
			m6 = z*y*(1-c)+x*s;
			m7 = 0;
		
			m8 = x*z*(1-c)+y*s;
			m9 = y*z*(1-c)-x*s;
			m10 = z*z*(1-c)+c;
			m11 = 0;
		
			m12 = 0;
			m13 = 0;
			m14 = 0;
			m15 = 1;
		
			return *this;
		}

		/** Apply scale into this matrix
		 */
		inline Matrix& applyScale(T x, T y, T z) {
			m0*=x;    m1*=x;    m2*=x;    m3*=x;
			m4*=y;    m5*=y;    m6*=y;    m7*=y;
			m8*=z;    m9*=z;    m10*=z;   m11*=z;
			return *this;
		}

		/** Apply translation into this matrix
		 */
		inline Matrix& applyTranslate(T x, T y, T z) {
			m12 += m0*x + m4*y + m8*z;
			m13 += m1*x + m5*y + m9*z;
			m14 += m2*x + m6*y + m10*z;
			return *this;
		}

		/** Transform given point using this matrix
		 */
		inline PointType3D<T> operator* (const PointType3D<T>& vec) {
			PointType3D<T> ret(T(0));
			for(register unsigned j = 0; j < 3; ++j)
				for(register unsigned i = 0; i < 3; ++i)
					ret.val[j] += vec.val[i]*m[j+i*4]; //scale and rotate disregarding w scaling
		
			for(register unsigned i = 0; i < 3; ++i)
				ret.val[i] += m[i+3*4]; //translate
		
			//do w scaling
			T w = m[15];
			for(register unsigned i = 0; i < 3; ++i)
				w += vec.val[i]*m[3+i*4];
		
			register T resip = 1/w;
		
			for(register unsigned i = 0; i < 3; ++i)
				ret[i] *= resip;
		
			return ret;
		}

	private:
		union {
			T m[16];
			struct {
				T m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15;
			};
		};
	};

	typedef Matrix<double> DoubleMatrix;
	typedef Matrix<int> IntMatrix;
}

#endif
