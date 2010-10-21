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

#ifndef FIFE_POOL_H
#define FIFE_POOL_H

// Standard C++ library includes
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "resource.h"
#include "resource_location.h"

namespace FIFE {

	struct ResourceLocationComparator {
		bool operator()(const ResourceLocation* r1, const ResourceLocation* r2) const
		{
			return r1->operator<(*r2);
		}
	};

	class IResource;

	enum { RES_LOADED = 0x01, RES_NON_LOADED  = 0x02};

	/**  Pool is used to optimize memory usage for resources
	 *
	 * Pool guarantees that there is minimal amount of resources
	 *   used in cases when it is would possible that multiple
	 *   instances of the same data would be loaded into the memory.
	 *   Pool is the owner for resources taking care of their deletion.
	 */
	class Pool {
	public:
		/** Indicates invalid index for pool
		 */
		static const int INVALID_ID = -1;

		/** Default constructor.
		 *  @param name The name used in debug output.
		 */
		Pool(const std::string& name);

		/** Destructor.
		 */
		virtual ~Pool();

		/** Adds new resource provider. Transfers provider ownership to the pool
		 */
		virtual void addResourceLoader(ResourceLoader* loader);

		/** Clear the resource loaders
		 */
		virtual void clearResourceLoaders();

		/** Adds new resource into the pool using the given location.
		 * @return The index of the resource in the pool.
		 */
		virtual int addResourceFromLocation(ResourceLocation* loc);

		/** This is a convenience version of addResourceFromLocation().
		 * It converts the filename into a ResourceLocation and then
		 * calls addResourceFromLocation.
		 *
		 * @param filename The file to be loaded.
		 * @return The index of the resource in the pool.
		 */
		virtual int addResourceFromFile(const std::string& filename);

		/** Gets resource from pool with given index
		 *
		 * @param inc Specifies weither this call will increase the ref counter
		 */
		virtual IResource& get(unsigned int index, bool inc = false);

		/** Removes the resource from pool if reference counter is null
		 *
		 * @param dec Specifies weither the ref counter will be decreased
		 * before checking
		 */
		virtual void release(unsigned int index, bool dec = false);

		/** Purge all loaded resources.
		 *  This will purge all loaded resources with a ref count of zero.
		 *  Indices remain valid, though.
		 *  @return Number of resources deleted.
		 */
		virtual int purgeLoadedResources();

		/** Gets amount of resources in the pool with given status
		 */
		virtual int getResourceCount(int status);

		/** Prints the cache statistics to the log
		 */
		virtual void printStatistics();

		/** Performs a sanity check for the location map.
		 */
		void sanityCheck();

		/** Resets the pool.
		 *  This will purge all loaded resources with a ref count of zero.
		 *  The location and loader information for the locations is lost.
		 *  Only the resource loaders are retained.
		 */
		virtual void reset();

	protected:
		class PoolEntry {
		public:
			PoolEntry(): resource(0), location(0), loader(0) {}
			~PoolEntry() {
				delete location;
				delete resource;
			}

			// Pointer to the resource that is loaded.
			IResource* resource;
			// Location of the resource.
			ResourceLocation* location;
			// Resource loader.
			ResourceLoader* loader;
		};

		void findAndSetProvider(PoolEntry& entry);

		std::vector<PoolEntry*> m_entries;
		typedef std::map<ResourceLocation*, int, ResourceLocationComparator> ResourceLocationToEntry;
		ResourceLocationToEntry m_location_to_entry;
		std::vector<ResourceLoader*> m_loaders;
		std::string m_name;
	};

} // FIFE

#endif
