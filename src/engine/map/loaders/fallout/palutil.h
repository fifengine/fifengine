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

#ifndef FIFE_MAP_LOADERS_FALLOUT_PALUTIL_H
#define FIFE_MAP_LOADERS_FALLOUT_PALUTIL_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// Platform specific includes
#include "util/fifeint.h"

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "singleton.h"

namespace FIFE { 
	class Animation;
	class Image;

namespace map { namespace loaders { namespace fallout {
	/** Helper class to auto-generate Animation from Image data with a 
	 * Palette-Animation
	 *
	 * This class is used to create an Animation, when a FRM file has palette 
	 * entries, that are normally used to create an Animation by rotationg the 
	 * colors in the palette.
	 *
	 * It takes the pixel data and holds the needed information. Then later
	 * generateAnimation() is called to actually generate the Animation.
	 * The Helper class PALUtil holds the static tables needed for all this.
	 */
	class PALAnimation {
		public:
			/** Constructor
			 * @param id Id of the palette animation as returned by 
			 * PALUtil::checkPixel()
			 * @param data The pixel data in PAL 8Bit format.
			 * @param width Width of the image.
			 * @param height Height of the image.
			 * @param gamma A gamma value in the range from 1 to 4
			 *
			 * @note This class does @b not assume ownership of the data array.
			 */
			PALAnimation(int id, uint8_t* data, int width, int height, int gamma) :
				m_id(id), m_data(data), 
				m_width(width), m_height(height), m_gamma(gamma) {}

			/** Generates a new Animation from the data
			 * @return A newly generated Animation
			 * @note This will allways generate a new Animation. 
			 */
			Animation* generateAnimation();

			/** Get the name of the PALAnimation 
			 * This mostly useful for debugging
			 */
			std::string getName() const;

			/** Check whether the PALAnimation is valid
			 * @param True, if an Animation can actually be generated from the Id
			 * as set in the constructor. This might be false happen when
			 * two different Palette-Animations are contained in the same image.
			 */
			bool isValid() const;

		private:
			int m_id;
			uint8_t* m_data;
			int m_width;
			int m_height;
			uint8_t m_gamma;

			/** Generate one frame
			 */
			Image* makeFrame(int nframe);
	};

	class PALUtil : public StaticSingleton<PALUtil> {
		public:
			/** Check whether a pixel is a Palette-Animation
			 * @param px A pixel encoded as a 8Bit palette entry.
			 * @return The id of the Palette-Animation or 0.
			 */
			uint8_t checkPixel(uint8_t px) { return m_table[px]; }

		private:
			/** Initialize the lookup tables.
			 */
			void init();

			uint8_t m_table[256];
			struct pal_animation {
				std::string name;
				int nr_elements;
				int pal_offset;
				uint8_t* color_table; // max 6 elements
				int speed; // in ms
			};
			typedef std::map<uint8_t,struct pal_animation> type_pal_animations;
			type_pal_animations m_pal_animations;

			SINGLEFRIEND(PALUtil);
			friend class PALAnimation;
	};

} } } } // FIFE::map::loaders::fallout

#endif
