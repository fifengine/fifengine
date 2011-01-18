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

		/** Destructor.
		 */
		virtual ~IResourceManager();

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
		virtual size_t getMemoryUsed() const;

		/** Returns the number of unloaded resources
		 *
		 * Gets the total number of resources created but not loaded.
		 * These resources have been defined in the resource manager
		 * but have yet to be loaded from disk.
		 *
		 * @return Total resources created
		 *
		 */
		virtual uint32_t getTotalResourcesCreated() const;

		/** Returns the number of loaded resources
		 *
		 * Gets the total number of resources created and loaded.
		 * These resources have been defined in the resource manager
		 * have been loaded from disk.
		 *
		 * @return Total resources loaded
		 *
		 */
		virtual uint32_t getTotalResourcesLoaded() const;

		/** Returns the number of defined resources
		 *
		 * Gets the total number of resources managed by the manager.
		 *
		 * @return Total resources managed
		 *
		 */
		virtual uint32_t getTotalResources() const;

		/** Creates a blank resource but does not load it immediately
		 *
		 * This is a pure virtual function.  The implementor must allocate
		 * memory for the type of resource the manager will manage.
		 * Typically from within this function you will call
		 * IResourceManager::add(IResource* res) to add the resource to
		 * the manager.
		 *
		 * @param name The resource name. Typically a filename.
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return A ResourcePtr to the newly created resource
		 *
		 * @see IResource
		 * @see IResourceLoader
		 *
		 */
		virtual ResourcePtr create(const std::string& name, IResourceLoader* loader = 0) = 0;

		/** Creates a blank resource and loads it from disk
		 *
		 * This function will create the resource if necessary and load
		 * the resource from disk.  If the resource is not defined it
		 * will call IResrouceManager::create() before loading.
		 *
		 * @param name The resource name. Typically a filename.
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return A ResourcePtr to the loaded resource
		 *
		 * @see IResource
		 * @see IResourceLoader
		 *
		 */
		virtual ResourcePtr load(const std::string& name, IResourceLoader* loader = 0);

		/** Add a resrouce to the manager
		 *
		 * This function will create a ResourcePtr and add the resource
		 * to the manager.  The manager assumes ownership of the resource
		 * so DO NOT delete it.
		 *
		 * @param res A pointer to the resource
		 * @return A ResourcePtr to the added resource
		 *
		 * @see IResource
		 *
		 */
		virtual ResourcePtr add(IResource* res);

		/** Checks to see if a resource exists
		 *
		 * This function will search the manager for resources
		 * that match the parameter
		 *
		 * @param name The name of the resource
		 * @return True if the resource exists.  False otherwise.
		 *
		 */
		virtual bool exists(const std::string& name);

		/** Checks to see if a resource exists
		 *
		 * This function will search the manager for resources
		 * that match the parameter
		 *
		 * @param handle The handle of the resource
		 * @return True if the resource exists.  False otherwise.
		 *
		 */
		virtual bool exists(ResourceHandle handle);

		/** Reloads a resource
		 *
		 * This function will reload a resource if it is managed
		 * by the manager.  If not it creates an entry in the log
		 * specifying that the resource could not be found.  It will
		 * load the resource if it is not already loaded.
		 *
		 * @param name The name of the resource
		 *
		 */
		virtual void reload(const std::string& name);

		/** Reloads a resource
		 *
		 * This function will reload a resource if it is managed
		 * by the manager.  If not it creates an entry in the log
		 * specifying that the resource could not be found.  It will
		 * load the resource if it is not already loaded.
		 *
		 * @param handle The handle of the resource
		 *
		 */
		virtual void reload(ResourceHandle handle);

		/** Reloads all resources
		 *
		 * This function will reload all resources managed by the
		 * manager.  It will load a resource if it is not already loaded.
		 *
		 * @param name The name of the resource
		 *
		 * @todo It might be beneficial to supply a parameter to
		 * only reload resources that are already loaded
		 *
		 */
		virtual void reloadAll();

		/** Loads all unreferenced resources
		 *
		 * All resources that have no external references will be
		 * loaded into memory.
		 *
		 */
		virtual void loadUnreferenced();

		/** Frees a resource from memory
		 *
		 * The resource is not deleted but it's data is freed.
		 * This calls the IResource::free() function and it is
		 * up to the resource to properly free it's memory.  The
		 * manager keeps a reference to the resource in case
		 * its required in the future.
		 *
		 * @param name The name of the resource
		 *
		 * @see IResource
		 *
		 */
		virtual void free(const std::string& name);

		/** Frees a resource from memory
		 *
		 * The resource is not deleted but it's data is freed.
		 * This calls the IResource::free() function and it is
		 * up to the resource to properly free it's memory.  The
		 * manager keeps a reference to the resource in case
		 * its required in the future.
		 *
		 * @param handle The handle of the resource
		 *
		 * @see IResource
		 *
		 */
		virtual void free(ResourceHandle handle);

		/** Frees all resources
		 *
		 * This calls the IResource::free() function for every
		 * resource the manager is managing.  It does not remove
		 * them from the manager.
		 *
		 * @see IResource
		 *
		 */
		virtual void freeAll();

		/** Frees all unreferenced resources
		 *
		 * This calls the IResource::free() function for resources
		 * that have no external references to them.  It does not
		 * remove them from the manager.
		 *
		 * @see IResource
		 *
		 */
		virtual void freeUnreferenced();

		/** Removes a resource
		 *
		 * This removes all references to the resource from the
		 * manager. It does not however guarantee that the resources
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param resource A ResourcePtr to the resource to be removed
		 * from the manager
		 *
		 * @note This is useful if you want to remove ownership of a resource
		 * from the manager
		 *
		 * @see IResource
		 *
		 */
		//free memory and remove from manager
		virtual void remove(ResourcePtr& resource); //useful to remove ownership from the manager

		/** Removes a resource
		 *
		 * This removes all references to the resource from the
		 * manager. It does not however guarantee that the resources
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param name The name of the resource
		 *
		 * @see IResource
		 *
		 */
		virtual void remove(const std::string& name);

		/** Removes a resource
		 *
		 * This removes all references to the resource from the
		 * manager. It does not however guarantee that the resources
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param handle The handle of the resource
		 *
		 * @see IResource
		 *
		 */
		virtual void remove(ResourceHandle handle);

		/** Removes all resources
		 *
		 * This effectively removes all references all resources from
		 * the manager.  If there are left over shared pointers to
		 * any resources they will not be deleted.
		 *
		 * @see IResource
		 *
		 */
		virtual void removeAll();

		/** Removes all unreferenced resources
		 *
		 * This effectively removes all resoiurces that dont have an
		 * external reference.  The resources will be deleted.
		 *
		 * @see IResource
		 *
		 */
		virtual void removeUnreferenced();

		/** Gets a shared pointer to the resource
		 *
		 * If the resource is not defined it will attempt to create
		 * and load the resource
		 *
		 * @param name The name of the resource
		 * @return A ResourcePtr to the resource
		 *
		 */
		virtual ResourcePtr get(const std::string& name);

		/** Gets a shared pointer to the resource
		 *
		 * If the resource is not defined it returns an empty
		 * (or invalid) SharedPtr and makes an entry in the log.
		 *
		 * @param handle The handle of the resource
		 * @return A ResourcePtr to the resource
		 *
		 */
		virtual ResourcePtr get(ResourceHandle handle);

		/** Gets a resource handle by name
		 *
		 * Returns the resource handle associated with the name
		 *
		 * @param name The name of the resource
		 * @return 0 if the resource name is invalid
		 *
		 */
		virtual ResourceHandle getResourceHandle(const std::string& name);

	private:
		typedef std::map< ResourceHandle, ResourcePtr > ResourceHandleMap;
		typedef std::map< ResourceHandle, ResourcePtr >::iterator ResourceHandleMapIterator;
		typedef std::map< ResourceHandle, ResourcePtr >::const_iterator ResourceHandleMapConstIterator;
		typedef std::pair< ResourceHandle, ResourcePtr > ResourceHandleMapPair;

		typedef std::map< std::string, ResourcePtr > ResourceNameMap;
		typedef std::map< std::string, ResourcePtr >::iterator ResourceNameMapIterator;
		typedef std::map< std::string, ResourcePtr >::const_iterator ResourceNameMapConstIterator;
		typedef std::pair< std::string, ResourcePtr > ResourceNameMapPair;

		ResourceHandleMap m_resHandleMap;

		ResourceNameMap m_resNameMap;
	};

} //FIFE

#endif
