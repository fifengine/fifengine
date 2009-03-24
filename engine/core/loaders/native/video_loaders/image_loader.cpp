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

// 3rd party library includes
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/resource/resource_location.h"
#include "util/resource/resource.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "video/renderbackend.h"
#include "video/image_location.h"

#include "image_loader.h"

namespace FIFE { 
	IResource* ImageLoader::loadResource(const ResourceLocation& location) {
		const ImageLocation* loc = dynamic_cast<const ImageLocation*>(&location);

		const std::string& filename = location.getFilename();
		boost::scoped_ptr<RawData> data (m_vfs->open(filename));
		size_t datalen = data->getDataLength();
		boost::scoped_array<uint8_t> darray(new uint8_t[datalen]);
		data->readInto(darray.get(), datalen);
		SDL_RWops* rwops = SDL_RWFromConstMem(darray.get(), datalen);
		SDL_Surface* surface = IMG_Load_RW(rwops, false);
		SDL_FreeRW(rwops);
		if( !surface ) {
			return 0;
		}

		Image* res = RenderBackend::instance()->createImage(surface);
		res->setResourceLocation(location);
		if (loc) {
			res->setXShift(loc->getXShift());
			res->setYShift(loc->getYShift());
		}
		res->setAlphaOptimizerEnabled(true);
		return res;
	};
}
