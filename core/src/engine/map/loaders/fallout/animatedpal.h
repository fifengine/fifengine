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

#ifndef FIFE_MAP_LOADERS_FALLOUT_ANIMATED_PAL_H
#define FIFE_MAP_LOADERS_FALLOUT_ANIMATED_PAL_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
class SDL_Color;

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "pal.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {
	
	/** An AnimatedBlock represents one part of the palette that is animated
	  by rotating the colors mapped to each index in the block.
	*/
	class AnimatedBlock {
		public:
			/** Creates an AnimatedBlock that loads its colors from the given PAL object. All colors in the 
			  range [startIndex, endIndex] are read from the palette and added to the new block. If pal is NULL
			  then no colors are added and the addColor method must be used to fill the animated block.
			*/
			AnimatedBlock(const std::string& name, int startIndex, int endIndex, int frameDuration, bool lightAdjust, PAL* pal);
			
			/** Adds a color that will be mapped to one of the indices of this block.
			 */			 
			void addColor(SDL_Color color);
		
		 	/// Returns the AnimatedBlock's name
			const std::string& getName() const;
			
			/** Gets block's startIndex. The methods getRed/Blue/Green will not work for an index 
			  that is less than the startIndex
			*/
			int getStartIndex() const;
			
			/** Gets block's endIndex. The methods getRed/Blue/Green will not work for an index 
			  that is greater than the endIndex
			*/
			int getEndIndex() const;
			
			/** Gets the number of frames in the animation produced by this palette. The methods 
			  getRed/Blue/Green will only work if 0 <= frame < getNumFrames()
			*/
			int getNumFrames() const;
			
			/// Gets length of time in milliseconds that each frame should be displayed. 
			int getFrameDuration() const;
			
			/** Tells whether colors held in this AnimatedBlock should be light adjusted before being displayed.
			  In fallout, colors that are meant to be glowing, like fire and monitors, are not light adjusted. They
			  are always the same bright color.
			
			  Note: Light adjusted may be the wrong term here. I think gamma refers to a different setting in fallout.
			  To be honest, this part, and the way fife adjusts the colors, confuses me. If you know better, fix this up
			*/
			bool isLightAdjusted() const;
		
			/** Gets the red/green/blue component of the color mapped to 
			  a given index in a given frame.
			*/	
			uint8_t getRed(int index, int frame) const;
			uint8_t getGreen(int index, int frame) const;
			uint8_t getBlue(int index, int frame) const;
		private:
			std::string m_name;
			int m_start_index;
			int m_end_index;
			int m_frame_duration;
			bool m_light_adjust;
			std::vector<SDL_Color> m_colors;
	};
	
	/** An AnimatedPalette is a palette of 256 colors with at most one AnimatedBlock being rotated. 
	  The current AnimatedBlock and the current frame in the rotation can be modified.
	*/
	class AnimatedPalette {
		public:
			AnimatedPalette();
			~AnimatedPalette();
			
			/// @return the current light level used by the palette.
			uint8_t getLightLevel() const;
			/// Sets the current light level used by the palette.
			void setLightLevel(uint8_t lightLevel);
			
			/** Checks if a given index belongs to an animated color.
			  @return the AnimatedBlock the of the index if there is one,
			          or NULL otherwise
			*/
			const AnimatedBlock* getBlock(uint8_t index) const;

			/// @return the current AnimatedBlock used by the palette. NULL if none.
			const AnimatedBlock* getCurrentAnimation() const;
			
			/** Sets the current AnimatedBlock used by the palette. Calling this
			  method resets the frame to 0.			  
			  @param block The new block. May be NULL to mean no block
			*/			  
			void setCurrentAnimation(const AnimatedBlock* block);
			/// @return the current frame in the animated block.
			int getCurrentFrame() const;
			/** Increments the frame number if there are remaining frames in the 
			  current AnimatedBlock
			*/
			void incrementFrame();
			
			/** Gets the Red/Green/Blue component of the color of the given index.
			  The returned color is the animated and light adjusted color, suitable
			  to display.
			*/
			uint8_t getRed(uint8_t index) const;
			uint8_t getGreen(uint8_t index) const;
			uint8_t getBlue(uint8_t index) const;
			
		private:
			PAL m_pal;
			std::vector<AnimatedBlock*> m_blocks;

			uint8_t m_light_level;			
			const AnimatedBlock* m_current_block;
			int m_current_frame;
			
			AnimatedBlock* m_blocks_by_index[256];
			
			void addBlock(AnimatedBlock* block);
			uint8_t lightLevelAdjust(uint8_t colorComp) const;
	};
	

} } } }//FIFE

#endif
