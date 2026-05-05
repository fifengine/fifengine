// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_OGGLOADER_H_
#define FIFE_OGGLOADER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "util/resource/resource.h"

namespace FIFE
{

    class /*FIFE_API*/ OggLoader : public IResourceLoader
    {
        public:
            OggLoader() = default;
            virtual void load(IResource* res);
    };
} // namespace FIFE

#endif
