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

#ifndef FIFE_MAP_LOADERS_FALLOUT_COLOR_OVERRIDE_H
#define FIFE_MAP_LOADERS_FALLOUT_COLOR_OVERRIDE_H

// Standard C++ library includes
#include <string>
#include <map>
#include <bitset>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE { namespace map { namespace loaders { namespace fallout {


	struct RGBA {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
	
	/**
	 * Overrides certain colors and alpha values in the palette of 
	 * an FRM file. Allows the alpha and color of any index to be overrided
	 * independantly or together. 
	 *
	 * This is overkill for supporting Fallout because it seems like whenever
	 * a FRM has indices that are meant to be transparent then EVERY non-zero
	 * index in the FRM is meant to be transparent. From the FRMs I've seen, 
	 * we could simply support a single override alpha and possible color per
	 * FRM file instead of one for each index.
	 */
	class ColorOverride {
		public:
			ColorOverride();
			
			/**
			 * Returns true if no alpha components are overridden
			 */
			bool areAllOverridesOpaque() const;
			
			/**
			 * Modifies the passed in RGBA with the overriden values.
			 * @param index The pixel's color index
			 * @param override The color to change
			 * @return true if override was modified, false otherwise
			 */
			bool getOverride(uint8_t index, RGBA& override) const;
			
			void addOverride(uint8_t index, const RGBA& override);
			void addOverride(uint8_t index, uint8_t alpha);
			
			typedef std::map<std::string, ColorOverride> OverrideMap;
			/**
			 * Loads ColorOverride objects from a XML file into a map.
			 */
			static void load(const std::string& filename, OverrideMap& overrides);
			
			// ColorOverride that doesn't override anything.
			static const ColorOverride EMPTY_OVERRIDE;
		private:
			// flags for each index. determines if the alpha component is overriden
			std::bitset<256> m_alpha_set;
			// flags for each index. determines if the rgb components are overriden
			std::bitset<256> m_color_set;
			// the overriding colors
			RGBA m_colors[256];			
	};
	
} } } }//FIFE

#endif
