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

// Standard C++ library includes
#include <iostream>

// 3rd party library includes
#include <SDL.h>
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"
#include "video/renderbackend.h"
#include "video/image_location.h"
#include "util/base/fife_stdint.h"
#include "util/resource/resource.h"
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "subimage_loader.h"

namespace FIFE { 
	static Logger _log(LM_NATIVE_LOADERS);
	
	IResource* SubImageLoader::loadResource(const ResourceLocation& location) {
		const ImageLocation* loc = dynamic_cast<const ImageLocation*>(&location);
		if (!loc) {
			// Wrong location type given for subimage provider
			return NULL;
		}
		Image* r = loc->getParentSource();
		if (!r) {
			// No parent source assigned, cannot provide subimage
			return NULL;
		}
		SDL_Surface* src = r->getSurface();
		if (!src) {
			return NULL;
		}
		SDL_Rect src_rect;
		src_rect.x = loc->getXShift();
		src_rect.y = loc->getYShift();
		src_rect.w = loc->getWidth();
		src_rect.h = loc->getHeight();

		FL_DBG(_log, LMsg("subimage_loader")
			<< " rect:" << Rect(src_rect.x,src_rect.y,src_rect.w,src_rect.h));

		uint32_t Amask = src->format->Amask ?  AMASK : 0;
		SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE, src_rect.w,  src_rect.h, 32,
		                                           RMASK, GMASK, BMASK, Amask);
		SDL_FillRect(result, NULL, 0);
		SDL_SetAlpha(src,0,SDL_ALPHA_OPAQUE);
		SDL_BlitSurface(src,&src_rect,result,0);

		Image* image = RenderBackend::instance()->createImage(result);
		image->setResourceLocation(location);
		return image;
	};

}
