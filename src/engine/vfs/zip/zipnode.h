// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_ZIP_ZIPNODE_H
#define FIFE_VFS_ZIP_ZIPNODE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <ostream>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"

namespace FIFE
{

    struct /*FIFE_API*/ ZipContentType
    {
        enum Enum : uint8_t
        {
            File = 0,  // specifies files as content type
            Directory, // specifies directories as content type
            All        // specifies everything as content type (no filter)
        };
    };

    struct /*FIFE_API*/ ZipEntryData
    {
        /** constructor
         */
        ZipEntryData();

        uint16_t comp;
        uint32_t crc32;
        uint32_t size_comp;
        uint32_t size_real;
        uint32_t offset;
    };

    // convenience typedef
    class ZipNode;
    using ZipNodeContainer = std::vector<ZipNode*>;
    class /*FIFE_API*/ ZipNode
    {
    public:
        /** constructor for creating a node
         *  @param name the name of the node
         *  @param parent the parent of this node, defaults to NULL
         */
        explicit ZipNode(std::string name, ZipNode* parent = nullptr);

        /** destructor
         */
        ~ZipNode();

        /** accessor for the name of this node
         *
         */
        const std::string& getName() const;

        /** accessor for the absolute path name of this node
         */
        std::string getFullName() const;

        /** accessor for the content type of this node
         * @return ZipContentType enumeration
         */
        ZipContentType::Enum getContentType() const;

        /** accessor for the parent node of this node
         *  will be NULL if this node has no parent
         */
        ZipNode* getParent() const;

        /** gives access to retrieve the children of this node
         * @param contentType allows filtering on the type of nodes
         *        returned, @see ZipContentType for description
         * @return vector of ZipNode pointers matching the request
         */
        std::vector<ZipNode*> getChildren(ZipContentType::Enum contentType = ZipContentType::All) const;

        /** gives access to retrieving a specific child node by name
         *  @param name the name of the child node to search for
         *  @param contentType optional for filtering on the type of node to return
         *  @return the ZipNode pointer matching the child (NULL if not found)
         */
        ZipNode* getChild(const std::string& name, ZipContentType::Enum contentType = ZipContentType::All) const;

        /** allows adding a child node to this node
         *  @note this should only be used internally by the FIFE zip classes
         *  @param child the name to add as a child node
         *  @return the newly created child ZipNode
         */
        ZipNode* addChild(const std::string& name);

        /** allows removing a child from this node
         *  @param child the child node to remove
         */
        void removeChild(ZipNode* child);

        /** allows removing a child node by name
         *   @param name the name of the child to remove
         */
        void removeChild(const std::string& name);

        /** accessor for checking if this node has any children
         *  @return true if node has no children
         *          false if node has children
         */
        bool isLeaf() const;

        /** accessor for checking if this node has children
         *  @return true if node has children
         *          false if node does not have children
         */
        bool isBranch() const;

        /** sets the zip file entry information for this
         *  node in the zip archive
         *  @param entryData the entry data that goes with this node
         */
        void setZipEntryData(const ZipEntryData& entryData);

        /** accessor for the entry data associated with this node
         *  in the zip archive
         *  @return ZipEntryData for this node
         */
        const ZipEntryData& getZipEntryData() const;

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
std::ostream& operator<<(std::ostream& os, const FIFE::ZipNode& node);

#endif
