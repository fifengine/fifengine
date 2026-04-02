// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_OGGLOADER_H_
#define FIFE_OGGLOADER_H_

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/resource/resource.h"

namespace FIFE
{

    class OggLoader : public IResourceLoader
    {
    public:
        OggLoader() = default;
        virtual void load(IResource* res);
    };
} // namespace FIFE

#endif