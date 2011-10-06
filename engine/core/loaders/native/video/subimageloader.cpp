/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
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

// Standard C++ library includes
#include <iostream>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"
#include "video/renderbackend.h"
#include "util/base/fife_stdint.h"
#include "util/resource/resource.h"
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "subimageloader.h"

namespace FIFE {
	static Logger _log(LM_NATIVE_LOADERS);

	void SubImageLoader::load(IResource* res) {
		Image* img = dynamic_cast<Image*>(res);

		if (!m_img) {
			// No parent source assigned, cannot provide subimage
			FL_ERR(_log, LMsg("subimage_loader") << " no parent source assigned.  Cannot provide subimage.");
			return;
		}

		SDL_Surface* src = m_img->getSurface();

		if (!src) {
			FL_ERR(_log, LMsg("subimage_loader") << " Cannot get the source image surface for image: " << m_img->getName());
			return;
		}

		SDL_Rect src_rect;
		src_rect.x = m_rect.x;
		src_rect.y = m_rect.y;
		src_rect.w = m_rect.w;
		src_rect.h = m_rect.h;

		FL_DBG(_log, LMsg("subimage_loader") << " rect:" << Rect(src_rect.x,src_rect.y,src_rect.w,src_rect.h));

		uint32_t Amask = src->format->Amask ?  AMASK : 0;

		SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE, src_rect.w,  src_rect.h, 32,
		                                           RMASK, GMASK, BMASK, Amask);
		SDL_FillRect(result, NULL, 0);
		SDL_SetAlpha(src,0,SDL_ALPHA_OPAQUE);
		SDL_BlitSurface(src,&src_rect,result,0);

		img->setSurface(result);
	}
}
