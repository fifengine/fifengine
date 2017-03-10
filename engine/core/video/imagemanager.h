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

#ifndef FIFE_IMAGE_MANAGER_H
#define FIFE_IMAGE_MANAGER_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/singleton.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"

#include "image.h"

namespace FIFE {

	/** ImageManager
	 *
	 * An interface for managing images.
	 *
	 * @see IResource
	 * @see IResourceLoader
	 * @see IResourceManager
	 *
	 */

	class ImageManager : public IResourceManager, public DynamicSingleton<ImageManager> {
	public:

		/** Default constructor.
		 */
		ImageManager() : IResourceManager() { }

		/** Destructor.
		 */
		virtual ~ImageManager();

		virtual size_t getMemoryUsed() const;
		virtual size_t getTotalResourcesCreated() const;
		virtual size_t getTotalResourcesLoaded() const;
		virtual size_t getTotalResources() const;

		/** Creates a blank Image but does not load it immediately
		 *
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return An ImagePtr to the newly created Image
		 *
		 * @see Image
		 * @see ImageLoader
		 * @see IResourceLoader
		 *
		 */
		virtual ImagePtr create(IResourceLoader* loader = 0);

		/** Creates a blank Image but does not load it immediately
		 *
		 * @param name The resource name. Typically a filename.
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return An ImagePtr to the newly created Image
		 *
		 * @see Image
		 * @see ImageLoader
		 * @see IResourceLoader
		 *
		 */
		virtual ImagePtr create(const std::string& name, IResourceLoader* loader = 0);

		/** Creates a blank resource and loads it from disk
		 *
		 * This function will create the Image if necessary and load
		 * the Image from disk.  If the Image is not defined it
		 * will call ImageManager::create() before loading.
		 *
		 * @param name The resource name. Typically a filename.
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return A ImagePtr to the loaded Image
		 *
		 * @see Image
		 * @see ImageLoader
		 * @see IResourceLoader
		 *
		 */
		virtual ImagePtr load(const std::string& name, IResourceLoader* loader = 0);

		/** Loads a blank resource
		 *
		 * @param width
		 * @param height
		 * @return An ImagePtr to the blank image (ready to be modified)
		 */
		virtual ImagePtr loadBlank(uint32_t width, uint32_t height);

		/** Loads a blank resource
		 *
		 * @param name
		 * @param width
		 * @param height
		 * @return An ImagePtr to the blank image (ready to be modified)
		 */
		virtual ImagePtr loadBlank(const std::string& name, uint32_t width, uint32_t height);

		/** Add an Image to the manager
		 *
		 * This function will create a ImagePtr and add the Image
		 * to the manager.  The manager assumes ownership of the Image
		 * so DO NOT delete it.
		 *
		 * @param res A pointer to the Image
		 * @return A ImagePtr to the added resource
		 *
		 * @see Image
		 *
		 */
		virtual ImagePtr add(Image* res);

		/** Checks to see if an Image exists
		 *
		 * This function will search the manager for Images
		 * that match the parameter
		 *
		 * @param name The name of the Image
		 * @return True if the Image exists.  False otherwise.
		 *
		 */
		virtual bool exists(const std::string& name);

		/** Checks to see if an Image exists
		 *
		 * This function will search the manager for Images
		 * that match the parameter
		 *
		 * @param handle The handle of the Image
		 * @return True if the Image exists.  False otherwise.
		 *
		 */
		virtual bool exists(ResourceHandle handle);

		/** Reloads an Image
		 *
		 * This function will reload an Image if it is managed
		 * by the manager.  If not it creates an entry in the log
		 * specifying that the Image could not be found.  It will
		 * load the Image if it is not already loaded.
		 *
		 * @param name The name of the resource
		 *
		 */
		virtual void reload(const std::string& name);

		/** Reloads a resource
		 *
		 * This function will reload an Image if it is managed
		 * by the manager.  If not it creates an entry in the log
		 * specifying that the Image could not be found.  It will
		 * load the Image if it is not already loaded.
		 *
		 * @param handle The handle of the resource
		 *
		 */
		virtual void reload(ResourceHandle handle);

		/** Reloads all Images
		 *
		 * This function will reload all Images managed by the
		 * manager.  It will load an Image if it is not already loaded.
		 *
		 * @todo It might be beneficial to supply a parameter to
		 * only reload resources that are already loaded
		 *
		 */
		virtual void reloadAll();

		/** Loads all unreferenced Images
		 *
		 * All Images that have no external references will be
		 * loaded into memory.
		 *
		 */
		virtual void loadUnreferenced();

		/** Frees an Image from memory
		 *
		 * The Image is not deleted but it's data is freed.
		 * This calls the Image::free() function and it is
		 * up to the resource to properly free it's memory.  The
		 * manager keeps a reference to the Image in case
		 * its required in the future.
		 *
		 * @param name The name of the Image
		 *
		 * @see Image
		 *
		 */
		virtual void free(const std::string& name);

		/** Frees an Image from memory
		 *
		 * The Image is not deleted but it's data is freed.
		 * This calls the Image::free() function and it is
		 * up to the resource to properly free it's memory.  The
		 * manager keeps a reference to the Image in case
		 * its required in the future.
		 *
		 * @param handle The handle of the Image
		 *
		 * @see Image
		 *
		 */
		virtual void free(ResourceHandle handle);

		/** Frees all Images
		 *
		 * This calls the Image::free() function for every
		 * Image the manager is managing.  It does not remove
		 * them from the manager.
		 *
		 * @see Image
		 *
		 */
		virtual void freeAll();

		/** Frees all unreferenced Image
		 *
		 * This calls the IResource::free() function for Images
		 * that have no external references to them.  It does not
		 * remove them from the manager.
		 *
		 * @see IResource
		 *
		 */
		virtual void freeUnreferenced();

		/** Removes an Image from the manager
		 *
		 * This removes all references to the Image from the
		 * manager. It does not however guarantee that the resources
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param resource A ImagePtr to the image to be removed
		 * from the manager
		 *
		 * @note This is useful if you want to remove ownership of a resource
		 * from the manager
		 *
		 * @see Image
		 *
		 */
		virtual void remove(ImagePtr& resource);

		/** Removes an Image from the manager
		 *
		 * This removes all references to the Image from the
		 * manager. It does not however guarantee that the resources
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param name The name of the Image
		 *
		 * @see Image
		 *
		 */
		virtual void remove(const std::string& name);

		/** Removes an Image from the manager
		 *
		 * This removes all references to the Image from the
		 * manager. It does not however guarantee that the resources
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param handle The handle of the Image
		 *
		 * @see Image
		 *
		 */
		virtual void remove(ResourceHandle handle);

		/** Removes all Images from the manager
		 *
		 * This effectively removes all references to all Images from
		 * the manager.  If there are left over shared pointers to
		 * any resources they will not be deleted.
		 *
		 * @see Image
		 *
		 */
		virtual void removeAll();

		/** Removes all unreferenced Images
		 *
		 * This effectively removes all Images that dont have an
		 * external reference.  The resources will be deleted.
		 *
		 * @see Image
		 *
		 */
		virtual void removeUnreferenced();

		/** Gets a shared pointer to the Image
		 *
		 * If the Image is not defined it will attempt to create
		 * and load the Image based on the name (it assumes the name
		 * is a filename)
		 *
		 * @param name The name of the Image
		 * @return An ImagePtr to the Image
		 *
		 */
		virtual ImagePtr get(const std::string& name);

		/** Gets a shared pointer to the Image
		 *
		 * If the resource is not defined it returns an empty
		 * (or invalid) ImagePtr and makes an entry in the log.
		 *
		 * @param handle The handle of the resource
		 * @return A ImagePtr to the resource
		 *
		 * @todo This should throw an exception instead of an
		 * empty ImagePtr
		 *
		 */
		virtual ImagePtr get(ResourceHandle handle);

		virtual ImagePtr getPtr(const std::string& name);
		virtual ImagePtr getPtr(ResourceHandle handle);

		/** Gets an Image handle by name
		 *
		 * Returns the Image handle associated with the name
		 *
		 * @param name The name of the Image
		 * @return 0 if the resource name is invalid
		 *
		 */
		virtual ResourceHandle getResourceHandle(const std::string& name);

		virtual void invalidate(const std::string& name);
		virtual void invalidate(ResourceHandle handle);
		virtual void invalidateAll();

	private:
		typedef std::map< ResourceHandle, ImagePtr > ImageHandleMap;
		typedef std::map< ResourceHandle, ImagePtr >::iterator ImageHandleMapIterator;
		typedef std::map< ResourceHandle, ImagePtr >::const_iterator ImageHandleMapConstIterator;
		typedef std::pair< ResourceHandle, ImagePtr > ImageHandleMapPair;

		typedef std::map< std::string, ImagePtr > ImageNameMap;
		typedef std::map< std::string, ImagePtr >::iterator ImageNameMapIterator;
		typedef std::map< std::string, ImagePtr >::const_iterator ImageNameMapConstIterator;
		typedef std::pair< std::string, ImagePtr > ImageNameMapPair;

		ImageHandleMap m_imgHandleMap;

		ImageNameMap m_imgNameMap;
	};

} //FIFE

#endif //FIFE_IMAGE_MANAGER_H
