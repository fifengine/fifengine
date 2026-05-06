// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_LOADERS_IMAGE_PROVIDER_H
#define FIFE_VIDEO_LOADERS_IMAGE_PROVIDER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "util/resource/resource.h"

namespace FIFE
{
    /** ImageLoader for some basic formats like jpeg, png etc.
     */
    class /*FIFE_API*/ ImageLoader : public IResourceLoader
    {
        public:
            ImageLoader() = default;
            virtual void load(IResource* res);
    };
} // namespace FIFE
#endif
