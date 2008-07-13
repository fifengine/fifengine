/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
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

	class IReferenceCounted {
	public:
		virtual ~IReferenceCounted() { }
		
		/** Calling this method marks resource be used by some resource client.
		 *  It adds one to resource counter that is kept up by the resource itself.
		 *  When resource is about to be deleted (e.g. due to pooling algorithms), 
		 *  reference counter is inspected. In case value is non-zero, resource 
		 *  shouldn't be deleted.
		 */
		virtual void addRef() = 0;

		/** Calling this method unmarks resource be used by a resource client.
		 *  @see addRef
		 */
		virtual void decRef() = 0;

		/** Gets the current reference count
		 *  @see addRef
		 */
		virtual unsigned int getRefCount() = 0;
	};

	/** IResource is the internal representation of a loaded file.
	 * One resource is always associated with one file (resource location).
	 */
	class IResource: public virtual IReferenceCounted {
	public:
		virtual ~IResource() { }

		/** Get the location/file of this resource.
		 */
		virtual const ResourceLocation& getResourceLocation() = 0;
		virtual const std::string& getResourceFile() = 0;

		/** Change the location/file of this resource.
		 */
		virtual void setResourceLocation(const ResourceLocation& location) = 0;
		virtual void setResourceFile(const std::string& filename) = 0;
		
		/** returns -1 if not pooled, otherwise its a valid id
		 */
		virtual int getPoolId() = 0;
		virtual void setPoolId(int poolid) = 0;
	};

	/** ResourceLoader defines a class for loading resources. The resource returned is owned by
	 * the caller, and must be deleted when finished.
	 * In case allocation fails, or loader cannot load given location, NULL is returned
	 */
	class ResourceLoader {
	public:
		virtual ~ResourceLoader() { };
		virtual IResource* loadResource(const ResourceLocation& location) = 0;
	};

	/** ResourceSaver defines a class for saving Resources. 
	 */
	class ResourceSaver {
	public:
		virtual ~ResourceSaver() { };

		virtual void save(const ResourceLocation& location, IResource* resource) = 0;
		virtual void save(const std::string& filename, IResource* resource) { save(ResourceLocation(filename), resource); }
	};
}

#endif
