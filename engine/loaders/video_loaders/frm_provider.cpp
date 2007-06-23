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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "loaders/fallout/frm.h"
#include "loaders/fallout/animatedpal.h"
#include "video/animation.h"
#include "video/image.h"
#include "util/log.h"

#include "frm_provider.h"

namespace FIFE { namespace video { namespace loaders {

	map::loaders::fallout::AnimatedPalette* FRMProvider::m_palette = 0;

	RenderAble* FRMProvider::createRenderable() {
		if (m_palette == 0) {
			m_palette = map::loaders::fallout::AnimatedPalette::load("content/gfx/fallout_palette.xml");
		}
		
		map::loaders::fallout::FRM frm(getLocation().getFilename(), m_palette);

		uint16_t dir = 0;
		uint16_t frame = 0; //getLocation().getFrame();

		dir = getLocation().getDirection();
		frame = getLocation().getFrame();
		
		if (frm.getNumFrames() > 1) { // Animation!
			Animation* result = new Animation(frm.getNumFrames());

			for (uint32_t j=0; j < frm.getNumFrames() ; j++) {
				Image* image = dynamic_cast<Image*>(frm.getFrame(dir, j));

				if( image == 0 ) {
					Log("frm_provider") 
						<< getLocation().toString() << "  ... not loaded";
					delete result;
					return 0;
				}
				// Image has X/Y shifts set to per frame offset.
				result->setFrame(j, image);
			}
	
			// set the global -per- renderable shift
			result->setXShift(frm.getShiftX(dir));
			result->setYShift(frm.getShiftY(dir));
			
			result->setActionFrame(frm.getActionFrameIdx());
	
			result->setCurrentFrame(0);
			uint16_t frm_fps = frm.getFramesPerSecond();
			result->setFrameDuration(1000 / frm_fps);

			return result;

		} else {
			RenderAble* image = frm.getFrame(dir, frame);
			if( dynamic_cast<Animation*>(image) ) {
				Log("frm_provider") 
					<< getLocation().toString() <<  " is an (pal)animation.";
			}

			// set the global -per- renderable shift

			image->setXShift(frm.getShiftX(dir));
			image->setYShift(frm.getShiftY(dir));

			return image;
		}
	}
} } }
