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

	template <class numT>
	struct float_traits { };

	template <typename T>
	class Math {
	public:
		typedef T num_type;
		typedef float_traits<num_type> traits_type;

		static inline num_type epsilon();
		static inline num_type zeroTolerance();
		static inline num_type max();
		static inline num_type pi();
		static inline num_type twoPi();
		static inline num_type halfPi();
		static inline num_type inversePi();
		static inline num_type inverseTwoPi();
		static inline num_type degToRad();
		static inline num_type radToDeg();
		static inline num_type log2();
		static inline num_type log10();
		static inline num_type invLog2();
		static inline num_type invLog10();

		static T ACos(T _val);
		static T ASin(T _val);
		static T ATan(T _val);
		static T ATan2(T _x, T _y);
		static T Ceil(T _val);
		static T Cos(T _val);
		static T Exp(T _val);
		static T FAbs(T _val);
		static T Floor(T _val);
		static T FMod (T _x, T _y);
		static T InvSqrt(T _val);
		static T Log(T _val);
		static T Log2(T _val);
		static T Log10(T _val);
		static T Pow(T _base, T _exponent);
		static T Sin(T _val);
		static T Sqr(T _val);
		static T Sqrt(T _val);
		static T Tan(T _val);
	};

	typedef Math<float> Mathf;
	typedef Math<double> Mathd;
	
	%template(Mathf) Math<float>;
	%template(Mathd) Math<double>;

}
