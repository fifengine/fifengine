/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

// 3rd party library includes
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "controller/engine.h"
#include "util/base/exception.h"
#include "util/resource/resource.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "video/renderbackend.h"
#include "video/image.h"

#include "imageloader.h"

namespace FIFE {
	void ImageLoader::load(IResource* res) {
		VFS* vfs = VFS::instance();

		Image* img = dynamic_cast<Image*>(res);

		//Have to save the images x and y shift or it gets lost when it's
		//loaded again.
		int32_t xShiftSave = img->getXShift();
		int32_t yShiftSave = img->getYShift();

		if(!img->isSharedImage()) {
			const std::string& filename = img->getName();
			boost::scoped_ptr<RawData> data (vfs->open(filename));
			size_t datalen = data->getDataLength();
			boost::scoped_array<uint8_t> darray(new uint8_t[datalen]);
			data->readInto(darray.get(), datalen);
			SDL_RWops* rwops = SDL_RWFromConstMem(darray.get(), static_cast<int>(datalen));

			SDL_Surface* surface = IMG_Load_RW(rwops, false);

			if (!surface) {
				throw SDLException(std::string("Fatal Error when loading image into a SDL_Surface: ") + SDL_GetError());
			}

			RenderBackend* rb = RenderBackend::instance();
			// in case of SDL we don't need to convert the surface
			if (rb->getName() == "SDL") {
				img->setSurface(surface);
			// in case of OpenGL we need a 32bit surface
			} else {
				SDL_PixelFormat dst_format = rb->getPixelFormat();
				SDL_PixelFormat src_format = *surface->format;
				uint8_t dstbits = dst_format.BitsPerPixel;
				uint8_t srcbits = src_format.BitsPerPixel;

				if (srcbits != 32 || dst_format.Rmask != src_format.Rmask || dst_format.Gmask != src_format.Gmask ||
					dst_format.Bmask != src_format.Bmask || dst_format.Amask != src_format.Amask) {
					dst_format.BitsPerPixel = 32;
					SDL_Surface* conv = SDL_ConvertSurface(surface, &dst_format, 0);
					dst_format.BitsPerPixel = dstbits;

					if (!conv) {
						throw SDLException(std::string("Fatal Error when converting surface to the screen format: ") + SDL_GetError());
					}

					img->setSurface(conv);
					SDL_FreeSurface(surface);
				} else {
					img->setSurface(surface);
				}
			}

			SDL_FreeRW(rwops);
		}
		//restore saved x and y shifts
		img->setXShift(xShiftSave);
		img->setYShift(yShiftSave);
	}
}  //FIFE
