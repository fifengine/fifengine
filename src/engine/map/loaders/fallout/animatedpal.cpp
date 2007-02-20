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

// 3rd party library includes
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "debugutils.h"
#include "exception.h"

#include "animatedpal.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {
	/// Helper function to build a SDL_Color
	SDL_Color createColor(Uint8 r, Uint8 g, Uint8 b) {
		SDL_Color c;
		c.r = r;
		c.g = g;
		c.b = b;
		return c;
	}
	
	AnimatedBlock::AnimatedBlock(const std::string& name, int startIndex, int endIndex, int frameDuration, bool lightAdjust, PAL* pal)
		: m_name(name), m_start_index(startIndex), m_end_index(endIndex), m_frame_duration(frameDuration), m_light_adjust(lightAdjust) {

		if (pal != 0) {			
			// extract colors from the palette
			for (uint8_t i = startIndex; i <= endIndex; i++) {
				addColor(createColor(pal->getRed(i), pal->getGreen(i), pal->getBlue(i)));
			}
		}
	}
	
	const std::string& AnimatedBlock::getName() const {
		return m_name;
	}
		
	int AnimatedBlock::getStartIndex() const {
		return m_start_index;
	}
	
	int AnimatedBlock::getEndIndex() const {
		return m_end_index;
	}
	
	int AnimatedBlock::getNumFrames() const {
		return m_colors.size();
	}
	
	int AnimatedBlock::getFrameDuration() const {
		return m_frame_duration;
	}
	
	bool AnimatedBlock::isLightAdjusted() const {
		return m_light_adjust;
	}
	
	void AnimatedBlock::addColor(SDL_Color color) {
		if (isLightAdjusted()) {
			// The animated colors that should be light adjusted (shoreline and slime)
			// are 4 times too bright in the color.pal file. A best solution is probably
			// to modify the color.pal, but that's annoying to do. It's easier to divide
			// by 4 here
			color.r /= 4;
			color.g /= 4;
			color.b /= 4;
		}
		
		m_colors.push_back(color);
	}
	
	uint8_t AnimatedBlock::getRed(int index, int frame) const {
		int i = (index - m_start_index + frame) % m_colors.size();
		return m_colors[i].r;
	}
	uint8_t AnimatedBlock::getBlue(int index, int frame) const {
		int i = (index - m_start_index + frame) % m_colors.size();
		return m_colors[i].b;
	}
	uint8_t AnimatedBlock::getGreen(int index, int frame) const {
		int i = (index - m_start_index + frame) % m_colors.size();
		return m_colors[i].g;
	}
	
	AnimatedPalette::AnimatedPalette()
		: m_pal("color.pal"), m_light_level(4), m_current_block(0), m_current_frame(0) {
	
		for (size_t i = 0; i < 256; i++) {
			m_blocks_by_index[i] = 0;
		}

		// create and add blocks of animated pixels
		AnimatedBlock* b = 0;		
		b = new AnimatedBlock("Slime", 229, 232, 200, true, &m_pal);
		/*b->addColor(createColor(0, 108, 0));
		b->addColor(createColor(11, 115, 7));
		b->addColor(createColor(27, 123, 15));
		b->addColor(createColor(43, 131, 27));*/
		addBlock(b);
		
		b = new AnimatedBlock("Monitors", 233, 237, 100, false, &m_pal);
		/*b->addColor(createColor(107, 107, 111));
		b->addColor(createColor(99, 103, 127));
		b->addColor(createColor(87, 107, 143));
		b->addColor(createColor(0, 147, 163));
		b->addColor(createColor(107, 187, 255));*/
		addBlock(b);
		
		// Create a single fire block of both slow and fast fire colors.
		// We don't currently handle images with pixels from seperate animated
		// blocks and slow and fast fire are often used together.
		b = new AnimatedBlock("Fire", 238, 248, 180, false, &m_pal);
		/*b->addColor(createColor(255, 0, 0));
		b->addColor(createColor(215, 0, 0));
		b->addColor(createColor(147, 43, 11));
		b->addColor(createColor(255, 119, 0));
		b->addColor(createColor(255, 59, 0));
		b->addColor(createColor(71, 0, 0));
		b->addColor(createColor(123, 0, 0));
		b->addColor(createColor(179, 0, 0));
		b->addColor(createColor(123, 0, 0));
		b->addColor(createColor(71, 0, 0));*/
		addBlock(b);
		
		b = new AnimatedBlock("Shoreline", 248, 253, 200, true, &m_pal);
		/*b->addColor(createColor(83, 63, 43));
		b->addColor(createColor(75, 59, 43));
		b->addColor(createColor(67, 55, 39));
		b->addColor(createColor(63, 51, 39));
		b->addColor(createColor(55, 47, 35));
		b->addColor(createColor(51, 43, 35));*/
		addBlock(b);
		
		b = new AnimatedBlock("BlinkingRed", 254, 254, 33, true, &m_pal);
		//b->addColor(createColor(252, 0, 0));
		b->addColor(createColor(0, 0, 0));
		addBlock(b);
	}

	AnimatedPalette::~AnimatedPalette() {
		for (size_t i = 0; i < m_blocks.size(); i++) {
			delete m_blocks[i];
		}
	}
	
	void AnimatedPalette::addBlock(AnimatedBlock* block) {
		m_blocks.push_back(block);
		
		// update our map from indices to blocks with the new block
		for (int i = block->getStartIndex(); i <= block->getEndIndex(); i++) {
			m_blocks_by_index[i] = block;	
		}
	}
	
	uint8_t AnimatedPalette::getLightLevel() const {
		return m_light_level;
	}
	
	void AnimatedPalette::setLightLevel(uint8_t lightLevel) {
		m_light_level = lightLevel;
	}
	
	const AnimatedBlock* AnimatedPalette::getBlock(uint8_t index) const {
		return m_blocks_by_index[index];	
	}

	const AnimatedBlock* AnimatedPalette::getCurrentAnimation() const {
		return m_current_block;
	}
	
	void AnimatedPalette::setCurrentAnimation(const AnimatedBlock* block) {
		m_current_block = block;
		m_current_frame = 0;
	}
	
	int AnimatedPalette::getCurrentFrame() const {
		return m_current_frame;
	}
	
	void AnimatedPalette::incrementFrame() {
		if (m_current_block != 0 && m_current_frame < m_current_block->getNumFrames()) {
			m_current_frame++;
		}
	}
	
	uint8_t AnimatedPalette::lightLevelAdjust(uint8_t colorComp) const {
		// multiply by the light level and cap at 255
		return std::min(colorComp * m_light_level, 0xff) & 0xff;
	}
	
	uint8_t AnimatedPalette::getRed(uint8_t index) const {
		const AnimatedBlock* block = getBlock(index);
		if (block == 0) {
			return lightLevelAdjust(m_pal.getRed(index));
		} else {
			// animated pixel color
			uint8_t result =  block->getRed(index, m_current_frame);
			if (block->isLightAdjusted()) {
				result = lightLevelAdjust(result);
			}
			return result;			
		}
	}
	
	uint8_t AnimatedPalette::getBlue(uint8_t index) const {
		const AnimatedBlock* block = getBlock(index);
		if (block == 0) {
			return lightLevelAdjust(m_pal.getBlue(index));
		} else {
			// animated pixel color
			uint8_t result =  block->getBlue(index, m_current_frame);
			if (block->isLightAdjusted()) {
				result = lightLevelAdjust(result);
			}
			return result;			
		}
	}
	
	uint8_t AnimatedPalette::getGreen(uint8_t index) const {
		const AnimatedBlock* block = getBlock(index);
		if (block == 0) {
			return lightLevelAdjust(m_pal.getGreen(index));
		} else {
			// animated pixel color
			uint8_t result =  block->getGreen(index, m_current_frame);
			if (block->isLightAdjusted()) {
				result = lightLevelAdjust(result);
			}
			return result;			
		}
	}

} } } } //FIFE::map::loaders::fallout
