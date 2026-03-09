// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IANIMATION_SAVER_H
#define FIFE_IANIMATION_SAVER_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/sharedptr.h"

namespace FIFE
{

    class IAnimationSaver
    {
    public:
        virtual ~IAnimationSaver() { };

        /** responsible for saving the animation
         */
        virtual void save(const std::string& filename) = 0;
    };

    typedef SharedPtr<IAnimationSaver> AnimationSaverPtr;
} // namespace FIFE

#endif
