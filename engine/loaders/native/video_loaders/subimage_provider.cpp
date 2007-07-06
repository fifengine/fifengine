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
#include <SDL.h>
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"
#include "video/pixelbuffer.h"
#include "video/renderbackend.h"
#include "util/debugutils.h"
#include "util/exception.h"
#include "video/imagecache.h"

#include "subimage_provider.h"

namespace FIFE { namespace video { namespace loaders {
	
	RenderAble* SubImageProvider::createRenderable() {
		RenderableLocation loc(getLocation());
		SDL_Rect src_rect;

		size_t imageid = boost::lexical_cast<size_t>(loc.getExtension(RenderableLocation::IMAGE_ID));

		src_rect.x = boost::lexical_cast<int>(loc.getExtension(RenderableLocation::X));
		src_rect.y = boost::lexical_cast<int>(loc.getExtension(RenderableLocation::Y));
		src_rect.w = boost::lexical_cast<int>(loc.getExtension(RenderableLocation::W));
		src_rect.h = boost::lexical_cast<int>(loc.getExtension(RenderableLocation::H));

		PixelBufferPtr pixelbuffer = ImageCache::instance()->getPixelBuffer(imageid);
		if( !pixelbuffer ) {
			return 0;
		}

		Debug("subimage_loader")
			<< "iid:" << imageid 
			<< " rect:" << Rect(src_rect.x,src_rect.y,src_rect.w,src_rect.h);

		SDL_Surface* src = pixelbuffer->getSurface();
		uint32_t Amask = src->format->Amask ?  0x000000ff : 0;
		SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE, src_rect.w,  src_rect.h, 32,
		                                           0xff000000, 0x00ff0000, 0x0000ff00, Amask);
		SDL_FillRect(result, NULL, 0);
		SDL_SetAlpha(src,0,SDL_ALPHA_OPAQUE);
		SDL_BlitSurface(src,&src_rect,result,0);

		return RenderBackend::instance()->createStaticImageFromSDL(result);
	};

} } }
