// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ANIMATION_LOADER_H
#define FIFE_ANIMATION_LOADER_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
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

    class AnimationLoader : public IAnimationLoader
    {
    public:
        AnimationLoader(VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager);

        /**
         * @see IAnimationLoader::isLoadable
         */
        virtual bool isLoadable(const std::string& filename) override;

        /**
         * @see IAnimationLoader::load
         */
        virtual AnimationPtr load(const std::string& filename) override;

        /**
         * @see IAnimationLoader::loadMultiple
         */
        virtual std::vector<AnimationPtr> loadMultiple(const std::string& filename) override;

    private:
        AnimationPtr loadAnimation(const std::string& filename, tinyxml2::XMLElement* animationElem);

        VFS* m_vfs;
        ImageManager* m_imageManager;
        AnimationManager* m_animationManager;
    };
} // namespace FIFE

#endif
