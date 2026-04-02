// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IMAPLOADER_H_
#define FIFE_IMAPLOADER_H_

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "ianimationloader.h"
#include "iatlasloader.h"
#include "iobjectloader.h"

namespace FIFE
{
    class Map;

    /** Interface class that all map loaders should derive from
     */
    class IMapLoader
    {
    public:
        virtual ~IMapLoader() = default;

        /** allows setting which object loader will be
         * used to load object files
         */
        virtual void setObjectLoader(const FIFE::ObjectLoaderPtr& objectLoader) = 0;

        /** allows setting which animation loader will be
         * used to load animation files
         */
        virtual void setAnimationLoader(const FIFE::AnimationLoaderPtr& animationLoader) = 0;

        /** allows setting which atlas loader will be
         * used to load atlas files
         */
        virtual void setAtlasLoader(const FIFE::AtlasLoaderPtr& atlasLoader) = 0;

        /** determines whether the resource is in
         *	the correct format for this loader
         */
        virtual bool isLoadable(const std::string& filename) const = 0;

        /** responsible for loading the map resource
         *	and populating the engine
         */
        virtual Map* load(const std::string& filename) = 0;
    };
} // namespace FIFE

#endif
