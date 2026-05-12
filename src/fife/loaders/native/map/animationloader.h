// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ANIMATION_LOADER_H
#define FIFE_ANIMATION_LOADER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "ianimationloader.h"
#include "video/animation.h"

namespace tinyxml2
{
    class XMLElement;
}

namespace FIFE
{

    class VFS;
    class ImageManager;
    class AnimationManager;

    class FIFE_API AnimationLoader : public IAnimationLoader
    {
        public:
            AnimationLoader(VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager);

            /**
             * @see IAnimationLoader::isLoadable
             */
            bool isLoadable(std::string const & filename) override;

            AnimationPtr load(std::string const & filename) override;

            std::vector<AnimationPtr> loadMultiple(std::string const & filename) override;

        private:
            AnimationPtr loadAnimation(std::string const & filename, tinyxml2::XMLElement* animationElem);

            VFS* m_vfs;
            ImageManager* m_imageManager;
            AnimationManager* m_animationManager;
    };
} // namespace FIFE

#endif
