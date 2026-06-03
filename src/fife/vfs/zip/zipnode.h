// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_ZIP_ZIPNODE_H
#define FIFE_VFS_ZIP_ZIPNODE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <memory>
#include <ostream>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"
#include "vfs/zip/zipentry.h"

namespace FIFE
{

    struct FIFE_API ZipContentType
    {
            enum Enum : uint8_t
            {
                File = 0,  // specifies files as content type
                Directory, // specifies directories as content type
                All        // specifies everything as content type (no filter)
            };
    };

    struct FIFE_API ZipEntryData
    {
            ZipEntryData();

            uint16_t comp;
            uint32_t crc32;
            uint32_t size_comp;
            uint32_t size_real;
            uint32_t offset;
    };

    // convenience typedef
    class ZipNode;
    using ZipNodeContainer = std::vector<std::unique_ptr<ZipNode>>;

    class FIFE_API ZipNode
    {
        public:
            /**
             * Constructor for creating a node.
             *
             * The content type is inferred from the name using HasExtension.
             * @param name the name of the node
             * @param parent the parent of this node, defaults to NULL
             */
            explicit ZipNode(std::string name, ZipNode* parent = nullptr);

            /**
             * Constructor for creating a node with an explicit entry type.
             *
             * The content type is set from the ZipEntryType rather than inferred.
             * @param name the name of the node
             * @param parent the parent of this node
             * @param entryType the explicit type (File, Directory, or Symlink)
             */
            ZipNode(std::string name, ZipNode* parent, ZipEntryType entryType);

            /**
             * accessor for the name of this node
             *
             */
            std::string const & getName() const;

            /**
             * accessor for the absolute path name of this node
             */
            std::string getFullName() const;

            /**
             * accessor for the content type of this node
             *
             * @return ZipContentType enumeration
             */
            ZipContentType::Enum getContentType() const;

            /**
             * accessor for the parent node of this node
             * will be NULL if this node has no parent
             */
            ZipNode* getParent() const;

            /**
             * gives access to retrieve the children of this node
             *
             * @param contentType allows filtering on the type of nodes
             *        returned, @see ZipContentType for description
             * @return vector of ZipNode pointers matching the request
             */
            std::vector<ZipNode*> getChildren(ZipContentType::Enum contentType = ZipContentType::All) const;

            /**
             * gives access to retrieving a specific child node by name
             *
             * @param name the name of the child node to search for
             * @param contentType optional for filtering on the type of node to return
             *
             * @return the ZipNode pointer matching the child (NULL if not found)
             */
            ZipNode* getChild(std::string const & name, ZipContentType::Enum contentType = ZipContentType::All) const;

            /**
             * Allows adding a child node to this node.
             *
             * The content type is inferred from the name using HasExtension.
             * @note this should only be used internally by the FIFE zip classes
             * @param name the name to add as a child node
             * @return the newly created child ZipNode
             */
            ZipNode* addChild(std::string const & name);

            /**
             * Allows adding a child node with an explicit entry type.
             *
             * The content type is set from the ZipEntryType rather than inferred.
             * @note this should only be used internally by the FIFE zip classes
             * @param name the name to add as a child node
             * @param entryType the explicit type (File, Directory, or Symlink)
             * @return the newly created child ZipNode
             */
            ZipNode* addChild(std::string const & name, ZipEntryType entryType);

            /** allows removing a child from this node
             *  @param child the child node to remove
             */
            void removeChild(ZipNode* child);

            /** allows removing a child node by name
             *   @param name the name of the child to remove
             */
            void removeChild(std::string const & name);

            /** accessor for checking if this node has any children
             *  @return true if node has no children
             *          false if node has children
             */
            bool isLeaf() const;

            /**
             * accessor for checking if this node has children
             *
             * @return true if node has children
             *         false if node does not have children
             */
            bool isBranch() const;

            /**
             * sets the zip file entry information for this
             * node in the zip archive
             * @param entryData the entry data that goes with this node
             */
            void setZipEntryData(ZipEntryData const & entryData);

            /**
             * accessor for the entry data associated with this node
             * in the zip archive
             *  @return ZipEntryData for this node
             */
            ZipEntryData const & getZipEntryData() const;

        private:
            std::string m_name;
            ZipContentType::Enum m_contentType;
            ZipEntryData m_entryData;

            ZipNode* m_parent;
            ZipNodeContainer m_fileChildren;
            ZipNodeContainer m_directoryChildren;
    };
} // namespace FIFE

/** debug stream for a zip node
 *  useful for debugging
 *  @param os output stream
 *  @param node the ZipNode to output
 *  @return os output stream to allow for chaining
 */
std::ostream& operator<<(std::ostream& os, FIFE::ZipNode const & node);

#endif
