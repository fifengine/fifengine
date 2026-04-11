// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IANIMATION_LOADER_H
#define FIFE_IANIMATION_LOADER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "video/animation.h"

namespace FIFE
{

    class FIFE_API IAnimationLoader
    {
    public:
        virtual ~IAnimationLoader() = default;

        /** determines whether the resource is in
         *	the correct format for this loader
         */
        virtual bool isLoadable(const std::string& filename) = 0;

        /** responsible for loading the animation
         * returns a shared pointer to an animation resource
         */
        virtual AnimationPtr load(const std::string& filename) = 0;

        /** responsible for loading all animations
         * returns a vector of shared pointer to an animation resource
         */
        virtual std::vector<AnimationPtr> loadMultiple(const std::string& filename) = 0;
    };

    using AnimationLoaderPtr = SharedPtr<IAnimationLoader>;
} // namespace FIFE

#endif
