/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <boost/scoped_array.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/animation.h"
#include "video/renderbackend.h"
#include "video/rendermanager.h"
#include "debugutils.h"
#include "imagecache.h"
#include "log.h"

#include "pal.h"
#include "palutil.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	namespace detail {
		// Get the log base 2 of certain values (powers of 2 that fit in a byte)
		int ln2(int pot) {
			for (int i = 0; i != 8; ++i) {
				if (pot == (1 << i)) {
					return i;
				}
			}
			return -1;
		}
	}
	
	bool PALAnimation::isValid() const {
		return detail::ln2(m_id) != -1;
	}

	std::string PALAnimation::getName() const {
		return PALUtil::instance()->m_pal_animations[m_id].name;
	}

	Animation* PALAnimation::generateAnimation() {
		Log("palanim") << "Generating Animation " << getName() << " with w,h= "
				<< m_width << ", " << m_height;
		struct PALUtil::pal_animation p = PALUtil::instance()->m_pal_animations[m_id];
		Animation *anim = new Animation(p.nr_elements);
		anim->setFrameDuration(p.speed);

		for(int i = 0; i != p.nr_elements; ++i) {
			Image* image = makeFrame(i);
			if (!image) {
				PANIC_PRINT("image == NULL");
			}
			anim->setFrame(i, image);
		}
		anim->setCurrentFrame(0);
		return anim;
	}

	// FIXME: I'm pretty sure these 'gamma' calculations aren't really how gamma
	// is supposed to work. --zahlman
	Image* PALAnimation::makeFrame(int nframe) {
		PAL palette("color.pal");
		struct PALUtil::pal_animation p = PALUtil::instance()->m_pal_animations[m_id];
		uint8_t* data = m_data;

		uint32_t size = m_width * m_height * 4;
		boost::scoped_array<uint8_t> imgdata(new uint8_t[size]);
		uint32_t* pixeldata = reinterpret_cast<uint32_t*>(imgdata.get());

		for (int32_t y = 0; y < m_height; ++y) {
			for (int32_t x = 0; x < m_width; ++x) {
				uint8_t index = *data;
				if (index == 0) {
					*pixeldata = 0x00000000;
				} else if (index >= p.pal_offset && index - p.pal_offset <= p.nr_elements) {
					index = index - p.pal_offset;
					index = (index + nframe) % p.nr_elements;
					double factor = 1.0;
					if (m_id & ( 1 | 8 )) { // Slime & Shoreline
						factor = 4.0 / m_gamma;
					}
					uint8_t red = uint8_t(p.color_table[3*index]/factor) & 0xff;
					uint8_t green = uint8_t(p.color_table[3*index + 1]/factor) & 0xff;
					uint8_t blue = uint8_t(p.color_table[3*index + 2]/factor) & 0xff;
				
					*pixeldata = (red << 24) | (green << 16) | (blue << 8) | 0xff;
				} else {
					uint8_t red = std::min(palette.getRed(index) * m_gamma, 0xff) & 0xff;
					uint8_t green = std::min(palette.getGreen(index) * m_gamma, 0xff) & 0xff;
					uint8_t blue = std::min(palette.getBlue(index) * m_gamma, 0xff) & 0xff;
				
					*pixeldata = (red << 24) | (green << 16) | (blue << 8) | 0xff;
				}
				++pixeldata;
				++data;
			}
		}

		return CRenderBackend()->createStaticImageFromRGBA(imgdata.get(), m_width, m_height);
	}

	PALUtil::PALUtil() {
		init();
	}

	PALUtil::~PALUtil() {}
			
// Hard coded PAL Animations ...
// FIXME: There must be a prettier, more structured way to initialize?

// Fire often is used combined (fast & slow Fire) ... since we currently CAN'T
// display this, as we'd need to decompose the images itself, we simply
// combine the fire parts.

	uint8_t PAL_Slime[] = {
		0, 108, 0,
		11, 115, 7,
		27, 123, 15,
		43, 131, 27
	};
	
	uint8_t PAL_Monitors[] = {
		107, 107, 111,
		99, 103, 127,
		87, 107, 143,
		0, 147, 163,
		107, 187, 255
	};
	
	uint8_t PAL_FireSlow[] = { // slow and fast fire
		255, 0, 0,
		215, 0, 0,
		147, 43, 11,
		255, 119, 0,
		255, 59, 0,
		71, 0, 0,
		123, 0, 0,
		179, 0, 0,
		123, 0, 0,
		71, 0, 0
	};
	
	uint8_t PAL_Shoreline[] = {
		83, 63, 43,
		75, 59, 43,
		67, 55, 39,
		63, 51, 39,
		55, 47, 35,
		51, 43, 35
	};
	
	uint8_t PAL_BlinkingRed[] = { 252, 0, 0 };
	
	int PAL_NrElements[] = { 4, 6, 10, 5, 1 };
	// NB These values are spaced according to the animation lengths
	uint8_t PAL_PalOffsets[] = { 229, 233, 238, 248, 254 };
	
	std::string PAL_Names[] = {
		"Slime", "Monitors", "FireSlow", "Shoreline", "BlinkingRed"
	};
	
	int PAL_Speeds[] = { 200, 200, 180, 100, 33 };

	uint8_t* PAL_ColorTables[] = {
		PAL_Slime, PAL_Monitors, PAL_FireSlow, PAL_Shoreline, PAL_BlinkingRed
	};

	void PALUtil::init() {
		Log("palutil") << "Init PAL Animation reference tables.";
		memset(m_table, 0, 256);

		struct pal_animation p;
		uint8_t id = 1;
		for(int i = 0; i != 5; ++i) {
			p.name = PAL_Names[i];
			p.speed = PAL_Speeds[i];
			p.nr_elements = PAL_NrElements[i];
			p.pal_offset = PAL_PalOffsets[i];
			p.color_table = PAL_ColorTables[i];
			m_pal_animations[id] = p;
			memset(m_table + p.pal_offset, id, p.nr_elements);
			id = id << 1;
		}
	}
} } } } // FIFE::map::loaders::fallout
