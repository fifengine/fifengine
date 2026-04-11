// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IATLAS_SAVER_H
#define FIFE_IATLAS_SAVER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "util/base/sharedptr.h"
#include <string>

namespace FIFE
{
    class FIFE_API IAtlasSaver
    {
    public:
        virtual ~IAtlasSaver() = default;

        /** responsible for saving the atlas
         */
        virtual void save(const std::string& filename) = 0;
    };

    using AtlasSaverPtr = SharedPtr<FIFE::IAtlasSaver>;
} // namespace FIFE

#endif
