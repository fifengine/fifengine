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

#ifndef FIFE_RESOURE_MANAGER_H
#define FIFE_RESOURE_MANAGER_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "resource.h"

namespace FIFE {

	/** IResourceManager
	 *
	 * An interface for managing resources.
	 *
	 * @see IResource
	 * @see IResourceLoader
	 *
	 */

	class IResourceManager {
	public:

		/** Default constructor.
		 */
		IResourceManager() { }

		/** destructor
		 */
		virtual ~IResourceManager() { }

		/** Gets the total amount of memory used by resources
		 *
		 * This function uses the IResource's getSize() function
		 * to calculate the total memory used.  It does not include
		 * memory used by the internal resource manager book keeping
		 * structures.  This value is only accurate if the resources
		 * getSize() function returns an accurate result.
		 *
		 * @return Total memory used
		 *
		 */
		virtual size_t getMemoryUsed() const = 0;

		/** Returns the number of unloaded resources
		 *
		 * Gets the total number of resources created but not loaded.
		 * These resources have been defined in the resource manager
		 * but have yet to be loaded from disk.
		 *
		 * @return Total resources created
		 *
		 */
		virtual size_t getTotalResourcesCreated() const = 0;

		/** Returns the number of loaded resources
		 *
		 * Gets the total number of resources created and loaded.
		 * These resources have been defined in the resource manager
		 * have been loaded from disk.
		 *
		 * @return Total resources loaded
		 *
		 */
		virtual size_t getTotalResourcesLoaded() const = 0;

		/** Returns the number of defined resources
		 *
		 * Gets the total number of resources managed by the manager.
		 *
		 * @return Total resources managed
		 *
		 */
		virtual size_t getTotalResources() const = 0;
	};

} //FIFE

#endif
