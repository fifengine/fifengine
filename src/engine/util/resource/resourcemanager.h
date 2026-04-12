// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_RESOURE_MANAGER_H
#define FIFE_RESOURE_MANAGER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "resource.h"

namespace FIFE
{

    /** IResourceManager
     *
     * An interface for managing resources.
     *
     * @see IResource
     * @see IResourceLoader
     *
     */

    class /*FIFE_API*/ IResourceManager
    {
    public:
        /** Default constructor.
         */
        IResourceManager() = default;

        /** destructor
         */
        virtual ~IResourceManager() = default;

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
        virtual size_t getMemoryUsed() const = 0;

        /** Returns the number of unloaded resources
         *
         * Gets the total number of resources created but not loaded.
         * These resources have been defined in the resource manager
         * but have yet to be loaded from disk.
         *
         * @return Total resources created
         *
         */
        virtual size_t getTotalResourcesCreated() const = 0;

        /** Returns the number of loaded resources
         *
         * Gets the total number of resources created and loaded.
         * These resources have been defined in the resource manager
         * have been loaded from disk.
         *
         * @return Total resources loaded
         *
         */
        virtual size_t getTotalResourcesLoaded() const = 0;

        /** Returns the number of defined resources
         *
         * Gets the total number of resources managed by the manager.
         *
         * @return Total resources managed
         *
         */
        virtual size_t getTotalResources() const = 0;
    };

} // namespace FIFE

#endif
