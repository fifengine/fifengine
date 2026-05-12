// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MAPSAVER_H_
#define FIFE_MAPSAVER_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "imapsaver.h"

namespace FIFE
{
    class Map;

    /**
     * Default map saver class implementing the IMapSaver interface
     */
    class FIFE_API MapSaver : public IMapSaver
    {
        public:
            /**
             * Constructor
             */
            MapSaver();

            /**
             * Destructor
             */
            ~MapSaver() override;

            void setObjectSaver(FIFE::ObjectSaverPtr const & objectSaver) override;

            void setAnimationSaver(FIFE::AnimationSaverPtr const & animationSaver) override;

            void setAtlasSaver(FIFE::AtlasSaverPtr const & atlasSaver) override;

            void save(
                Map const & map, std::string const & filename, std::vector<std::string> const & importFiles) override;

        private:
            ObjectSaverPtr m_objectSaver;
            AnimationSaverPtr m_animationSaver;
            AtlasSaverPtr m_atlasSaver;
    };
} // namespace FIFE

#endif
