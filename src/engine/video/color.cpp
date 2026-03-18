// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "color.h"

namespace FIFE
{
    Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) : m_r(r), m_g(g), m_b(b), m_a(alpha) { }

    void Color::set(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
    {
        m_r = r;
        m_g = g;
        m_b = b;
        m_a = alpha;
    }

    void Color::setR(uint8_t r)
    {
        m_r = r;
    }

    void Color::setG(uint8_t g)
    {
        m_g = g;
    }

    void Color::setB(uint8_t b)
    {
        m_b = b;
    }

    void Color::setAlpha(uint8_t alpha)
    {
        m_a = alpha;
    }

    uint8_t Color::getR() const
    {
        return m_r;
    }

    uint8_t Color::getG() const
    {
        return m_g;
    }

    uint8_t Color::getB() const
    {
        return m_b;
    }

    uint8_t Color::getAlpha() const
    {
        return m_a;
    }
} // namespace FIFE