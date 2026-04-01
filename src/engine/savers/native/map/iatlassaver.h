// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IATLAS_SAVER_H
#define FIFE_IATLAS_SAVER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/sharedptr.h"
#include <string>

namespace FIFE
{
    class IAtlasSaver
    {
    public:
        virtual ~IAtlasSaver() { }

        /** responsible for saving the atlas
         */
        virtual void save(const std::string& filename) = 0;
    };

    using AtlasSaverPtr = SharedPtr<FIFE::IAtlasSaver>;} // namespace FIFE

#endif
