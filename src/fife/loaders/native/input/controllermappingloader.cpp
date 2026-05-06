// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "controllermappingloader.h"

// Standard C++ library includes
#include <memory>
#include <string>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "util/base/exception.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"

namespace FIFE
{
    void ControllerMappingLoader::load(std::string const & filename)
    {
        VFS* vfs = VFS::instance();

        std::unique_ptr<RawData> data(vfs->open(filename));
        size_t const datalen = data->getDataLength();
        std::unique_ptr<uint8_t[]> const darray(new uint8_t[datalen]);
        data->readInto(darray.get(), datalen);
        SDL_IOStream* iostream = SDL_IOFromConstMem(darray.get(), datalen);
        if (SDL_AddGamepadMappingsFromIO(iostream, false) == -1) {
            throw SDLException(std::string("Error when loading gamepad mappings: ") + SDL_GetError());
        }
        SDL_CloseIO(iostream);
    }
} // namespace FIFE
