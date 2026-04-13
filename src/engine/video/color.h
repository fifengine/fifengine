// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_COLOR_H
#define FIFE_VIDEO_COLOR_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include "util/base/fife_stdint.h"
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes

namespace FIFE
{
    class /*FIFE_API*/ Color
    {
    public:
        /** Constructor.
         *
         * @param r Red channel value.
         * @param g Green channel value.
         * @param b Blue channel value.
         * @param alpha Alpha channel value.
         */
        explicit Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t alpha = 255);

        /** Destructor
         */
        ~Color() = default;

        Color(const Color&)            = default;
        Color& operator=(const Color&) = default;
        Color(Color&&)                 = default;
        Color& operator=(Color&&)      = default;

        /** Compares equality of two colors
         */
        bool operator==(const Color& color) const
        {
            return (m_r == color.m_r && m_g == color.m_g && m_b == color.m_b && m_a == color.m_a);
        }

        /** Compares unequality of two colors
         */
        bool operator!=(const Color& color) const
        {
            return !(*this == color);
        }

        /** Overload less operator
         */
        bool operator<(const Color& rhs) const
        {
            if (m_r != rhs.m_r) {
                return m_r < rhs.m_r;
            }
            if (m_g != rhs.m_g) {
                return m_g < rhs.m_g;
            }
            if (m_b != rhs.m_b) {
                return m_b < rhs.m_b;
            }
            return m_a < rhs.m_a;
        }

        /** Set all color channel values.
         *
         * @param r Red channel value.
         * @param g Green channel value.
         * @param b Blue channel value.
         * @param alpha Alpha channel value.
         */
        void set(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);

        /** Set red channel value.
         *
         * @param r New value.
         */
        void setR(uint8_t r);

        /** Set green channel value.
         *
         * @param g New value.
         */
        void setG(uint8_t g);

        /** Set blue channel value.
         *
         * @param b New value.
         */
        void setB(uint8_t b);

        /** Set alpha channel value.
         *
         * @param alpha New value.
         */
        void setAlpha(uint8_t alpha);

        /** @return Red channel value.
         */
        uint8_t getR() const;

        /** @return Green channel value.
         */
        uint8_t getG() const;

        /** @return Blue channel value.
         */
        uint8_t getB() const;

        /** @return Alpha channel value.
         */
        uint8_t getAlpha() const;

    private:
        uint8_t m_r;
        uint8_t m_g;
        uint8_t m_b;
        uint8_t m_a;
    };
} // namespace FIFE

#endif // FIFE_VIDEO_COLOR_H
