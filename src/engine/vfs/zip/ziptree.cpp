// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cassert>
#include <filesystem>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/filesystem.h"

#include "zipnode.h"
#include "ziptree.h"

namespace FIFE
{

    ZipTree::ZipTree() : m_rootNode(new ZipNode("/")) { }

    ZipTree::~ZipTree()
    {
        delete m_rootNode;
    }

    ZipNode* ZipTree::addNode(const std::string& nodePath)
    {
        fs::path filePath(nodePath);
        std::string filename = filePath.string();

        // remove the trailing '/' if it exists
        if (filename[filename.length() - 1] == '/') {
            filename = filename.erase(filename.length() - 1);
        }

        filePath = fs::path(filename);

        ZipNode* node       = m_rootNode;
        ZipNode* tempNode   = nullptr;
        ZipNode* returnNode = nullptr;
        for (fs::path::iterator iter = filePath.begin(); iter != filePath.end(); ++iter) {
            std::string pathString = GetPathIteratorAsString(iter);
            tempNode               = node->getChild(pathString);

            if (!tempNode) {
                // attempt to add the child since it was not found
                // and reset the node to the new child node
                node = node->addChild(pathString);
            } else {
                // node found so reset node
                node = tempNode;
            }

            if (node) {
                // node was found or added, set the return node properly
                returnNode = node;
            } else {
                // node was not found and could not be added
                return nullptr;
            }
        }

        return returnNode;
    }

    ZipNode* ZipTree::getNode(const std::string& name) const
    {
        fs::path filePath(name);
        ZipNode* returnNode = nullptr;
        ZipNode* tempNode   = nullptr;
        ZipNode* node       = getRootNode();
        for (fs::path::iterator iter = filePath.begin(); iter != filePath.end(); ++iter) {
            std::string pathString = GetPathIteratorAsString(iter);

            if (pathString == ".." && (node != getRootNode())) {
                // handle ".." path case by setting the node back to the parent
                node = node->getParent();
            } else {
                // look for the path name in the child nodes
                tempNode = node->getChild(pathString);

                if (tempNode) {
                    node = tempNode;

                    // node was found so we reset the return node
                    // to the newly found node
                    returnNode = node;
                }
            }
        }

        return returnNode;
    }

    ZipNode* ZipTree::getRootNode() const
    {
        return m_rootNode;
    }
} // namespace FIFE

std::ostream& operator<<(std::ostream& os, const FIFE::ZipTree& zipTree)
{
    assert(zipTree.getRootNode() != 0);

    os << *(zipTree.getRootNode());

    return os;
}