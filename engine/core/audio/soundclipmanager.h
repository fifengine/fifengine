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

#ifndef FIFE_SOUNDCLIP_MANAGER_H
#define FIFE_SOUNDCLIP_MANAGER_H

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

#include "soundclip.h"

namespace FIFE {

	/** SoundClipManager
	 *
	 * An interface for managing images.
	 *
	 * @see IResource
	 * @see IResourceLoader
	 * @see IResourceManager
	 *
	 */

	class SoundClipManager : public IResourceManager, public DynamicSingleton<SoundClipManager> {
	public:

		/** Default constructor.
		 */
		SoundClipManager() : IResourceManager() { }

		/** Destructor.
		 */
		virtual ~SoundClipManager();

		virtual size_t getMemoryUsed() const;
		virtual size_t getTotalResourcesCreated() const;
		virtual size_t getTotalResourcesLoaded() const;
		virtual size_t getTotalResources() const;

		/** Creates a blank SoundClip but does not load it immediately
		 *
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return A SoundClipPtr to the newly created SoundClip
		 *
		 * @see SoundClip
		 * @see OggLoader
		 * @see IResourceLoader
		 *
		 */
		virtual SoundClipPtr create(IResourceLoader* loader = 0);

		/** Creates a blank SoundClip but does not load it immediately
		 *
		 * @param name The resource name. Typically a filename.
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return A SoundClipPtr to the newly created SoundClip
		 *
		 * @see SoundClip
		 * @see OggLoader
		 * @see IResourceLoader
		 *
		 */
		virtual SoundClipPtr create(const std::string& name, IResourceLoader* loader = 0);

		/** Creates a blank resource and loads it from disk
		 *
		 * This function will create the SoundClip if necessary and load
		 * the clip from disk.  If the SoundClip is not defined it
		 * will call SoundClipManager::create() before loading.
		 *
		 * @param name The resource name. Typically a filename.
		 * @param loader A pointer to the custom resource loader.  The
		 * default is NULL.  If this parameter is present the resource
		 * will use the loader to load instead of the default built in
		 * functions.
		 * @return A SoundClipPtr to the loaded SoundClip
		 *
		 * @see SoundClip
		 * @see OggLoader
		 * @see IResourceLoader
		 *
		 */
		virtual SoundClipPtr load(const std::string& name, IResourceLoader* loader = 0);

		/** Add an Image to the manager
		 *
		 * This function will create a SoundClipPtr and add the SoundClip
		 * to the manager.  The manager assumes ownership of the SoundClip
		 * so DO NOT delete it.
		 *
		 * @param res A pointer to the SoundClip
		 * @return A SoundClipPtr to the added resource
		 *
		 * @see SoundClip
		 *
		 */
		virtual SoundClipPtr add(SoundClip* res);

		/** Checks to see if a SoundClip exists
		 *
		 * This function will search the manager for SoundClips
		 * that match the parameter
		 *
		 * @param name The name of the SoundClip
		 * @return True if the SoundClip exists.  False otherwise.
		 *
		 */
		virtual bool exists(const std::string& name);

		/** Checks to see if a SoundClip exists
		 *
		 * This function will search the manager for SoundClips
		 * that match the parameter
		 *
		 * @param handle The handle of the SoundClip
		 * @return True if the SoundClip exists.  False otherwise.
		 *
		 */
		virtual bool exists(ResourceHandle handle);

		/** Reloads a SoundClip
		 *
		 * This function will reload a SoundClip if it is managed
		 * by the manager.  If not it creates an entry in the log
		 * specifying that the SoundClip could not be found.  It will
		 * load the SoundClip if it is not already loaded.
		 *
		 * @param name The name of the resource
		 *
		 */
		virtual void reload(const std::string& name);

		/** Reloads a resource
		 *
		 * This function will reload a SoundClip if it is managed
		 * by the manager.  If not it creates an entry in the log
		 * specifying that the SoundClip could not be found.  It will
		 * load the SoundClip if it is not already loaded.
		 *
		 * @param handle The handle of the resource
		 *
		 */
		virtual void reload(ResourceHandle handle);

		/** Reloads all SoundClips
		 *
		 * This function will reload all SoundClips managed by the
		 * manager.  It will load a SoundClip if it is not already loaded.
		 *
		 * @todo It might be beneficial to supply a parameter to
		 * only reload resources that are already loaded
		 *
		 */
		virtual void reloadAll();

		/** Loads all unreferenced SoundClips
		 *
		 * All SoundClips that have no external references will be
		 * loaded into memory.
		 *
		 */
		virtual void loadUnreferenced();

		/** Frees a SoundClip from memory
		 *
		 * The SoundClip is not deleted but it's data is freed.
		 * This calls the SoundClip::free() function and it is
		 * up to the resource to properly free it's memory.  The
		 * manager keeps a reference to the SoundClip in case
		 * its required in the future.
		 *
		 * @param name The name of the SoundClip
		 *
		 * @see SoundClip
		 *
		 */
		virtual void free(const std::string& name);

		/** Frees a SoundClip from memory
		 *
		 * The SoundClip is not deleted but it's data is freed.
		 * This calls the SoundClip::free() function and it is
		 * up to the resource to properly free it's memory.  The
		 * manager keeps a reference to the SoundClip in case
		 * its required in the future.
		 *
		 * @param handle The handle of the SoundClip
		 *
		 * @see SoundClip
		 *
		 */
		virtual void free(ResourceHandle handle);

		/** Frees all SoundClips
		 *
		 * This calls the SoundClip::free() function for every
		 * Image the manager is managing.  It does not remove
		 * them from the manager.
		 *
		 * @see SoundClip
		 *
		 */
		virtual void freeAll();

		/** Frees all unreferenced SoundClip
		 *
		 * This calls the SoundClip::free() function for SoundClips
		 * that have no external references to them.  It does not
		 * remove them from the manager.
		 *
		 * @see SoundClip
		 *
		 */
		virtual void freeUnreferenced();

		/** Removes a SoundClip from the manager
		 *
		 * This removes all references to the SoundClip from the
		 * manager. It does not however guarantee that the resource's
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param resource A SoundClipPtr to the image to be removed
		 * from the manager
		 *
		 * @note This is useful if you want to remove ownership of a resource
		 * from the manager
		 *
		 * @see SoundClip
		 *
		 */
		virtual void remove(SoundClipPtr& resource);

		/** Removes a SoundClip from the manager
		 *
		 * This removes all references to the SoundClip from the
		 * manager. It does not however guarantee that the resource's
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param name The name of the SoundClip
		 *
		 * @see SoundClip
		 *
		 */
		virtual void remove(const std::string& name);

		/** Removes a SoundClip from the manager
		 *
		 * This removes all references to the SoundClip from the
		 * manager. It does not however guarantee that the resource's
		 * destructor is called.  If the client has any left over
		 * references to the resource it will not be freed.
		 *
		 * @param handle The handle of the SoundClip
		 *
		 * @see SoundClip
		 *
		 */
		virtual void remove(ResourceHandle handle);

		/** Removes all SoundClips from the manager
		 *
		 * This effectively removes all references to all SoundClips from
		 * the manager.  If there are left over shared pointers to
		 * any resources they will not be deleted.
		 *
		 * @see SoundClip
		 *
		 */
		virtual void removeAll();

		/** Removes all unreferenced SoundClips
		 *
		 * This effectively removes all SoundClips that dont have an
		 * external reference.  The resources will be deleted.
		 *
		 * @see SoundClip
		 *
		 */
		virtual void removeUnreferenced();

		/** Gets a shared pointer to the SoundClip
		 *
		 * If the SoundClip is not defined it will attempt to create
		 * and load the SoundClip based on the name (it assumes the name
		 * is a filename)
		 *
		 * @param name The name of the SoundClip
		 * @return A SoundClipPtr to the SoundClip
		 *
		 */
		virtual SoundClipPtr get(const std::string& name);

		/** Gets a shared pointer to the SoundClip
		 *
		 * If the resource is not defined it returns an empty
		 * (or invalid) SoundClipPtr and makes an entry in the log.
		 *
		 * @param handle The handle of the resource
		 * @return A SoundClipPtr to the resource
		 *
		 * @todo This should throw an exception instead of an
		 * empty SoundClipPtr
		 *
		 */
		virtual SoundClipPtr get(ResourceHandle handle);

		/** Gets a SoundClip handle by name
		 *
		 * Returns the SoundClip handle associated with the name
		 *
		 * @param name The name of the SoundClip
		 * @return 0 if the resource name is invalid
		 *
		 */
		virtual ResourceHandle getResourceHandle(const std::string& name);

	private:
		typedef std::map< ResourceHandle, SoundClipPtr > SoundClipHandleMap;
		typedef std::map< ResourceHandle, SoundClipPtr >::iterator SoundClipHandleMapIterator;
		typedef std::map< ResourceHandle, SoundClipPtr >::const_iterator SoundClipHandleMapConstIterator;
		typedef std::pair< ResourceHandle, SoundClipPtr > SoundClipHandleMapPair;

		typedef std::map< std::string, SoundClipPtr > SoundClipNameMap;
		typedef std::map< std::string, SoundClipPtr >::iterator SoundClipNameMapIterator;
		typedef std::map< std::string, SoundClipPtr >::const_iterator SoundClipNameMapConstIterator;
		typedef std::pair< std::string, SoundClipPtr > SoundClipNameMapPair;

		SoundClipHandleMap m_sclipHandleMap;

		SoundClipNameMap m_sclipNameMap;
	};

} //FIFE

#endif //FIFE_SOUNDCLIP_MANAGER_H
