// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "controllermappingloader.h"

// Standard C++ library includes
#include <memory>
#include <string>
#include <vector>

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

        auto data            = vfs->open(filename);
        size_t const datalen = data->getDataLength();
        std::vector<uint8_t> darray(datalen);
        data->readInto(darray.data(), datalen);
        SDL_IOStream* iostream = SDL_IOFromConstMem(darray.data(), datalen);
        if (SDL_AddGamepadMappingsFromIO(iostream, false) == -1) {
            throw SDLException(std::string("Error when loading gamepad mappings: ") + SDL_GetError());
        }
        SDL_CloseIO(iostream);
    }
} // namespace FIFE
