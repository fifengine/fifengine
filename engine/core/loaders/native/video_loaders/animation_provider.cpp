/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/animation.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "util/logger.h"
#include "util/exception.h"
#include "util/purge.h"
#include "xml/tinyxml/tinyxml.h"

#include "animation_provider.h"
#include "image_provider.h"

namespace FIFE {
	static Logger _log(LM_NATIVE_LOADERS);

	Animation* AnimationProvider::loadAnimationFromLocation(const ResourceLocation& location) {
		return dynamic_cast<Animation*>(loadPooledResourceFromLocation(location));
	}
	
	const int INITIAL_OFFSET = -9999999;
	IPooledResource* AnimationProvider::loadPooledResourceFromLocation(const ResourceLocation& location) {
		assert(m_pool);
		
		const std::string& filename = location.getFilename();
		TiXmlDocument doc(filename);

		if (!doc.LoadFile()) {
			throw InvalidFormat(std::string("Error loading animation file '") 
				+ filename + "' "
				+ doc.ErrorDesc());
		}

		TiXmlNode* node = doc.RootElement();
		TiXmlElement* element;
		if (node) {
			element = node->ToElement();
		} else {
			throw InvalidFormat("Error: could not switch to xml:element?");
		}

		int common_frame_delay = 0;
		int actionFrame = 0;
		int x_offset = 0;
		int y_offset = 0;
		element->QueryIntAttribute("delay", &common_frame_delay);
		element->QueryIntAttribute("action", &actionFrame);
		element->QueryIntAttribute("x_offset", &x_offset);
		element->QueryIntAttribute("y_offset", &y_offset);

		Animation* animation = new Animation();
		
		TiXmlElement* frame_element = element->FirstChildElement("frame");
		if (!frame_element) {
			throw InvalidFormat("animation without <frame>s");
		}
		
		while (frame_element) {
			const char* source = frame_element->Attribute("source");
 			if( source == 0 ) {
				delete animation;
				throw InvalidFormat("animation without <frame>s");
 			}

			int frame_x_offset = INITIAL_OFFSET;
			int frame_y_offset = INITIAL_OFFSET;
			int frame_delay = -1;
			frame_element->QueryIntAttribute("x_offset", &frame_x_offset);
			frame_element->QueryIntAttribute("y_offset", &frame_y_offset);
			frame_element->QueryIntAttribute("delay", &frame_delay);

			Image* image = NULL;
			try {
				image = &m_pool->getImage(m_pool->addResourceFromFile(source));
			} catch (NotFound) {
				delete animation;
				throw InvalidFormat(std::string("couldn't load image '") 
				                    + source + "'");
			}

			if (frame_x_offset != INITIAL_OFFSET) {
				image->setXShift(frame_x_offset);
			} else {
				image->setXShift(x_offset);
			}
			if (frame_y_offset != INITIAL_OFFSET) {
				image->setYShift(frame_y_offset);
			} else {
				image->setYShift(y_offset);
			}
			if (frame_delay >= 0) {
				animation->addFrame(image, frame_delay);
			} else {
				animation->addFrame(image, common_frame_delay);
			}
			frame_element= frame_element->NextSiblingElement("frame");
		}

		animation->setActionFrame(actionFrame);
		FL_DBG(_log, LMsg("animation_loader")
			<< "file: '" << filename
			<< "' frames:" << animation->getNumFrames());

		return animation;
	}

}
