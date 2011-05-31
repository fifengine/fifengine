/**************************************************************************
*   Copyright (C) 2005-2009 by the FIFE team                              *
*   http://www.fifengine.de                                               *
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

#ifndef FIFE_IMAPLOADER_H_
#define FIFE_IMAPLOADER_H_

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"

namespace FIFE {
	class Map;
	class ResourceLocation;

	/** Interface class that all map loaders should derive from
	 */
	class IMapLoader {
	public:
		virtual ~IMapLoader() { };

        /** determines whether the resource is in
        *	the correct format for this loader
        */
        virtual bool isLoadable(const std::string& filename) const = 0;

		/** responsible for loading the map resource
		 *	and populating the engine
		 */
		virtual Map* load(const std::string& filename) = 0;

	};
}

#endif
