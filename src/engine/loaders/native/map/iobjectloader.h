// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IOBJECTLOADER_H_
#define FIFE_IOBJECTLOADER_H_

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

namespace FIFE
{

    /** Interface class that all object loaders should derive from
     */
    class IObjectLoader
    {
    public:
        virtual ~IObjectLoader() = default;

        /** allows setting which animation loader will be
         * used to load animation files
         */
        virtual void setAnimationLoader(const AnimationLoaderPtr& animationLoader) = 0;

        virtual AnimationLoaderPtr getAnimationLoader() = 0;

        /** allows setting which atlas loader will be
         * used to load atlas files
         */
        virtual void setAtlasLoader(const AtlasLoaderPtr& atlasLoader) = 0;

        virtual AtlasLoaderPtr getAtlasLoader() = 0;

        /** determines whether the resource is in
         *	the correct format for this loader
         */
        virtual bool isLoadable(const std::string& filename) const = 0;

        /** responsible for loading the object resource
         *	and populating the engine
         */
        virtual void load(const std::string& filename) = 0;
    };

    using ObjectLoaderPtr = SharedPtr<FIFE::IObjectLoader>;
} // namespace FIFE

#endif
