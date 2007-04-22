/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include "singleton.h"
#include "video/renderable_location.h"
#include "video/renderable_provider.h"
#include <vector>
#include <map>
#include <list>

namespace FIFE {
	class RenderAble;
	class RenderableProviderConstructor;

	/**  Image cache
     *
     * Image cache stores a list of images which can be retrieved by
	 * number. It automates loading and unloading of these resources.
	 * Images are added using the addImageXX() functions, and a Renderable
	 * object is retrieved with the getImage() function. Do not store a
	 * pointer to a Renderable object, request it used getImage each frame
	 * or else it could be unloaded. If a resource is unloadable but is 
	 * requested the cache provides a dummy resource.
     */
	class ImageCache : public DynamicSingleton<ImageCache> {
		public:
			/** Constructor.
			 *
			 * Reserves space for the image cache. Initialised the cache
			 * stats and creates the provider constructors, which create
			 * the appropriete RenderableProvider objects.
			 */
			ImageCache();

			/** Destructor. Empties cache and prints stats.
			 */
			virtual ~ImageCache();

			enum {	UNDEFINED_ID = 0 };

			/** Add an already created RenderAble* to the cache.
			 *
			 * @param renderable The RenderAble to be cached.
			 * @return The index of the image in the cache.
			 */
			size_t addImage(RenderAble* renderable);

			/** Add a resource for future reference to the cache.
			 *
			 * Renderables will be created from the file or other location.
			 * The renderable will be loaded first when it is retrieved using
			 * getImage.
			 *
			 * @param location The location of the renderable to be loaded.
			 * @return The index of the image in the cache.
			 */
			size_t addImageFromLocation(const RenderableLocation& location);

			/** This is a convenience version of addImageFromLocation.
			 *
			 * It converts the filename into a RenderableLocation and then
			 * calls addImageFromLocation.
			 *
			 * @param filename The file to be loaded.
			 * @return The index of the image in the cache.
			 */
			size_t addImageFromFile(const std::string& filename);

			/** Get the Renderable added to the cache.
			 *
			 *  This will on demand load the renderable from a location,
			 *  if it is not already loaded. Throws for invalid index.
			 *
			 * @param index The index of the RenderAble in the cache.
			 * @return Pointer to the RenderAble.
			 */
			RenderAble* getImage(size_t index);

			/** Delete unused RenderAbles
			 *
			 * This is called at the end of each frame by Engine::mainLoop.
			 * It only unloads renderables that can be reloaded. It checks a
			 * chunk of known size each frame. It loops through these chunks
			 * every few frames. This means that the game won't grind to a halt
			 * if a large amount of data has to be unloaded, it can be unloaded
			 * over a few frames.
			 */
			void collect();

		private:
			/** A cache entry.
			 *
			 * Includes all the information needed to reload and maange.
			 */
			struct s_cache_entry {
				// Pointer to the renderable is loaded.
				RenderAble* renderable;
				// The location of the renderable.
				RenderableLocation location;
				// The loader object.
				RenderableProviderPtr provider;
				// Credits, used for managing unused resources.
				int credits;
				// If the entry can be unloaded and reloaded if nessecary.
				bool   reloadable;
			};

			/** Load a renderable by index.
			 *
			 * Creates a provider for the entry if nessecary and tries to
			 * load the renderable. If it fails it returns a dummy 
			 * renderable.
			 *
			 * @param index The index of the RenderAble to be loaded.
			 * @return Pointer to the RenderAble.
			 */
			RenderAble* loadImage( size_t index );

			/** Unloads unused resources.
			 *
			 * Checks all the entries between the two given indexes.
			 * If an entry is reloadable and its credits have fallen low
			 * enough it is unloaded.
			 *
			 * @param start The beginning cache index.
			 * @param end The end cache index
			 * @param turns The amount to decrement each entry's credits by.
			 */
			void collectChunk(size_t start, size_t end,int turns);

			/** Unloads a renderable.
			 *
			 * Deletes the renderable and unloads the RenderableProvider.
			 * The resource is still in the cache list and can be reloaded.
			 *
			 * @param centry The cache entry to be unloaded.
			 */
			void unloadImage( s_cache_entry& centry );

			/** Calls cleanCache() and cleanProviders(). Called by destructor.
			 */
			void cleanUp();

			/** Deletes all renderables and empties the cache.
			 */
			void cleanCache();

			/** Deletes all providers and clears the provider list.
			 */
			void cleanProviders();

			/** Stores the cache statistics.
			 */
			struct s_statistic {
				// Number of loadable resources.
				int loadable;
				// Number of dummies loaded.
				int dummies;
				// Number of entries in the cache.
				size_t total;
				// The amount to decrement each entries credits by when checking
				// for collection.
				unsigned int turns;
				// The index of the next chunk of entries to be checked for 
				// collection.
				unsigned int cstart;
				// The size of each colection chunk to be checked.
				unsigned int chunk_size;
				// The number of entries with loaded renderables.
				size_t alive;
			};

			/** Add an entry to the cache list.
			 *
			 * Creates an s_cache_entry object. If it is not reloadable and isn't
			 * isn't already loaded it aborts. One of the three params has to be a
			 * valid object. ie the location must contain a valid location, the
			 * provider must be initialised or the renderable must already be loaded.
			 *
			 * @param location The location of the resource.
			 * @param provider The provider object to load the resource.
			 * @param renderable Pointer to the renderable object.
			 * @return Index of the entry in the cache.
			 *
			 * @see s_cache_entry
			 */
			size_t addCacheEntry( const RenderableLocation& location,RenderableProviderPtr provider,RenderAble* renderable);

			/** Create provider for a cache entry.
			 *
			 * If an object is created from a location the first time it is loaded
			 * it needs a provider to do the loading. This function finds the correct
			 * provider for the cache entry from the provider list. It also loads
			 * the renderable in this function for the first time. For following
			 * reloads it is loaded in loadImage().
			 *
			 * @param renderable Pointer to the renderable object.
			 */
			void findAndSetProvider( s_cache_entry& cacheEntry );

			/** Prints the cache statistics to the standard output
			 */
			void printStatistics();

			typedef std::vector<s_cache_entry> type_cache;
			typedef std::map<RenderAble*,size_t> type_renderablemap;
			typedef std::map<RenderableLocation,size_t> type_locationmap;

			typedef std::list<RenderableProviderConstructor*> type_providerconstructors;

			// list of all cache entries. all indexes returned refer to this object.
			type_cache m_cache;
			// map of renderable objects to see if an object has already been added
			// by addImage() before.
			type_renderablemap m_renderablemap;
			// map of locations added with addImageFromLocation() to stop the same 
			// location being added multiple times.
			type_locationmap m_locationmap;
			// List of provider constructors. See findAndSetProvider().
			type_providerconstructors m_providerconstructors;

			// Cache stats.
			s_statistic m_stats;
	};

}//FIFE

#endif
