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

#ifndef MFFALLOUTPAL_H
#define MFFALLOUTPAL_H

#include <string>
#include <stdint.h>

namespace FIFE {

	/** Helper class for Fallout file format
	 * Fallout saves images in 8bit palette encoded format.
	 * This class loads such a palette and offers some convenience
	 * functions.
	 */
	class MFFalloutPAL {
		public:
			/** Constructor
			 * @param file The palette file to use.
			 */
			MFFalloutPAL(const std::string& file);

			/** Destructor
			 */
			~MFFalloutPAL();

			/** Convenient palette entry access function.
			 * The parameter is allways the index into the palette
			 * and must be >= 0 and < 256 otherwise this will
			 * throw an IndexOverflow. If the return value is
			 * a 32bit integer it is the color encoded as RGBA
			 * with an alpha value of @c 0xff.
			 */ 
			uint8_t get(size_t index) const;

			/** Convenience functions
			 * @see get
			 */
			uint8_t getRed(uint8_t index) const;
			/** Convenience functions
			 * @see get
			 */
			uint8_t getGreen(uint8_t index) const;
			/** Convenience functions
			 * @see get
			 */
			uint8_t getBlue(uint8_t index) const;

			/** Convenience operator
			 * @see get
			 */
			uint32_t operator[](uint8_t) const;
			/** Convenience functions
			 * @see get
			 */
			uint32_t getRGB(uint8_t index) const;

		private:
			uint8_t* m_palette;

			MFFalloutPAL(const MFFalloutPAL&);
			MFFalloutPAL& operator=(const MFFalloutPAL&);
	};

}//FIFE

#endif
