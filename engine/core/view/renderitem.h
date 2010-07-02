/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
			RenderItem();

			Instance* instance;

			/** Returns closest matching static image for given angle
			* @return id for static image
			* @see ObjectVisual::getStaticImageIndexByAngle
			*/
			int getStaticImageIndexByAngle(unsigned int angle, Instance* instance);
		
			// point where instance was drawn during the previous render
			DoublePoint3D screenpoint;
			
			// dimensions of this visual on the virtual screen
			Rect bbox;

			// dimensions of this visual during the previous render
			Rect dimensions;
			
			// image used during previous render
			Image* image;
			
			// current facing angle
			int facing_angle;

			// current transparency
			unsigned char transparency;
		private:
			int m_cached_static_img_id;
			int m_cached_static_img_angle;
	};

	typedef std::vector<RenderItem*> RenderList;
}

#endif
