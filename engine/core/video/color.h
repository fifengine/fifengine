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

#ifndef FIFE_VIDEO_COLOR_H
#define FIFE_VIDEO_COLOR_H

// Standard C++ library includes
#include <string>
#include <vector>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
	class Color {
	public:
		
		/** Constructor.
		 * 
		 * @param r Red channel value.
		 * @param g Green channel value.
		 * @param b Blue channel value.
		 * @param alpha Alpha channel value.
		 */
		Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t alpha = 255);
		
		/** Destructor
		 */
		~Color();
		
		/** Compares equality of two colors
		 */
		inline bool operator==(const Color& color) const {
			return (m_r == color.m_r && m_g == color.m_g && m_b == color.m_b && m_a == color.m_a);
		}
		
		/** Compares unequality of two colors
		 */
		inline bool operator!=(const Color& color) const {
			return !(*this == color);
		}

		/** Overload less operator
		 */
		inline bool operator<(const Color& rhs) const {
			if (m_r != rhs.m_r) {
				return m_r < rhs.m_r;
			} else if (m_g != rhs.m_g) {
				return m_g < rhs.m_g;
			} else if (m_b != rhs.m_b) {
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
}

#endif //FIFE_VIDEO_COLOR_H