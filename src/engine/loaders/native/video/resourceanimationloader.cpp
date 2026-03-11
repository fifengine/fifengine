// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "video/animation.h"

#include "resourceanimationloader.h"

namespace FIFE
{

    void ResourceAnimationLoader::load(IResource* res)
    {
        auto* anim = dynamic_cast<Animation*>(res);
        if (anim != nullptr) {
            std::vector<ImagePtr> frames = anim->getFrames();
            for (auto& frame : frames) {
                if (frame->getState() != IResource::RES_LOADED) {
                    frame->load();
                }
            }
        }
    }
} // namespace FIFE
