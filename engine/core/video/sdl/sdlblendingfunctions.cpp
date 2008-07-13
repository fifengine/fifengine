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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "sdlblendingfunctions.h"

namespace FIFE {

	struct ColorRGB8 {
		unsigned char r, g, b;
	};

	struct ColorRGBA8 {
		unsigned char r, g, b, a;
	};

	void SDL_BlendRow_RGBA8_to_RGBA8( const unsigned char* src, unsigned char* dst, unsigned int alpha, int n ) {
		const ColorRGBA8* srcColor = reinterpret_cast< const ColorRGBA8* >( src );
		ColorRGBA8* dstColor = reinterpret_cast< ColorRGBA8* >( dst );

		for( int i = n; 0 < i; --i ) {
			register unsigned int aMulA = alpha * srcColor->a;

			if( aMulA ) {
				register unsigned int OneMin_aMulA = 65535 - aMulA;
				dstColor->r = ( aMulA * srcColor->r + OneMin_aMulA * dstColor->r ) >> 16;
				dstColor->g = ( aMulA * srcColor->g + OneMin_aMulA * dstColor->g ) >> 16;
				dstColor->b = ( aMulA * srcColor->b + OneMin_aMulA * dstColor->b ) >> 16;
				dstColor->a = 255;
			}
			++dstColor;
			++srcColor;
		}
	}

	void SDL_BlendRow_RGBA8_to_RGB8( const unsigned char* src, unsigned char* dst, unsigned int alpha, int n ) {
		const ColorRGBA8* srcColor = reinterpret_cast< const ColorRGBA8* >( src );
		ColorRGB8* dstColor = reinterpret_cast< ColorRGB8* >( dst );

		for( int i = n; 0 < i; --i ) {
			register unsigned int aMulA = alpha * srcColor->a;
			if( aMulA ) {
				register unsigned int OneMin_aMulA = 65535 - aMulA;
				dstColor->r = ( aMulA * srcColor->r + OneMin_aMulA * dstColor->r ) >> 16;
				dstColor->g = ( aMulA * srcColor->g + OneMin_aMulA * dstColor->g ) >> 16;
				dstColor->b = ( aMulA * srcColor->b + OneMin_aMulA * dstColor->b ) >> 16;
			}

			++dstColor;
			++srcColor;
		}
	}

	void SDL_BlendRow_RGBA8_to_RGB565( const unsigned char* src, unsigned char* dst, unsigned int alpha, int n ) {
		const ColorRGBA8* srcColor = reinterpret_cast< const ColorRGBA8* >( src );
		unsigned short* dstColor = reinterpret_cast< unsigned short* >( dst );

		for( int i = n; 0 < i; --i ) {
			register unsigned int aMulA = ( alpha * srcColor->a ) >> 8;
			if( aMulA ) {
				register unsigned int OneMin_aMulA = 255 - aMulA;
				register unsigned int c = *dstColor;
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


	void SDL_BlendRow_RGBA4_to_RGB565( const unsigned char* src, unsigned char* dst, unsigned int alpha, int n ) {
		const unsigned short* srcColor = reinterpret_cast< const unsigned short* >( src );
		unsigned short* dstColor = reinterpret_cast< unsigned short* >( dst );

		for( int i = n; 0 < i; --i ) {
			register unsigned int c1 = *dstColor;
			register unsigned int c2 = *srcColor;

			unsigned int aMulA = c2 & 0xF;
			aMulA = ( alpha * aMulA ) / 15;	///< upgrade to range 0-255
			if( aMulA ) {
				register unsigned int OneMin_aMulA = 255 - aMulA;
				register unsigned int result;
				result = ( ( ( ( c2 & 0xF000 ) | 0x0800 ) * aMulA ) + ( ( c1 & 0xF800 ) * OneMin_aMulA ) ) & 0xF80000;
				result |= ( ( ( ( ( c2 & 0x0F00 ) >> 1 ) | 0x0040 ) * aMulA ) + ( ( c1 & 0x07E0 ) * OneMin_aMulA ) ) & 0x07E000;
				result |= ( ( ( ( ( c2 & 0x00F0 ) >> 3 ) | 0x0001 ) * aMulA ) + ( ( c1 & 0x001F ) * OneMin_aMulA ) ) & 0x001F00;
				/// multiplying by alpha resulted in shift.
				*dstColor = static_cast< unsigned short int >( result >> 8 );
			}

			++dstColor;
			++srcColor;
		}
	}

}
