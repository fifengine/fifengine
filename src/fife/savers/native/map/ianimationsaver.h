// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IANIMATION_SAVER_H
#define FIFE_IANIMATION_SAVER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "util/base/sharedptr.h"

namespace FIFE
{

    class FIFE_API IAnimationSaver
    {
        public:
            virtual ~IAnimationSaver() = default;

            /** responsible for saving the animation
             */
            virtual void save(std::string const & filename) = 0;
    };

    using AnimationSaverPtr = SharedPtr<IAnimationSaver>;
} // namespace FIFE

#endif
