// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IOBJECTSAVER_H_
#define FIFE_IOBJECTSAVER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "util/base/sharedptr.h"

#include "ianimationsaver.h"

namespace FIFE
{

    /** Interface class that all object savers should derive from
     */
    class /*FIFE_API*/ IObjectSaver
    {
    public:
        virtual ~IObjectSaver() = default;

        /** allows setting which animation saver will be
         * used to save animation files
         */
        virtual void setAnimationSaver(const AnimationSaverPtr& animationSaver) = 0;

        /** responsible for saving the object resource
         */
        virtual void save(const std::string& filename) = 0;
    };

    using ObjectSaverPtr = SharedPtr<FIFE::IObjectSaver>;
} // namespace FIFE

#endif
