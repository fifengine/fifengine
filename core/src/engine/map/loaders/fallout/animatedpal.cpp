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
#include <memory>

// 3rd party library includes
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "xml/tinyxml/tinyxml.h"
#include "xml/xmlutil.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "util/debugutils.h"
#include "util/exception.h"

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
	
	AnimatedBlock::AnimatedBlock(const std::string& name, int startIndex, int endIndex, int frameDuration, bool lightAdjust)
		: m_name(name), m_start_index(startIndex), m_end_index(endIndex), m_frame_duration(frameDuration), m_light_adjust(lightAdjust) {

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
		// Division by 4 in case of slime etc is no good (loose too many bits)
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
	
	AnimatedPalette::AnimatedPalette(const std::string& base_palette)
		: m_pal(base_palette), m_light_level(4), m_current_block(0), m_current_frame(0) {
	
		for (size_t i = 0; i < 256; i++) {
			m_blocks_by_index[i] = 0;
		}

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
	
	void AnimatedPalette::setCurrentFrame(int frame) {
		m_current_frame = frame % m_current_block->getNumFrames();
	}
	
	uint8_t AnimatedPalette::lightLevelAdjust(uint8_t colorComp) const {
		// multiply by the light level and cap at 255
		colorComp = colorComp * m_light_level / 4;
		if (colorComp > 255) {
			return 255;
		} else {
			return colorComp;
		}
	}
	
	uint8_t AnimatedPalette::getRed(uint8_t index) const {
		const AnimatedBlock* block = getBlock(index);
		if (block == 0) {
			// Normal LL adjustment
			return std::min(int(m_pal.getRed(index) * m_light_level), 255);
		} else {
			// animated pixel color
			uint8_t result =  block->getRed(index, m_current_frame);
			if (block->isLightAdjusted()) {
				// LL adjustment, but preserve bits
				result = lightLevelAdjust(result);
			} // Else not LL Adjustment (Fire, Monitors and BlinkingRed )
			return result;			
		}
	}
	
	uint8_t AnimatedPalette::getBlue(uint8_t index) const {
		const AnimatedBlock* block = getBlock(index);
		if (block == 0) {
			return std::min(int(m_pal.getBlue(index) * m_light_level), 255);
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
			return std::min(int(m_pal.getGreen(index) * m_light_level), 255);
		} else {
			// animated pixel color
			uint8_t result =  block->getGreen(index, m_current_frame);
			if (block->isLightAdjusted()) {
				result = lightLevelAdjust(result);
			}
			return result;			
		}
	}

	AnimatedPalette* AnimatedPalette::load(const std::string& filename) {
		TiXmlDocument doc(filename);
		doc.LoadFile(); // Throws CannotOpenFile

		TiXmlNode* node = doc.RootElement();
		TiXmlElement* el = node->ToElement();
		std::string base_palette = xmlutil::queryElement<std::string>(el, "palette");
		std::auto_ptr<AnimatedPalette> anim_palette(new AnimatedPalette(base_palette));

		TiXmlElement* anim_block = el->FirstChildElement("animated-block");
		while( anim_block ) {
			std::string name  = xmlutil::queryElement<std::string>(anim_block, "name");
			int startIndex    = xmlutil::queryElement<int>(anim_block, "start-index");
			int endIndex      = xmlutil::queryElement<int>(anim_block, "end-index");
			int frameDuration = xmlutil::queryElement<int>(anim_block, "frame-duration");
			bool lightAdjust  = xmlutil::queryElement<bool>(anim_block, "light-adjust");
			Log("palanim")
				<< name << " from: " << startIndex << " to: " << endIndex
				<< " delay: " << frameDuration << (lightAdjust ? " light-adjusted " : "");

			std::auto_ptr<AnimatedBlock> b(new AnimatedBlock(name,startIndex, endIndex,
			                               frameDuration, lightAdjust));
			TiXmlElement* color_element = anim_block->FirstChildElement("colors");
			if( color_element ) {
				color_element = color_element->FirstChildElement("color");
				while( color_element ) {
					SDL_Color c = xmlutil::queryElement<SDL_Color>(color_element);
					b->addColor(c);
					color_element = color_element->NextSiblingElement("color");
				}
			}
			anim_palette->addBlock( b.release() );
			anim_block = anim_block->NextSiblingElement("animated-block");
		}
		return anim_palette.release();
	}

} } } } //FIFE::map::loaders::fallout
