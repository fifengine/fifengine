/***************************************************************************
 *   Copyright (C) 2005-2010 by the FIFE team                              *
 *   http://www.fifengine.net                                               *
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

#ifndef FIFE_UTIL_FIFE_MATH_H
#define FIFE_UTIL_FIFE_MATH_H

// Standard C++ library includes
#include <cmath>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#ifndef ABS
#define ABS(x) ((x)<0?-(x):(x))

#endif

// Sort out the missing round function in MSVC:
#if defined( WIN32 ) && defined( _MSC_VER )
inline double round(const double x) {
	return x < 0.0 ? ceil(x - 0.5) : floor(x + 0.5);
}
#endif

namespace FIFE {

	static const float FLT_ZERO_TOLERANCE = 1e-06f;
	static const float FLT_PI = 4.0f*std::atan(1.0f);
	static const float FLT_TWO_PI = 2.0f*FLT_PI;
	static const float FLT_HALF_PI = 0.5f*FLT_PI;
	static const float FLT_INVERSE_PI = 1.0f/FLT_PI;
	static const float FLT_INVERSE_TWO_PI = 1.0f/FLT_TWO_PI;
	static const float FLT_DEG_TO_RAD = FLT_PI/180.0f;
	static const float FLT_RAD_TO_DEG = 180.0f/FLT_PI;
	static const float FLT_LOG_2 = std::log(2.0f);
	static const float FLT_LOG_10 = std::log(10.0f);
	static const float FLT_INV_LOG_2 = 1.0f/std::log(2.0f);
	static const float FLT_INV_LOG_10 = 1.0f/std::log(10.0f);

	static const double DBL_ZERO_TOLERANCE = 1e-08;
	static const double DBL_PI = 4.0*std::atan(1.0f);
	static const double DBL_TWO_PI = 2.0*DBL_PI;
	static const double DBL_HALF_PI = 0.5*DBL_PI;
	static const double DBL_INVERSE_PI = 1.0/DBL_PI;
	static const double DBL_INVERSE_TWO_PI = 1.0/DBL_TWO_PI;
	static const double DBL_DEG_TO_RAD = DBL_PI/180.0;
	static const double DBL_RAD_TO_DEG = 180.0/DBL_PI;
	static const double DBL_LOG_2 = std::log(2.0f);
	static const double DBL_LOG_10 = std::log(10.0f);
	static const double DBL_INV_LOG_2 = 1.0/std::log(2.0f);
	static const double DBL_INV_LOG_10 = 1.0/std::log(10.0f);

	inline unsigned nextPow2(unsigned x)
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return ++x;
	}
} //FIFE

#endif // FIFE_UTIL_FIFE_MATH_H
