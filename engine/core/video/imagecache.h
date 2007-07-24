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

#ifndef FIFE_IMAGECACHE_H
#define FIFE_IMAGECACHE_H

// Standard C++ library includes
#include <list>
#include <map>
#include <vector>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "renderable_location.h"
#include "renderable_provider.h"

#include "util/singleton.h"

namespace FIFE {

	class RenderAble;
	class RenderableProviderConstructor;
	class PixelBuffer;
	typedef boost::shared_ptr<PixelBuffer> PixelBufferPtr;

	/** Image cache.
	 *
	 * Image cache stores a list of images which can be retrieved by
	 * number. It automates loading and unloading of these resources.
	 * Images are added using the addImageXXX() functions, and a Renderable
	 * object is retrieved with the getImage() function. Do not store a
	 * pointer to a Renderable object, request it used getImage() each frame
	 * or else it could be unloaded. If a resource is unloadable but is 
	 * requested the cache provides a dummy resource.
	 *
	 * Question & Answer:
	 * 	Q:	May a Provider call addImageFromXXX() on the ImageCache?
	 *	A:	Yes, as long it doesn't start a separate thread to do so.
	 *
	 *	Q:	Is the ImageCache suitable for statefull information,
	 *		like Animations
	 *	A:	In principle NO. The cache assures that there is only
	 *		one RenderAble* associated with a unique resource
	 *		location.
	 *		If you need stateful information, decouple it from the
	 *		entries in the ImageCache via a clone() method for
	 *		example.
	 *
	 *	Q:	What shall I pay attention to, when modifying the
	 *		ImageCache, adding new Providers for example.
	 *	A:	The ImageCache heavily relies on std::map to work
	 *		correctly. That means that specifically the
	 *		operators '<' and '==' @b MUST work consistantly
	 *		for the RenderableLocation helper class.
	 *		The order imposed by '<' has no special meaning,
	 *		but must be consistant. This means:
	 *		(A,B,C are arbitrary RenderableLocation instances)
	 *			A '<' B and B '<' C => A '<' C
	 *			A not '<' B and B not '<' A <=> A '==' B
	 *		The problem is that if you do not preserve these
	 *		relations no obvious bugs will show up, but
	 *		any time you call addImageFromXXX the image might
	 *		get added again. This has two major results:
	 *		1) The ImageCache will get crowded. A typical
	 *		FO2 map contains 400-600 unique renderables.
	 *		With a bug in the sort order a lot of these
	 *		will be loaded multiple times, resulting in an
	 *		effective leak. (And performance loss.)
	 *		2) Maybe worse, this might hide bugs in FIFE.
	 *
	 *	The future:
	 *		1) The 'credits' based cache policy might not be
	 *		good enough. One other policy would be to use
	 *		a time stamp. Or the credit increase with each
	 *		cache hit could be non-linear:
	 *		<pre>
	 *			          _____----- Maximum
	 *			    _____/
	 *		       ____/
	 *		    __/
	 *		___/                   ------ Starting Credits
	 *
	 *		</pre>
	 *		2) For some use cases a function to exclude images
	 *		from the collect-cycle could be added.
	 *		lockImage / unlockImage
	 *		This might however be error-prone.
	 */
	class ImageCache : public DynamicSingleton<ImageCache> {
		public:
			/** Constructor.
			 *
			 * Reserves space for the image cache. Initialises the cache
			 * stats and creates the provider constructors, which create
			 * the appropiate RenderableProvider objects.
			 */
			ImageCache();

			/** Destructor. 
			 *
			 * Empties cache and prints stats.
			 */
			virtual ~ImageCache();

			enum {	UNDEFINED_ID = 0 };

			/** Adds a RenderableProviderConstructor for loading a certain type of
			 * RenderAbles. Before a RenderAble can be added, a Constructor
			 * capable of loading that type of RenderAble must be supplied.
			 *
			 * @param provider the RenderableProviderConstructor to be added
			 */
			void addRenderableProviderConstructor(RenderableProviderConstructor* provider);

			/** Adds an already created RenderAble* to the cache.
			 *
			 * @param renderable The RenderAble to be cached.
			 * @return The index of the image in the cache.
			 */
			size_t addImage(RenderAble* renderable);

			/** Adds a resource for future reference to the cache.
			 *
			 * Renderables will be created from the file or other location.
			 * The renderable will be loaded first when it is retrieved using
			 * getImage.
			 *
			 * @param location The location of the renderable to be loaded.
			 * @return The index of the image in the cache.
			 */
			size_t addImageFromLocation(const RenderableLocation& location);

			/** This is a convenience version of addImageFromLocation().
			 *
			 * It converts the filename into a RenderableLocation and then
			 * calls addImageFromLocation.
			 *
			 * @param filename The file to be loaded.
			 * @return The index of the image in the cache.
			 */
			size_t addImageFromFile(const std::string& filename);

			/** Gets the Renderable added to the cache.
			 *
			 *  This will on demand load the renderable from a location,
			 *  if it is not already loaded. Throws for invalid index.
			 *
			 * @param index The index of the RenderAble in the cache.
			 * @return Pointer to the RenderAble.
			 */
			RenderAble* getImage(size_t index);

			/** Get a PixelBuffer associated with an image
			 *
			 *  This is a convenience function, it essentially gets
			 *  the image, checks it actually is an Image and tries to
			 *  aquire it's pixel buffer. It tries hard, but may fail.
			 *  It will return an invalid PixelBufferPtr then.
			 *
			 *  @note This might cause a reload of the image, thus
			 *  invalidating any pointers you might have currently in use.
			 *
			 *  @param index The index of a RenderAble in the cache.
			 *  @return Shared pointer of a PixelBuffer
			 */
			PixelBufferPtr getPixelBuffer(size_t index);


			/** Deletes unused RenderAbles
			 *
			 * This is called at the end of each frame by Engine::mainLoop().
			 * It only unloads renderables that can be reloaded. It checks a
			 * chunk of known size each frame. It loops through these chunks
			 * every few frames. This means that the game won't grind to a halt
			 * if a large amount of data has to be unloaded, it can be unloaded
			 * over a few frames.
			 */
			void collect();

			/** Directly deletes a RenderAble
			 *
			 *  This will delete the renderable with the given id, if
			 *  the cache can reload it.
			 *
			 *  @param index The index of the RenderAble in the cache
			 */
			void unloadImage(size_t index);

			/** Prints the cache statistics to the standard output
			 */
			void printStatistics();

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

			/** Loads a renderable by index.
			 *
			 * Creates a provider for the entry if necessary and tries to
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

			/** Adds an entry to the cache list.
			 *
			 * Creates an s_cache_entry object. If it is not reloadable and isn't
			 * isn't already loaded it aborts. One of the three params has to be a
			 * valid object, ie the location must contain a valid location, the
			 * provider must be initialised or the renderable must already be loaded.
			 *
			 * @param location The location of the resource.
			 * @param provider The provider object to load the resource.
			 * @param renderable Pointer to the renderable object.
			 * @return Index of the entry in the cache.
			 * @see s_cache_entry
			 */
			size_t addCacheEntry( const RenderableLocation& location,RenderableProviderPtr provider,RenderAble* renderable);

			/** Creates provider for a cache entry.
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

			typedef std::vector<s_cache_entry> type_cache;
			typedef std::map<RenderAble*,size_t> type_renderablemap;
			typedef std::map<RenderableLocation,size_t> type_locationmap;

			typedef std::list<RenderableProviderConstructor*> type_providerconstructors;

			// List of all cache entries. All indexes returned refer to this object.
			type_cache m_cache;
			// Map of renderable objects to see if an object has already been added
			// by addImage() before.
			type_renderablemap m_renderablemap;
			// Map of locations added with addImageFromLocation() to stop the same 
			// location being added multiple times.
			type_locationmap m_locationmap;
			// List of provider constructors. See findAndSetProvider().
			type_providerconstructors m_providerconstructors;

			// Cache stats.
			s_statistic m_stats;
	};

}//FIFE

#endif
