/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

#ifndef FIFE_ANIMATION_MANAGER_H
#define FIFE_ANIMATION_MANAGER_H

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

#include "animation.h"

namespace FIFE {

	/** AnimationManager
	 *
	 * An interface for managing animations.
	 *
	 * @see IResource
	 * @see IResourceLoader
	 * @see IResourceManager
	 *
	 */

	class AnimationManager : public IResourceManager, public DynamicSingleton<AnimationManager> {
	public:

		/** Default constructor.
		 */
		AnimationManager() : IResourceManager() { }

		/** Destructor.
		 */
		virtual ~AnimationManager();

		virtual size_t getMemoryUsed() const;
		virtual size_t getTotalResourcesCreated() const;
		virtual size_t getTotalResourcesLoaded() const;
		virtual size_t getTotalResources() const;

		/** Creates a blank Animation but does not load it immediately
		 *
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return An AnimationPtr to the newly created Animation
		 *
		 * @see Animation
		 * @see AnimationLoader
		 * @see IResourceLoader
		 *
		 */
		virtual AnimationPtr create(IResourceLoader* loader = 0);

		/** Creates a blank Animation but does not load it immediately
		 *
		 * @param name The resource name. Typically a filename.
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return An AnimationPtr to the newly created Animation
		 *
		 * @see Animation
		 * @see AnimationLoader
		 * @see IResourceLoader
		 *
		 */
		virtual AnimationPtr create(const std::string& name, IResourceLoader* loader = 0);

		/** Creates a blank resource and loads it from disk
		 *
		 * This function will create the Animation if necessary and load
		 * the Animation from disk.  If the Animation is not defined it
		 * will call AnimationManager::create() before loading.
		 *
		 * @param name The resource name. Typically a filename.
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return A AnimationPtr to the loaded Animation
		 *
		 * @see Animation
		 * @see AnimationLoader
		 * @see IResourceLoader
		 *
		 */
		virtual AnimationPtr load(const std::string& name, IResourceLoader* loader = 0);

		/** Add an Animation to the manager
		 *
		 * This function will create a AnimationPtr and add the Animation
		 * to the manager.  The manager assumes ownership of the Animation
		 * so DO NOT delete it.
		 *
		 * @param res A pointer to the Animation
		 * @return A AnimationPtr to the added resource
		 *
		 * @see Animation
		 *
		 */
		virtual AnimationPtr add(Animation* res);

		/** Checks to see if an Animation exists
		 *
		 * This function will search the manager for Animations
		 * that match the parameter
		 *
		 * @param name The name of the Animation
		 * @return True if the Animation exists.  False otherwise.
		 *
		 */
		virtual bool exists(const std::string& name);

		/** Checks to see if an Animation exists
		 *
		 * This function will search the manager for Animations
		 * that match the parameter
		 *
		 * @param handle The handle of the Animation
		 * @return True if the Animation exists.  False otherwise.
		 *
		 */
		virtual bool exists(ResourceHandle handle);

		/** Reloads an Animation
		 *
		 * This function will reload an Animation if it is managed
		 * by the manager.  If not it creates an entry in the log
		 * specifying that the Animation could not be found.  It will
		 * load the Animation if it is not already loaded.
		 *
		 * @param name The name of the resource
		 *
		 */
		virtual void reload(const std::string& name);

		/** Reloads a resource
		 *
		 * This function will reload an Animation if it is managed
		 * by the manager.  If not it creates an entry in the log
		 * specifying that the Animation could not be found.  It will
		 * load the Animation if it is not already loaded.
		 *
		 * @param handle The handle of the resource
		 *
		 */
		virtual void reload(ResourceHandle handle);

		/** Reloads all Animations
		 *
		 * This function will reload all Animations managed by the
		 * manager.  It will load an Animation if it is not already loaded.
		 *
		 * @todo It might be beneficial to supply a parameter to
		 * only reload resources that are already loaded
		 *
		 */
		virtual void reloadAll();

		/** Loads all unreferenced Animations
		 *
		 * All Animations that have no external references will be
		 * loaded into memory.
		 *
		 */
		virtual void loadUnreferenced();

		/** Frees an Animation from memory
		 *
		 * The Animation is not deleted but it's data is freed.
		 * This calls the Animation::free() function and it is
		 * up to the resource to properly free it's memory.  The
		 * manager keeps a reference to the Animation in case
		 * its required in the future.
		 *
		 * @param name The name of the Animation
		 *
		 * @see Animation
		 *
		 */
		virtual void free(const std::string& name);

		/** Frees an Animation from memory
		 *
		 * The Animation is not deleted but it's data is freed.
		 * This calls the Animation::free() function and it is
		 * up to the resource to properly free it's memory.  The
		 * manager keeps a reference to the Animation in case
		 * its required in the future.
		 *
		 * @param handle The handle of the Animation
		 *
		 * @see Animation
		 *
		 */
		virtual void free(ResourceHandle handle);

		/** Frees all Animations
		 *
		 * This calls the Animation::free() function for every
		 * Animation the manager is managing.  It does not remove
		 * them from the manager.
		 *
		 * @see Animation
		 *
		 */
		virtual void freeAll();

		/** Frees all unreferenced Animation
		 *
		 * This calls the IResource::free() function for Animations
		 * that have no external references to them.  It does not
		 * remove them from the manager.
		 *
		 * @see IResource
		 *
		 */
		virtual void freeUnreferenced();

		/** Removes an Animation from the manager
		 *
		 * This removes all references to the Animation from the
		 * manager. It does not however guarantee that the resources
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param resource A AnimationPtr to the Animation to be removed
		 * from the manager
		 *
		 * @note This is useful if you want to remove ownership of a resource
		 * from the manager
		 *
		 * @see Animation
		 *
		 */
		virtual void remove(AnimationPtr& resource);

		/** Removes an Animation from the manager
		 *
		 * This removes all references to the Animation from the
		 * manager. It does not however guarantee that the resources
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param name The name of the Animation
		 *
		 * @see Animation
		 *
		 */
		virtual void remove(const std::string& name);

		/** Removes an Animation from the manager
		 *
		 * This removes all references to the Animation from the
		 * manager. It does not however guarantee that the resources
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param handle The handle of the Animation
		 *
		 * @see Animation
		 *
		 */
		virtual void remove(ResourceHandle handle);

		/** Removes all Animations from the manager
		 *
		 * This effectively removes all references to all Animations from
		 * the manager.  If there are left over shared pointers to
		 * any resources they will not be deleted.
		 *
		 * @see Animation
		 *
		 */
		virtual void removeAll();

		/** Removes all unreferenced Animations
		 *
		 * This effectively removes all Animations that dont have an
		 * external reference.  The resources will be deleted.
		 *
		 * @see Animation
		 *
		 */
		virtual void removeUnreferenced();

		/** Gets a shared pointer to the Animation
		 *
		 * If the Animation is not defined it will attempt to create
		 * and load the Animation based on the name (it assumes the name
		 * is a filename)
		 *
		 * @param name The name of the Animation
		 * @return An AnimationPtr to the Animation
		 *
		 */
		virtual AnimationPtr get(const std::string& name);

		/** Gets a shared pointer to the Animation
		 *
		 * If the resource is not defined it returns an empty
		 * (or invalid) AnimationPtr and makes an entry in the log.
		 *
		 * @param handle The handle of the resource
		 * @return A AnimationPtr to the resource
		 *
		 * @todo This should throw an exception instead of an
		 * empty AnimationPtr
		 *
		 */
		virtual AnimationPtr get(ResourceHandle handle);

		virtual AnimationPtr getPtr(const std::string& name);
		virtual AnimationPtr getPtr(ResourceHandle handle);

		/** Gets an Animation handle by name
		 *
		 * Returns the Animation handle associated with the name
		 *
		 * @param name The name of the Animation
		 * @return 0 if the resource name is invalid
		 *
		 */
		virtual ResourceHandle getResourceHandle(const std::string& name);

		virtual void invalidate(const std::string& name);
		virtual void invalidate(ResourceHandle handle);
		virtual void invalidateAll();

	private:
		typedef std::map< ResourceHandle, AnimationPtr > AnimationHandleMap;
		typedef std::map< ResourceHandle, AnimationPtr >::iterator AnimationHandleMapIterator;
		typedef std::map< ResourceHandle, AnimationPtr >::const_iterator AnimationHandleMapConstIterator;
		typedef std::pair< ResourceHandle, AnimationPtr > AnimationHandleMapPair;

		typedef std::map< std::string, AnimationPtr > AnimationNameMap;
		typedef std::map< std::string, AnimationPtr >::iterator AnimationNameMapIterator;
		typedef std::map< std::string, AnimationPtr >::const_iterator AnimationNameMapConstIterator;
		typedef std::pair< std::string, AnimationPtr > AnimationNameMapPair;

		AnimationHandleMap m_animHandleMap;

		AnimationNameMap m_animNameMap;
	};

} //FIFE

#endif //FIFE_ANIMATION_MANAGER_H
