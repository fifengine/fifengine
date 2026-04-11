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
#include "video/animation.h"

#include "ianimationloader.h"

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
        bool isLoadable(const std::string& filename) override;

        AnimationPtr load(const std::string& filename) override;

        std::vector<AnimationPtr> loadMultiple(const std::string& filename) override;

    private:
        AnimationPtr loadAnimation(const std::string& filename, tinyxml2::XMLElement* animationElem);

        VFS* m_vfs;
        ImageManager* m_imageManager;
        AnimationManager* m_animationManager;
    };
} // namespace FIFE

#endif
