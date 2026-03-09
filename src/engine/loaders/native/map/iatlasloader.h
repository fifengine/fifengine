// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IATLAS_LOADER_H
#define FIFE_IATLAS_LOADER_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/imagemanager.h"

namespace FIFE
{
    class Atlas;
    typedef SharedPtr<Atlas> AtlasPtr;

    class IAtlasLoader
    {
    public:
        virtual ~IAtlasLoader() { }

        /** determines whether the resource is in
         *	the correct format for this loader
         */
        virtual bool isLoadable(const std::string& filename) = 0;

        /** responsible for loading the atlas
         * returns a shared pointer to an image resource
         */
        virtual AtlasPtr load(const std::string& filename) = 0;

        /** responsible for loading the all atlases
         * returns a vector of shared pointer to an image resource
         */
        virtual std::vector<AtlasPtr> loadMultiple(const std::string& filename) = 0;
    };

    typedef SharedPtr<FIFE::IAtlasLoader> AtlasLoaderPtr;
} // namespace FIFE

#endif
