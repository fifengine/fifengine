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


// Sort out the missing round function in MSVC:
#if defined( WIN32 ) && defined( _MSC_VER )
inline double round(const double x) {
	return x < 0.0 ? ceil(x - 0.5) : floor(x + 0.5); 
}
#endif

#ifndef ABS
#define ABS(x) ((x)<0?-(x):(x))

#endif

#ifndef M_PI
#define M_PI        3.14159265358979323846

#endif

#endif // FIFE_UTIL_FIFE_MATH_H
