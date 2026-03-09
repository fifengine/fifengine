// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "audio/soundclip.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/vfs.h"

#include "ogg_loader.h"
#include "sounddecoder_ogg.h"

namespace FIFE
{
    static Logger _log(LM_NATIVE_LOADERS);

    void OggLoader::load(IResource* res)
    {
        VFS* vfs = VFS::instance();

        std::string filename = res->getName();

        RawData* rdptr = vfs->open(filename);
        dynamic_cast<SoundClip*>(res)->adobtDecoder(new SoundDecoderOgg(rdptr));
    }
} // namespace FIFE
