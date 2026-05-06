// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_OBJECT_LOADER_H_
#define FIFE_OBJECT_LOADER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "ianimationloader.h"
#include "iatlasloader.h"
#include "iobjectloader.h"
#include "util/base/sharedptr.h"

namespace FIFE
{

    class Model;
    class VFS;
    class ImageManager;
    class AnimationManager;

    class /*FIFE_API*/ ObjectLoader : public IObjectLoader
    {
        public:
            ObjectLoader(
                Model* model,
                VFS* vfs,
                ImageManager* imageManager,
                AnimationManager* animationManager,
                AnimationLoaderPtr const & animationLoader = AnimationLoaderPtr(),
                AtlasLoaderPtr const & atlasLoader         = AtlasLoaderPtr());

            ~ObjectLoader() override;

            void setAnimationLoader(AnimationLoaderPtr const & animationLoader) override;

            AnimationLoaderPtr getAnimationLoader() override;

            void setAtlasLoader(AtlasLoaderPtr const & atlasLoader) override;

            AtlasLoaderPtr getAtlasLoader() override;

            bool isLoadable(std::string const & filename) const override;

            void load(std::string const & filename) override;

            /**
             * Used to load an object, atlas or animation file
             * if directory is provided then file is assumed relative to directory
             */
            void loadImportFile(std::string const & file, std::string const & directory = "");

            /**
             * Used to load a directory of object, atlas or animation  files recursively
             */
            void loadImportDirectory(std::string const & directory);

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
