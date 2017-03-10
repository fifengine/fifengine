/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_IMAPSAVER_H_
#define FIFE_IMAPSAVER_H_

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "iatlassaver.h"
#include "iobjectsaver.h"
#include "ianimationsaver.h"

namespace FIFE {
	class Map;

	/** Interface class that all map savers should derive from
	 */
	class IMapSaver {
	public:
		virtual ~IMapSaver() { };

        /** allows setting which object saver will be
        * used to save object files
        */
        virtual void setObjectSaver(const FIFE::ObjectSaverPtr& objectSaver) = 0;

        /** allows setting which animation saver will be
        * used to save animation files
        */
        virtual void setAnimationSaver(const FIFE::AnimationSaverPtr& animationSaver) = 0;

        /** allows setting which atlas saver will be
        * used to save atlas files
        */
        virtual void setAtlasSaver(const FIFE::AtlasSaverPtr& atlasSaver) = 0;

		/** responsible for saving the map resource
		 * used to save map files
		 */
		virtual void save(const Map& map, const std::string& filename, const std::vector<std::string>& importFiles) = 0;

	};
}

#endif
