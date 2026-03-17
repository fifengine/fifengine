// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_UTIL_FIFE_MATH_H
#define FIFE_UTIL_FIFE_MATH_H

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <numbers>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE
{

    static float const FLT_STD_EPSILON    = std::numeric_limits<float>::epsilon();
    static float const FLT_STD_MAX        = std::numeric_limits<float>::max();
    static float const FLT_ZERO_TOLERANCE = 1e-06F;
    static float const FLT_PI             = 4.0F * std::atan(1.0F);
    static float const FLT_TWO_PI         = 2.0F * FLT_PI;
    static float const FLT_HALF_PI        = 0.5F * FLT_PI;
    static float const FLT_INVERSE_PI     = 1.0F / FLT_PI;
    static float const FLT_INVERSE_TWO_PI = 1.0F / FLT_TWO_PI;
    static float const FLT_DEG_TO_RAD     = FLT_PI / 180.0F;
    static float const FLT_RAD_TO_DEG     = 180.0F / FLT_PI;
    static float const FLT_LOG_2          = std::numbers::ln2_v<float>;
    static float const FLT_LOG_10         = std::numbers::ln10_v<float>;
    static float const FLT_INV_LOG_2      = 1.0F / std::numbers::ln2_v<float>;
    static float const FLT_INV_LOG_10     = 1.0F / std::numbers::ln10_v<float>;

    static double const DBL_STD_EPSILON    = std::numeric_limits<double>::epsilon();
    static double const DBL_STD_MAX        = std::numeric_limits<double>::max();
    static double const DBL_ZERO_TOLERANCE = 1e-08;
    static double const DBL_PI             = 4.0 * std::atan(1.0);
    static double const DBL_TWO_PI         = 2.0 * DBL_PI;
    static double const DBL_HALF_PI        = 0.5 * DBL_PI;
    static double const DBL_INVERSE_PI     = 1.0 / DBL_PI;
    static double const DBL_INVERSE_TWO_PI = 1.0 / DBL_TWO_PI;
    static double const DBL_DEG_TO_RAD     = DBL_PI / 180.0;
    static double const DBL_RAD_TO_DEG     = 180.0F / DBL_PI;
    static double const DBL_LOG_2          = std::numbers::ln2;
    static double const DBL_LOG_10         = std::numbers::ln10;
    static double const DBL_INV_LOG_2      = 1.0 / std::numbers::ln2;
    static double const DBL_INV_LOG_10     = 1.0 / std::numbers::ln10;

    /**
     * Generic template for floating-point type traits.
     *
     * Specializations provide constants and helper functions for specific
     * floating-point types.
     *
     * @ingroup utils
     */
    template <class numT>
    struct float_traits
    {
    };

    /**
     * Specialization of float traits for float.
     *
     * float_traits specialization for `float`.
     * Provides useful constants and conversion helpers.
     */
    template <>
    struct float_traits<float>
    {
        using float_type = float; ///< Floating-point typedef for this specialization.

        /** Machine epsilon for float. */
        static float_type epsilon()
        {
            return FLT_STD_EPSILON;
        }
        /** Recommended zero tolerance for float comparisons. */
        static float_type zeroTolerance()
        {
            return FLT_ZERO_TOLERANCE;
        }
        /** Maximum finite float value. */
        static float_type max()
        {
            return FLT_STD_MAX;
        }
        /** Pi constant for floats. */
        static float_type pi()
        {
            return FLT_PI;
        }
        /** Two times pi. */
        static float_type twoPi()
        {
            return FLT_TWO_PI;
        }
        /** Half of pi. */
        static float_type halfPi()
        {
            return FLT_HALF_PI;
        }
        /** 1/pi constant. */
        static float_type inversePi()
        {
            return FLT_INVERSE_PI;
        }
        /** 1/(2*pi) constant. */
        static float_type inverseTwoPi()
        {
            return FLT_INVERSE_TWO_PI;
        }
        /** Degrees to radians conversion factor. */
        static float_type degToRad()
        {
            return FLT_DEG_TO_RAD;
        }
        /** Radians to degrees conversion factor. */
        static float_type radToDeg()
        {
            return FLT_RAD_TO_DEG;
        }
        /** Natural log base 2 for floats. */
        static float_type log2()
        {
            return FLT_LOG_2;
        }
        /** Natural log base 10 for floats. */
        static float_type log10()
        {
            return FLT_LOG_10;
        }
        /** 1 / ln(2) for float conversions. */
        static float_type invLog2()
        {
            return FLT_INV_LOG_2;
        }
        /** 1 / ln(10) for float conversions. */
        static float_type invLog10()
        {
            return FLT_INV_LOG_10;
        }
    };

    /**
     * Specialization of float traits for double.
     *
     * float_traits specialization for `double`.
     * Provides useful constants and conversion helpers.
     */
    template <>
    struct float_traits<double>
    {
        using float_type = double; ///< Floating-point typedef for this specialization.

        /** Machine epsilon for double. */
        static float_type epsilon()
        {
            return DBL_STD_EPSILON;
        }
        /** Recommended zero tolerance for double comparisons. */
        static float_type zeroTolerance()
        {
            return DBL_ZERO_TOLERANCE;
        }
        /** Maximum finite double value. */
        static float_type max()
        {
            return DBL_STD_MAX;
        }
        /** Pi constant for double. */
        static float_type pi()
        {
            return DBL_PI;
        }
        /** Two times pi. */
        static float_type twoPi()
        {
            return DBL_TWO_PI;
        }
        /** Half of pi. */
        static float_type halfPi()
        {
            return DBL_HALF_PI;
        }
        /** 1/pi constant. */
        static float_type inversePi()
        {
            return DBL_INVERSE_PI;
        }
        /** 1/(2*pi) constant. */
        static float_type inverseTwoPi()
        {
            return DBL_INVERSE_TWO_PI;
        }
        /** Degrees to radians conversion factor. */
        static float_type degToRad()
        {
            return DBL_DEG_TO_RAD;
        }
        /** Radians to degrees conversion factor. */
        static float_type radToDeg()
        {
            return DBL_RAD_TO_DEG;
        }
        /** Natural log base 2 for doubles. */
        static float_type log2()
        {
            return DBL_LOG_2;
        }
        /** Natural log base 10 for doubles. */
        static float_type log10()
        {
            return DBL_LOG_10;
        }
        /** 1 / ln(2) for double conversions. */
        static float_type invLog2()
        {
            return DBL_INV_LOG_2;
        }
        /** 1 / ln(10) for double conversions. */
        static float_type invLog10()
        {
            return DBL_INV_LOG_10;
        }
    };

    /**
     * Template utility providing math functions and constants for numeric type T.
     *
     * Utility math functions and constants templated on numeric type T.
     * Use `Mathf` and `Mathd` for float and double respectively.
     *
     * @ingroup utils
     */
    template <typename T>
    class Math
    {
    public:
        /**
         * Type definitions
         */
        using num_type = T;

        /** Trait alias providing numeric constants and helpers for `num_type`. */
        using traits_type = float_traits<num_type>;

        /** Returns machine epsilon for the numeric type. */
        static num_type epsilon()
        {
            return traits_type::epsilon();
        }

        /** Recommended zero tolerance for comparisons. */
        static num_type zeroTolerance()
        {
            return traits_type::zeroTolerance();
        }

        /** Maximum finite value for the numeric type. */
        static num_type max()
        {
            return traits_type::max();
        }

        /** Pi constant. */
        static num_type pi()
        {
            return traits_type::pi();
        }

        /** Two times pi. */
        static num_type twoPi()
        {
            return traits_type::twoPi();
        }

        /** Half of pi. */
        static num_type halfPi()
        {
            return traits_type::halfPi();
        }

        /** 1/pi constant. */
        static num_type inversePi()
        {
            return traits_type::inversePi();
        }

        /** 1/(2*pi) constant. */
        static num_type inverseTwoPi()
        {
            return traits_type::inverseTwoPi();
        }

        /** Degrees-to-radians factor. */
        static num_type degToRad()
        {
            return traits_type::degToRad();
        }

        /** Radians-to-degrees factor. */
        static num_type radToDeg()
        {
            return traits_type::radToDeg();
        }

        /** Natural log base 2. */
        static num_type log2()
        {
            return traits_type::log2();
        }

        /** Natural log base 10. */
        static num_type log10()
        {
            return traits_type::log10();
        }

        /** 1/ln(2) helper. */
        static num_type invLog2()
        {
            return traits_type::invLog2();
        }

        /** 1/ln(10) helper. */
        static num_type invLog10()
        {
            return traits_type::invLog10();
        }

        /** Arc cosine (clamped). */
        static T ACos(T _val);

        /** Arc sine (clamped). */
        static T ASin(T _val);

        /** Arc tangent. */
        static T ATan(T _val);

        /** Two-argument arc tangent. */
        static T ATan2(T _x, T _y);

        /** Ceiling of value. */
        static T Ceil(T _val);

        /** Cosine of value. */
        static T Cos(T _val);

        /** Exponential e^x. */
        static T Exp(T _val);

        /** Absolute value (floating). */
        static T FAbs(T _val);

        /** Floor of value. */
        static T Floor(T _val);

        /** Floating-point modulus. */
        static T FMod(T _x, T _y);

        /** Inverse square root (1/sqrt(x)). */
        static T InvSqrt(T _val);

        /** Natural logarithm. */
        static T Log(T _val);

        /** Log base 2. */
        static T Log2(T _val);

        /** Log base 10. */
        static T Log10(T _val);

        /** Power function. */
        static T Pow(T _base, T _exponent);

        /** Sine of value. */
        static T Sin(T _val);

        /** Square of value. */
        static T Sqr(T _val);

        /** Square root. */
        static T Sqrt(T _val);

        /** Tangent of value. */
        static T Tan(T _val);

        /** Approximate equality check using epsilon. */
        static bool Equal(T _val1, T _val2);
    };

    using Mathf = Math<float>;
    using Mathd = Math<double>;

    template <typename T>
    inline T Math<T>::ACos(T _val)
    {
        if (-static_cast<T>(1) < _val) {
            if (_val < static_cast<T>(1)) {
                return static_cast<T>(std::acos(_val));
            }

            return static_cast<T>(0);
        }

        return pi();
    }

    template <class T>
    inline T Math<T>::ASin(T _val)
    {
        if (-static_cast<T>(1) < _val) {
            if (_val < static_cast<T>(1)) {
                return static_cast<T>(std::asin(_val));
            }

            return halfPi();
        }

        return -halfPi();
    }

    template <class T>
    inline T Math<T>::ATan(T _val)
    {
        return static_cast<T>(std::atan(_val));
    }

    template <class T>
    inline T Math<T>::ATan2(T _x, T _y)
    {
        return static_cast<T>(std::atan2(_x, _y));
    }

    template <class T>
    inline T Math<T>::Ceil(T _val)
    {
        return static_cast<T>(std::ceil(_val));
    }

    template <class T>
    inline T Math<T>::Cos(T _val)
    {
        return static_cast<T>(std::cos(_val));
    }

    template <class T>
    inline T Math<T>::Exp(T _val)
    {
        return static_cast<T>(std::exp(_val));
    }

    template <class T>
    inline T Math<T>::FAbs(T _val)
    {
        return static_cast<T>(std::fabs(_val));
    }

    template <class T>
    inline T Math<T>::Floor(T _val)
    {
        return static_cast<T>(std::floor(_val));
    }

    template <class T>
    inline T Math<T>::FMod(T _x, T _y)
    {
        return static_cast<T>(std::fmod(_x, _y));
    }

    template <class T>
    inline T Math<T>::InvSqrt(T _val)
    {
        return static_cast<T>(1 / std::sqrt(_val));
    }

    template <class T>
    inline T Math<T>::Log(T _val)
    {
        return static_cast<T>(std::log(_val));
    }

    template <class T>
    inline T Math<T>::Log2(T _val)
    {
        return invLog2() * static_cast<T>(std::log(_val));
    }
    template <class T>
    inline T Math<T>::Log10(T _val)
    {

        return invLog10() * static_cast<T>(std::log(_val));
    }

    template <class T>
    inline T Math<T>::Pow(T _base, T _exponent)
    {
        return static_cast<T>(std::pow(_base, _exponent));
    }

    template <class T>
    inline T Math<T>::Sin(T _val)
    {
        return static_cast<T>(std::sin(_val));
    }

    template <class T>
    inline T Math<T>::Sqr(T _val)
    {
        return _val * _val;
    }

    template <class T>
    inline T Math<T>::Sqrt(T _val)
    {
        return static_cast<T>(std::sqrt(_val));
    }

    template <class T>
    inline T Math<T>::Tan(T _val)
    {
        return static_cast<T>(std::tan(_val));
    }

    template <class T>
    inline bool Math<T>::Equal(T _val1, T _val2)
    {
        return std::fabs(_val1 - _val2) < epsilon();
    }

    /**
     * Returns the next higher power of 2 based on the passed argument.
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
} // namespace FIFE

#endif // FIFE_UTIL_FIFE_MATH_H
