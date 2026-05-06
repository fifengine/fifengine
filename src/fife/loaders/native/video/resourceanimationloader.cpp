// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "resourceanimationloader.h"

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "video/animation.h"

namespace FIFE
{

    void ResourceAnimationLoader::load(IResource* res)
    {
        auto* anim = dynamic_cast<Animation*>(res);
        if (anim != nullptr) {
            std::vector<ImagePtr> const frames = anim->getFrames();
            for (auto const & frame : frames) {
                if (frame->getState() != IResource::RES_LOADED) {
                    frame->load();
                }
            }
        }
    }
} // namespace FIFE
