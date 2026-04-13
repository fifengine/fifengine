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
    class /*FIFE_API*/ MapSaver : public IMapSaver
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

        void setObjectSaver(const FIFE::ObjectSaverPtr& objectSaver) override;

        void setAnimationSaver(const FIFE::AnimationSaverPtr& animationSaver) override;

        void setAtlasSaver(const FIFE::AtlasSaverPtr& atlasSaver) override;

        void save(const Map& map, const std::string& filename, const std::vector<std::string>& importFiles) override;

    private:
        ObjectSaverPtr m_objectSaver;
        AnimationSaverPtr m_animationSaver;
        AtlasSaverPtr m_atlasSaver;
    };
} // namespace FIFE

#endif
