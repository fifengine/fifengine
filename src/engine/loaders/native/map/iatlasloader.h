// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IATLAS_LOADER_H
#define FIFE_IATLAS_LOADER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "video/imagemanager.h"

namespace FIFE
{
    class Atlas;
    using AtlasPtr = SharedPtr<Atlas>;

    /**
     * Interface class that all atlas loaders should derive from
     */
    class /*FIFE_API*/ IAtlasLoader
    {
        public:
            virtual ~IAtlasLoader() = default;

            /** determines whether the resource is in
             *	the correct format for this loader
             */
            virtual bool isLoadable(std::string const & filename) = 0;

            /** responsible for loading the atlas
             * returns a shared pointer to an image resource
             */
            virtual AtlasPtr load(std::string const & filename) = 0;

            /** responsible for loading the all atlases
             * returns a vector of shared pointer to an image resource
             */
            virtual std::vector<AtlasPtr> loadMultiple(std::string const & filename) = 0;
    };

    using AtlasLoaderPtr = SharedPtr<FIFE::IAtlasLoader>;
} // namespace FIFE

#endif
