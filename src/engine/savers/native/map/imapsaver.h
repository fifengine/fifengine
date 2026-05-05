// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IMAPSAVER_H_
#define FIFE_IMAPSAVER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "ianimationsaver.h"
#include "iatlassaver.h"
#include "iobjectsaver.h"

namespace FIFE
{
    class Map;

    /**
     * Interface class that all map savers should derive from
     */
    class /*FIFE_API*/ IMapSaver
    {
        public:
            virtual ~IMapSaver() = default;

            /** allows setting which object saver will be
             * used to save object files
             */
            virtual void setObjectSaver(FIFE::ObjectSaverPtr const & objectSaver) = 0;

            /** allows setting which animation saver will be
             * used to save animation files
             */
            virtual void setAnimationSaver(FIFE::AnimationSaverPtr const & animationSaver) = 0;

            /** allows setting which atlas saver will be
             * used to save atlas files
             */
            virtual void setAtlasSaver(FIFE::AtlasSaverPtr const & atlasSaver) = 0;

            /** responsible for saving the map resource
             * used to save map files
             */
            virtual void save(
                Map const & map, std::string const & filename, std::vector<std::string> const & importFiles) = 0;
    };
} // namespace FIFE

#endif
