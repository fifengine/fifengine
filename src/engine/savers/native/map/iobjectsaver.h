// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IOBJECTSAVER_H_
#define FIFE_IOBJECTSAVER_H_

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/sharedptr.h"

#include "ianimationsaver.h"

namespace FIFE
{

    /** Interface class that all object savers should derive from
     */
    class IObjectSaver
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
