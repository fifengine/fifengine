// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <memory>
#include <string>

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

namespace FIFE
{
    void ControllerMappingLoader::load(const std::string& filename)
    {
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
} // namespace FIFE
