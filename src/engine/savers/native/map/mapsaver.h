// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MAPSAVER_H_
#define FIFE_MAPSAVER_H_

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "imapsaver.h"

namespace FIFE
{
    class Map;

    /** default map saver class implementing the IMapSaver interface
     */
    class MapSaver : public IMapSaver
    {
    public:
        /** constructor
         */
        MapSaver();

        /** destructor
         */
        ~MapSaver();

        /** allows setting which object saver will be
         * used to save object files
         */
        virtual void setObjectSaver(const FIFE::ObjectSaverPtr& objectSaver);

        /** allows setting which animation saver will be
         * used to save animation files
         */
        virtual void setAnimationSaver(const FIFE::AnimationSaverPtr& animationSaver);

        /** allows setting which atlas saver will be
         * used to save atlas files
         */
        virtual void setAtlasSaver(const FIFE::AtlasSaverPtr& atlasSaver);

        /** responsible for saving the map resource
         * used to save map files
         */
        virtual void save(const Map& map, const std::string& filename, const std::vector<std::string>& importFiles);

    private:
        ObjectSaverPtr m_objectSaver;
        AnimationSaverPtr m_animationSaver;
        AtlasSaverPtr m_atlasSaver;
    };
} // namespace FIFE

#endif