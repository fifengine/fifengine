/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_RESOURE_H
#define FIFE_RESOURE_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "resource_location.h"

namespace FIFE {

	/** IResource is the internal representation of a loaded file.
	 * One resource is always associated with one file (resource location).
	 */
	class IResource {
	public:
		virtual ~IResource() { };

		/** Get the location/file of this resource.
		 */
		virtual const ResourceLocation& getResourceLocation() = 0;
		virtual const std::string& getResourceFile() = 0;

		/** Change the location/file of this resource.
		 */
		virtual void setResourceLocation(const ResourceLocation& location) = 0;
		virtual void setResourceFile(const std::string& filename) = 0;
	};

	/** IResourceLoader defines an interface for loading resources. The resource returned is owned by
	 * the caller, and must be deleted when finished.
	 */
	class IResourceLoader {
	public:
		virtual ~IResourceLoader() { };

		virtual IResource* loadResourceFromLocation(const ResourceLocation& location) = 0;
		virtual IResource* loadResourceFromFile(const std::string& filename) = 0;
	};

	/** IResourceSaver defines an interface for saving Resources. 
	 */
	class IResourceSaver {
	public:
		virtual ~IResourceSaver() { };

		virtual void saveResourceToLocation(const ResourceLocation& location, IResource* resource) = 0;
		virtual void saveResourceToFile(const std::string& filename, IResource* resource) = 0;
	};
}

#endif
