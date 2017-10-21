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
#include <memory>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"

#include "controllermappingloader.h"

namespace FIFE {
	void ControllerMappingLoader::load(const std::string& filename) {
		VFS* vfs = VFS::instance();

		std::unique_ptr<RawData> data(vfs->open(filename));
		size_t datalen = data->getDataLength();
		std::unique_ptr<uint8_t[]> darray(new uint8_t[datalen]);
		data->readInto(darray.get(), datalen);
		SDL_RWops* rwops = SDL_RWFromConstMem(darray.get(), static_cast<int>(datalen));
		if (SDL_GameControllerAddMappingsFromRW(rwops, 0) == -1) {
			throw SDLException(std::string("Error when loading gamecontroller mappings: ") + SDL_GetError());
		}
		SDL_FreeRW(rwops);
	}
}  //FIFE
