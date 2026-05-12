// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IOBJECTLOADER_H_
#define FIFE_IOBJECTLOADER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "ianimationloader.h"
#include "iatlasloader.h"
#include "util/base/sharedptr.h"

namespace FIFE
{

    /**
     * Interface class that all object loaders should derive from
     */
    class FIFE_API IObjectLoader
    {
        public:
            virtual ~IObjectLoader() = default;

            /** allows setting which animation loader will be
             * used to load animation files
             */
            virtual void setAnimationLoader(AnimationLoaderPtr const & animationLoader) = 0;

            virtual AnimationLoaderPtr getAnimationLoader() = 0;

            /** allows setting which atlas loader will be
             * used to load atlas files
             */
            virtual void setAtlasLoader(AtlasLoaderPtr const & atlasLoader) = 0;

            virtual AtlasLoaderPtr getAtlasLoader() = 0;

            /** determines whether the resource is in
             *	the correct format for this loader
             */
            virtual bool isLoadable(std::string const & filename) const = 0;

            /** responsible for loading the object resource
             *	and populating the engine
             */
            virtual void load(std::string const & filename) = 0;
    };

    using ObjectLoaderPtr = SharedPtr<FIFE::IObjectLoader>;
} // namespace FIFE

#endif
