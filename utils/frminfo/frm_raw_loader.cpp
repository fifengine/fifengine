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
#include "map/loaders/fallout/frm.h"
#include "map/loaders/fallout/pal.h"
#include "map/loaders/fallout/animatedpal.h"
#include "vfs/raw/rawdata.h"

#include "frm_raw_loader.h"

namespace FIFE {
	RawFRM::RawFRM(const std::string & fname, map::loaders::fallout::AnimatedPalette* palette) : map::loaders::fallout::FRM(fname, palette) {
	}

	std::pair<uint16_t, uint16_t> RawFRM::getFrameDimension(uint16_t dir, uint16_t frame) {
		FrameInfo & fi = m_frame_info[dir][frame];
		return std::make_pair(fi.width, fi.height);
	}

	uint8_t* RawFRM::getFrameRaw(uint16_t dir, uint16_t frame) {
		FrameInfo & fi = m_frame_info[dir][frame];
		uint32_t size = fi.width * fi.height;
		m_data->setIndex(fi.fpos);
		uint8_t* raw_data = new uint8_t[size];
		uint8_t* buff = raw_data;
		m_data->readInto(raw_data, size);
		return buff;
	}

	uint8_t* RawFRM::getFrameRGBA(uint16_t dir, uint16_t frame) {
		uint8_t* _data = getFrameRaw(dir, frame);
		map::loaders::fallout::PAL palette("color.pal");
		int id = 0;
		uint8_t* dataCopy = _data;

		FrameInfo &fi = m_frame_info[dir][frame];

		uint32_t size = fi.width * fi.height * 4;
		uint8_t* rgba_data = new uint8_t[size];
		uint32_t* pixeldata = reinterpret_cast<uint32_t*>(rgba_data);
		for (uint32_t y = 0; y < fi.height; ++y) {
			for (uint32_t x = 0; x < fi.width; ++x) {
				uint8_t index = *(_data++);
				if (index == 0) {
					*pixeldata = 0x00000000;
				} else {
					//id |= palutil->checkPixel(index);

					uint8_t alpha = 0xff;
					if( index == 108 ) { // 108 is the transparent window pixel index
						alpha = 0x80;
					}
					/** @todo: find solution: 13 should be yellow/red and transparent only for specific objects.
						else
						if( 13 == index ) {	///< 13 is transparent force-field
						alpha = 0x80;
						}
						*/

          /*
					uint8_t red = std::min(palette.getRed(index) * m_custom_gamma, 0xff) & 0xff;
					uint8_t green = std::min(palette.getGreen(index) * m_custom_gamma, 0xff) & 0xff;
					uint8_t blue = std::min(palette.getBlue(index) * m_custom_gamma, 0xff) & 0xff;

					*pixeldata = (red << 24) | (green << 16) | (blue << 8) | alpha;
          */
          *pixeldata = (palette.getRed(index) << 24) | (palette.getGreen(index) << 16) | (palette.getBlue(index) << 8) | alpha;
				}
				++pixeldata;
			}
		}
		delete [] dataCopy;
		return rgba_data;
	}

}
