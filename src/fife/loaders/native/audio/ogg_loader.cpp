// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "ogg_loader.h"

// Standard C++ library includes
#include <string>

// Platform specific includes

// 3rd party library includes

// FIFE includes
#include "audio/soundclip.h"
#include "sounddecoder_ogg.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/vfs.h"

namespace FIFE
{
    static Logger _log(LM_NATIVE_LOADERS);

    void OggLoader::load(IResource* res)
    {
        VFS* vfs = VFS::instance();

        std::string const filename = res->getName();

        RawData* rdptr = vfs->open(filename);
        dynamic_cast<SoundClip*>(res)->adobtDecoder(new SoundDecoderOgg(rdptr));
    }
} // namespace FIFE
