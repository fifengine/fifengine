// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_OBJECT_LOADER_H_
#define FIFE_OBJECT_LOADER_H_

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/sharedptr.h"

#include "ianimationloader.h"
#include "iatlasloader.h"
#include "iobjectloader.h"

namespace FIFE
{

    class Model;
    class VFS;
    class ImageManager;
    class AnimationManager;

    class ObjectLoader : public IObjectLoader
    {
    public:
        ObjectLoader(
            Model* model,
            VFS* vfs,
            ImageManager* imageManager,
            AnimationManager* animationManager,
            const AnimationLoaderPtr& animationLoader = AnimationLoaderPtr(),
            const AtlasLoaderPtr& atlasLoader         = AtlasLoaderPtr());

        ~ObjectLoader();

        /**
         * @see IObjectLoader::setAnimationLoader
         */
        virtual void setAnimationLoader(const AnimationLoaderPtr& animationLoader);

        /**
         * @see IObjectLoader::getAnimationLoader
         */
        virtual AnimationLoaderPtr getAnimationLoader();

        /**
         * @see IObjectLoader::setAtlasLoader
         */
        virtual void setAtlasLoader(const AtlasLoaderPtr& atlasLoader);

        /**
         * @see IObjectLoader::getAtlasLoader
         */
        virtual AtlasLoaderPtr getAtlasLoader();

        /**
         * @see IObjectLoader::isLoadable
         */
        virtual bool isLoadable(const std::string& filename) const;

        /**
         * @see IObjectLoader::load
         */
        virtual void load(const std::string& filename);

        /** used to load an object, atlas or animation file
         * if directory is provided then file is assumed relative to directory
         */
        void loadImportFile(const std::string& file, const std::string& directory = "");

        /** used to load a directory of object, atlas or animation  files recursively
         */
        void loadImportDirectory(const std::string& directory);

    private:
        Model* m_model;
        VFS* m_vfs;
        ImageManager* m_imageManager;
        AnimationManager* m_animationManager;
        AnimationLoaderPtr m_animationLoader;
        AtlasLoaderPtr m_atlasLoader;
    };
} // namespace FIFE

#endif