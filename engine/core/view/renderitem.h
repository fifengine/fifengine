/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_VIEW_RENDERITEM_H
#define FIFE_VIEW_RENDERITEM_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "visual.h"

namespace FIFE {

	class Instance;

	class RenderItem {
		public:
			RenderItem(Instance* parent);
			~RenderItem();
			Instance* instance;

			/** Returns closest matching static image for given angle
			* @return id for static image
			* @see ObjectVisual::getStaticImageIndexByAngle
			*/
			int32_t getStaticImageIndexByAngle(uint32_t angle, Instance* instance);

			/** Resets the important values.
			*/
			void reset();

			// point where instance was drawn during the previous render
			DoublePoint3D screenpoint;

			// dimensions of this visual on the virtual screen
			Rect bbox;

			// dimensions of this visual during the previous render
			Rect dimensions;

			// image used during previous render
			ImagePtr image;

			// current facing angle
			int32_t facingAngle;

			// current transparency
			uint8_t transparency;

			// current frame index (e.g. needed for action frame)
			int32_t currentFrame;

			// pointer to vector that holds animation overlay images
			std::vector<ImagePtr>* animationOverlayImages;
			// pointer to vector that holds color overlay images
			std::vector<ImagePtr>* colorOverlayImages;
			OverlayColors* colorOverlay;
		private:
			int32_t m_cachedStaticImgId;
			int32_t m_cachedStaticImgAngle;
	};

	typedef std::vector<RenderItem*> RenderList;
}

#endif
