// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IANIMATION_LOADER_H
#define FIFE_IANIMATION_LOADER_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/animation.h"

namespace FIFE
{

    class IAnimationLoader
    {
    public:
        virtual ~IAnimationLoader() { };

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

    typedef SharedPtr<IAnimationLoader> AnimationLoaderPtr;
} // namespace FIFE

#endif
