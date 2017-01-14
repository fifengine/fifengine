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

#ifndef FIFE_UTIL_FIFE_MATH_H
#define FIFE_UTIL_FIFE_MATH_H

// Standard C++ library includes
#include <cassert>
#include <cmath>
#include <limits>
#include <algorithm>

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

	static const float FLT_STD_EPSILON = std::numeric_limits<float>::epsilon();
	static const float FLT_STD_MAX = (std::numeric_limits<float>::max)();
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

	static const double DBL_STD_EPSILON = std::numeric_limits<double>::epsilon();
	static const double DBL_STD_MAX = (std::numeric_limits<double>::max)();
	static const double DBL_ZERO_TOLERANCE = 1e-08;
	static const double DBL_PI = 4.0*std::atan(1.0);
	static const double DBL_TWO_PI = 2.0*DBL_PI;
	static const double DBL_HALF_PI = 0.5*DBL_PI;
	static const double DBL_INVERSE_PI = 1.0/DBL_PI;
	static const double DBL_INVERSE_TWO_PI = 1.0/DBL_TWO_PI;
	static const double DBL_DEG_TO_RAD = DBL_PI/180.0;
	static const double DBL_RAD_TO_DEG = 180.0f/DBL_PI;
	static const double DBL_LOG_2 = std::log(2.0);
	static const double DBL_LOG_10 = std::log(10.0);
	static const double DBL_INV_LOG_2 = 1.0/std::log(2.0);
	static const double DBL_INV_LOG_10 = 1.0/std::log(10.0);

	template <class numT>
	struct float_traits { };

	template <>
	struct float_traits<float> {
		typedef float float_type;
		static inline float_type epsilon() { return FLT_STD_EPSILON; }
		static inline float_type zeroTolerance() { return FLT_ZERO_TOLERANCE; }
		static inline float_type max() { return FLT_STD_MAX; }
		static inline float_type pi() { return FLT_PI; }
		static inline float_type twoPi() { return FLT_TWO_PI; }
		static inline float_type halfPi() { return FLT_HALF_PI; }
		static inline float_type inversePi() { return FLT_INVERSE_PI; }
		static inline float_type inverseTwoPi() { return FLT_INVERSE_TWO_PI; }
		static inline float_type degToRad() { return FLT_DEG_TO_RAD; }
		static inline float_type radToDeg() { return FLT_RAD_TO_DEG; }
		static inline float_type log2() { return FLT_LOG_2; }
		static inline float_type log10() { return FLT_LOG_10; }
		static inline float_type invLog2() { return FLT_INV_LOG_2; }
		static inline float_type invLog10() { return FLT_INV_LOG_10; }
	};

	template <>
	struct float_traits<double> {
		typedef double float_type;
		static inline float_type epsilon() { return DBL_STD_EPSILON; }
		static inline float_type zeroTolerance() { return DBL_ZERO_TOLERANCE; }
		static inline float_type max() { return DBL_STD_MAX; }
		static inline float_type pi() { return DBL_PI; }
		static inline float_type twoPi() { return DBL_TWO_PI; }
		static inline float_type halfPi() { return DBL_HALF_PI; }
		static inline float_type inversePi() { return DBL_INVERSE_PI; }
		static inline float_type inverseTwoPi() { return DBL_INVERSE_TWO_PI; }
		static inline float_type degToRad() { return DBL_DEG_TO_RAD; }
		static inline float_type radToDeg() { return DBL_RAD_TO_DEG; }
		static inline float_type log2() { return DBL_LOG_2; }
		static inline float_type log10() { return DBL_LOG_10; }
		static inline float_type invLog2() { return DBL_INV_LOG_2; }
		static inline float_type invLog10() { return DBL_INV_LOG_10; }
	};

	template <typename T>
	class Math {
	public:
		typedef T num_type;
		typedef float_traits<num_type> traits_type;

		static inline num_type epsilon() { return traits_type::epsilon(); }
		static inline num_type zeroTolerance() { return traits_type::zeroTolerance(); }
		static inline num_type max() { return traits_type::max(); }
		static inline num_type pi() { return traits_type::pi(); }
		static inline num_type twoPi() { return traits_type::twoPi(); }
		static inline num_type halfPi() { return traits_type::halfPi(); }
		static inline num_type inversePi() { return traits_type::inversePi(); }
		static inline num_type inverseTwoPi() { return traits_type::inverseTwoPi(); }
		static inline num_type degToRad() { return traits_type::degToRad(); }
		static inline num_type radToDeg() { return traits_type::radToDeg(); }
		static inline num_type log2() { return traits_type::log2(); }
		static inline num_type log10() { return traits_type::log10(); }
		static inline num_type invLog2() { return traits_type::invLog2(); }
		static inline num_type invLog10() { return traits_type::invLog10(); }

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
		static bool Equal(T _val1, T _val2);
	};

	typedef Math<float> Mathf;
	typedef Math<double> Mathd;

	template<typename T>
	inline T Math<T>::ACos(T _val) {
		if (-static_cast<T>(1) < _val) {
			if (_val < static_cast<T>(1)) {
				return static_cast<T>(std::acos(_val));
			}
			else {
				return static_cast<T>(0);
			}
		}
		else {
			return pi();
		}
	}

	template <class T>
	inline T Math<T>::ASin(T _val) {
		if (-static_cast<T>(1) < _val) {
			if (_val < static_cast<T>(1)) {
				return static_cast<T>(std::asin(_val));
			}
			else {
				return halfPi();
			}
		}
		else {
			return -halfPi();
		}
	}

	template <class T>
	inline T Math<T>::ATan(T _val) {
		return static_cast<T>(std::atan(_val));
	}

	template <class T>
	inline T Math<T>::ATan2(T _x, T _y) {
		return static_cast<T>(std::atan2(_x, _y));
	}

	template <class T>
	inline T Math<T>::Ceil(T _val) {
		return static_cast<T>(std::ceil(_val));
	}

	template <class T>
	inline T Math<T>::Cos(T _val) {
		return static_cast<T>(std::cos(_val));
	}

	template <class T>
	inline T Math<T>::Exp(T _val){
		return static_cast<T>(std::exp(_val));
	}

	template <class T>
	inline T Math<T>::FAbs(T _val) {
		return static_cast<T>(std::fabs(_val));
	}

	template <class T>
	inline T Math<T>::Floor(T _val) {
		return static_cast<T>(std::floor(_val));
	}

	template <class T>
	inline T Math<T>::FMod(T _x, T _y) {
		return static_cast<T>(std::fmod(_x, _y));
	}

	template <class T>
	inline T Math<T>::InvSqrt(T _val) {
		return static_cast<T>(1/std::sqrt(_val));
	}

	template <class T>
	inline T Math<T>::Log(T _val) {
		return static_cast<T>(std::log(_val));
	}

	template <class T>
	inline T Math<T>::Log2(T _val) {
		return invLog2() * static_cast<T>(std::log(_val));
	}
	template <class T>
	inline T Math<T>::Log10(T _val) {

		return invLog10() * static_cast<T>(std::log(_val));
	}

	template <class T>
	inline T Math<T>::Pow(T _base, T _exponent) {
		return static_cast<T>(std::pow(_base, _exponent));
	}

	template <class T>
	inline T Math<T>::Sin(T _val) {
		return static_cast<T>(std::sin(_val));
	}

	template <class T>
	inline T Math<T>::Sqr(T _val) {
		return _val*_val;
	}

	template <class T>
	inline T Math<T>::Sqrt(T _val) {
		return static_cast<T>(std::sqrt(_val));
	}

	template <class T>
	inline T Math<T>::Tan(T _val) {
		return static_cast<T>(std::tan(_val));
	}

	template <class T>
	inline bool Math<T>::Equal(T _val1, T _val2) {
		return std::fabs(_val1 - _val2) < epsilon();
	}

	/** Returns the next higher power of 2 based on the passed argument
	 */
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
