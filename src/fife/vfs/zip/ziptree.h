// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_ZIP_ZIPTREE_H
#define FIFE_VFS_ZIP_ZIPTREE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <memory>
#include <ostream>
#include <string>

// FIFE includes
#include "vfs/zip/zipentry.h"

namespace FIFE
{

    class ZipNode;

    class FIFE_API ZipTree
    {
        public:
            /** constructor
             */
            ZipTree();

            ZipTree(ZipTree const &)            = delete;
            ZipTree& operator=(ZipTree const &) = delete;
            /**
             * Adds a node to the proper place in the tree
             * based on the node path name.
             *
             * @param nodePath the node description
             * @param entryType the type of the node to add
             * @return ZipNode of the most derived path added
             *         (value after the last path separator)
             *         or NULL if the path could not be added
             */
            ZipNode* addNode(std::string const & nodePath, ZipEntryType entryType);

            /**
             * accessor for getting a node by name.
             *
             * @param name the name to search for
             * @return ZipNode pointer, or NULL if not found
             */
            ZipNode* getNode(std::string const & name) const;

            /** accessor for the root node of the tree
             *  mostly used for debugging purposes
             *  @return ZipNode pointer to the root node
             */
            ZipNode* getRootNode() const;

        private:
            std::unique_ptr<ZipNode> m_rootNode;
    };
} // namespace FIFE

std::ostream& operator<<(std::ostream& os, FIFE::ZipTree const & zipTree);

#endif
