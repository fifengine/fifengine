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
#include <boost/scoped_array.hpp>
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/animation.h"
#include "video/image.h"
#include "debugutils.h"
#include "exception.h"
#include "tinyxml/tinyxml.h"
#include "util/purge.h"

#include "animation_provider.h"
#include "image_provider.h"

namespace FIFE { namespace map { namespace loaders { namespace util {
	
	RenderAble* AnimationProvider::createRenderable() {

		const std::string& filename = getLocation().getFilename();
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


		int frameDelay = 0;
		int actionFrame = 0;
		element->QueryIntAttribute("delay", &frameDelay);
		element->QueryIntAttribute("action", &actionFrame);

		std::vector<Image*> frames;
		
		TiXmlElement* frame_element = element->FirstChildElement("frame");
		if (!frame_element) {
			throw InvalidFormat("animation without <frame>s");
		}
		
		while (frame_element) {
			const char* source = frame_element->Attribute("source");
 			if( source == 0 ) {
				purge( frames );
				throw InvalidFormat("animation without <frame>s");
 			}

			int x_off = 0;
			int y_off = 0;
			frame_element->QueryIntAttribute("x_offset", &x_off);
			frame_element->QueryIntAttribute("y_offset", &y_off);

			RenderableLocation location(RenderAble::RT_IMAGE, source);
			ImageProvider image_loader(location);
			
			// static cast is safe, it IS a image loader.
			Image* image = static_cast<Image*>(image_loader.createRenderable());
			if( !image ) {
				purge( frames );
				throw InvalidFormat(std::string("couldn't load image '") 
				                    + source + "'");
			}

			image->setXShift(x_off);
			image->setYShift(y_off);
			
			frames.push_back(image);
			frame_element= frame_element->NextSiblingElement("frame");
		}

		Animation* animation = new Animation(frames.size());
		animation->setFrameDuration(frameDelay);
		animation->setActionFrame(actionFrame);
		for (size_t i = 0; i < frames.size(); ++i) {
			animation->setFrame(i, frames[i]);
		}
		animation->setCurrentFrame(0);
		
		return animation;
	}

} } } }
