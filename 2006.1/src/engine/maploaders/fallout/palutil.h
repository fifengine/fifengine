/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef PALUTIL_H
#define PALUTIL_H
#include <SDL.h>
#include <stdint.h>
#include <map>
#include <string>
#include <vector>

#include "singleton.h"

namespace FIFE {
	class Image;
	class Animation;
	/** Helper class to auto-generate Animation from Image data with a Palette-Animation
	 *
	 * This class is used to create an Animation, when a FRM file has palette entries,
	 * that are normally used to create an Animation by rotationg the colors in the palette.
	 *
	 * It takes the pixel data and holds the needed information. Then later
	 * generateAnimation() is called to actually generate the Animation.
	 * The Helper class PALUtil holds the static tables needed for all this.
	 */
	class PALAnimation {
		public:
			/** Constructor
			 * @param id Id of the palette animation as returned by PALUtil::checkPixel()
			 * @param data The pixel data in PAL 8Bit format.
			 * @param width Width of the image.
			 * @param height Height of the image.
			 * @param gamma A gamma value in the range form 1 to 4
			 *
			 * @note This class does @b not take Ownership over the data array.
			 */
			PALAnimation(int id, uint8_t* data,int width,int height,int gamma);

			/** Destructor
			 */
			~PALAnimation();

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
			Image* makeFrame(int nframe );
	};

	class PALUtil : public StaticSingleton<PALUtil> {
		public:
			/** Check whether a pixel is a Palette-Animation
			 * @param px A pixel encoded as a 8Bit palette entry.
			 * @return The id of the Palette-Animation or 0.
			 */
			uint8_t checkPixel(uint8_t px);

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

	inline
	uint8_t PALUtil::checkPixel(uint8_t px) {
		return m_table[px];
	};

}; // FIFE

#endif
