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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "sdlblendingfunctions.h"

namespace FIFE {

	struct ColorRGB8 {
		uint8_t r, g, b;
	};

	struct ColorRGBA8 {
		uint8_t r, g, b, a;
	};

	void SDL_BlendRow_RGBA8_to_RGBA8( const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n ) {
		const ColorRGBA8* srcColor = reinterpret_cast< const ColorRGBA8* >( src );
		ColorRGBA8* dstColor = reinterpret_cast< ColorRGBA8* >( dst );

		for( int32_t i = n; 0 < i; --i ) {
			register uint32_t aMulA = alpha * srcColor->a;

			if( aMulA ) {
				register uint32_t OneMin_aMulA = 65535 - aMulA;
				dstColor->r = ( aMulA * srcColor->r + OneMin_aMulA * dstColor->r ) >> 16;
				dstColor->g = ( aMulA * srcColor->g + OneMin_aMulA * dstColor->g ) >> 16;
				dstColor->b = ( aMulA * srcColor->b + OneMin_aMulA * dstColor->b ) >> 16;
				dstColor->a = 255;
			}
			++dstColor;
			++srcColor;
		}
	}

	void SDL_BlendRow_RGBA8_to_RGB8( const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n ) {
		const ColorRGBA8* srcColor = reinterpret_cast< const ColorRGBA8* >( src );
		ColorRGB8* dstColor = reinterpret_cast< ColorRGB8* >( dst );

		for( int32_t i = n; 0 < i; --i ) {
			register uint32_t aMulA = alpha * srcColor->a;
			if( aMulA ) {
				register uint32_t OneMin_aMulA = 65535 - aMulA;
				dstColor->r = ( aMulA * srcColor->r + OneMin_aMulA * dstColor->r ) >> 16;
				dstColor->g = ( aMulA * srcColor->g + OneMin_aMulA * dstColor->g ) >> 16;
				dstColor->b = ( aMulA * srcColor->b + OneMin_aMulA * dstColor->b ) >> 16;
			}

			++dstColor;
			++srcColor;
		}
	}

	void SDL_BlendRow_RGBA8_to_RGB565( const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n ) {
		const ColorRGBA8* srcColor = reinterpret_cast< const ColorRGBA8* >( src );
		uint16_t* dstColor = reinterpret_cast< uint16_t* >( dst );

		for( int32_t i = n; 0 < i; --i ) {
			register uint32_t aMulA = ( alpha * srcColor->a ) >> 8;
			if( aMulA ) {
				register uint32_t OneMin_aMulA = 255 - aMulA;
				register uint32_t c = *dstColor;
				*dstColor = ( ( ( srcColor->b * aMulA ) +
					( ( ( c & 0xF800 ) >> 8 ) * OneMin_aMulA ) ) & 0xF800 ) |
					( ( ( ( srcColor->g * aMulA ) +
					( ( ( c & 0x07E0 ) >> 3 ) * OneMin_aMulA ) ) >> 5 ) & 0x07E0 ) |
					( ( ( ( srcColor->r * aMulA ) +
					( ( ( c & 0x001F ) << 3 ) * OneMin_aMulA ) ) >> 11 ) & 0x001F );
			}

			++dstColor;
			++srcColor;
		}
	}


	void SDL_BlendRow_RGBA4_to_RGB565( const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n ) {
		const uint16_t* srcColor = reinterpret_cast< const uint16_t* >( src );
		uint16_t* dstColor = reinterpret_cast< uint16_t* >( dst );

		for( int32_t i = n; 0 < i; --i ) {
			register uint32_t c1 = *dstColor;
			register uint32_t c2 = *srcColor;

			uint32_t aMulA = c2 & 0xF;
			aMulA = ( alpha * aMulA ) / 15;	///< upgrade to range 0-255
			if( aMulA ) {
				register uint32_t OneMin_aMulA = 255 - aMulA;
				register uint32_t result;
				result = ( ( ( ( c2 & 0xF000 ) | 0x0800 ) * aMulA ) + ( ( c1 & 0xF800 ) * OneMin_aMulA ) ) & 0xF80000;
				result |= ( ( ( ( ( c2 & 0x0F00 ) >> 1 ) | 0x0040 ) * aMulA ) + ( ( c1 & 0x07E0 ) * OneMin_aMulA ) ) & 0x07E000;
				result |= ( ( ( ( ( c2 & 0x00F0 ) >> 3 ) | 0x0001 ) * aMulA ) + ( ( c1 & 0x001F ) * OneMin_aMulA ) ) & 0x001F00;
				/// multiplying by alpha resulted in shift.
				*dstColor = static_cast< uint16_t >( result >> 8 );
			}

			++dstColor;
			++srcColor;
		}
	}

}
