// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_LOADERS_ANIMATION_PROVIDER_H
#define FIFE_VIDEO_LOADERS_ANIMATION_PROVIDER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "util/resource/resource.h"

namespace FIFE
{
    /** ResourceAnimationLoader.
     */
    class FIFE_API ResourceAnimationLoader : public IResourceLoader
    {
    public:
        ResourceAnimationLoader() = default;
        virtual void load(IResource* res);
    };
} // namespace FIFE
#endif