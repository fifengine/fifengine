/***************************************************************************
 *   Copyright (C) 2005-2010 by the FIFE team                              *
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
%module fife

%{
#include "util/math/fife_math.h"
%}

namespace FIFE {

	static const float FLT_ZERO_TOLERANCE;
	static const float FLT_PI;
	static const float FLT_TWO_PI;
	static const float FLT_HALF_PI;
	static const float FLT_INVERSE_PI;
	static const float FLT_INVERSE_TWO_PI;
	static const float FLT_DEG_TO_RAD;
	static const float FLT_RAD_TO_DEG;
	static const float FLT_LOG_2;
	static const float FLT_LOG_10;
	static const float FLT_INV_LOG_2;
	static const float FLT_INV_LOG_10;

	static const double DBL_ZERO_TOLERANCE;
	static const double DBL_PI;
	static const double DBL_TWO_PI;
	static const double DBL_HALF_PI;
	static const double DBL_INVERSE_PI;
	static const double DBL_INVERSE_TWO_PI;
	static const double DBL_DEG_TO_RAD;
	static const double DBL_RAD_TO_DEG;
	static const double DBL_LOG_2;
	static const double DBL_LOG_10;
	static const double DBL_INV_LOG_2;
	static const double DBL_INV_LOG_10;

}
