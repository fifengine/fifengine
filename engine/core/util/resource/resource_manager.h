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

	class IResourceManager {
	public:
		IResourceManager() { }
		virtual ~IResourceManager();

		//gets the total amount of memory used by resources
		virtual size_t getMemoryUsed() const;

		//gets the total number of resources created but not loaded
		virtual uint32_t getTotalResourcesCreated();

		//gets the total number of resource loaded
		virtual uint32_t getTotalResourcesLoaded();

		//gets the total number of resources defined
		virtual uint32_t getTotalResources();

		//creates a blank resource but does not load it immediately
		virtual ResourcePtr create(const std::string& name, IResourceLoader* loader = 0) = 0;

		//creates a resources and loads it
		virtual ResourcePtr load(const std::string& name, IResourceLoader* loader = 0);

		//add a resrouce to the manager that the manager did not load
		virtual ResourcePtr add(IResource* res);

		//checks to make sure resouce exists
		virtual bool exists(const std::string& name);
		virtual bool exists(ResourceHandle handle);

		//forces a reload of a resource (even if it's not loaded)
		virtual void reload(const std::string& name);
		virtual void reload(ResourcePtr& res);
		virtual void reloadAll();

		//loads all unreferenced resources
		virtual void loadUnreferenced();

		//free memory but dont remove from manager
		virtual void free(const std::string& name);
		virtual void free(ResourceHandle handle);
		virtual void freeAll();

		//all unreferenced resources will be freed but not removed from manager
		virtual void freeUnreferenced();

		//free memory and remove from manager
		virtual void remove(ResourcePtr& resource);
		virtual void remove(const std::string& name);
		virtual void remove(ResourceHandle handle);
		virtual void removeAll();

		//frees and removes all unreferenced resources
		virtual void removeUnreferenced();

		//get a shared pointer to the resource
		//loads the resource if required
		virtual ResourcePtr get(const std::string& name);
		virtual ResourcePtr get(ResourceHandle handle);

		//get the resource handle by name
		virtual ResourceHandle getResourceHandle(const std::string& name);

	protected:
		typedef std::map< ResourceHandle, ResourcePtr > ResourceHandleMap;
		typedef std::map< ResourceHandle, ResourcePtr >::iterator ResourceHandleMapIterator;
		typedef std::map< ResourceHandle, ResourcePtr >::const_iterator ResourceHandleMapConstIterator;
		typedef std::pair< ResourceHandle, ResourcePtr > ResourceHandleMapPair;

		typedef std::map< std::string, ResourceHandle > ResourceNameHandleMap;
		typedef std::map< std::string, ResourceHandle >::iterator ResourceNameHandleMapIterator;
		typedef std::map< std::string, ResourceHandle >::const_iterator ResourceNameHandleMapConstIterator;
		typedef std::pair< std::string, ResourceHandle > ResourceNameHandleMapPair;

		ResourceHandleMap m_resHandleMap;

		//@todo implement this for faster name lookup
		ResourceNameHandleMap m_resNameHandleMap;
	};

} //FIFE

#endif
