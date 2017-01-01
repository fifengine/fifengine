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

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/fife_boost_filesystem.h"

#include "ziptree.h"
#include "zipnode.h"

namespace FIFE {

    ZipTree::ZipTree() 
    : m_rootNode(new ZipNode("/")) {

    }

    ZipTree::~ZipTree() {
        delete m_rootNode;
    }

    ZipNode* ZipTree::addNode(const std::string& nodePath) {
        bfs::path filePath(nodePath);
        std::string filename = filePath.string();

        // remove the trailing '/' if it exists because the
        // boost::filesystem::path objects will report extra
        // path parts otherwise
        if (filename[filename.length()-1] == '/') {
            filename = filename.erase(filename.length()-1);
        }

        filePath = bfs::path(filename);

        ZipNode* node = m_rootNode;
        ZipNode* tempNode = 0;
        ZipNode* returnNode = 0;
        for (bfs::path::iterator iter = filePath.begin(); iter != filePath.end(); ++iter) {
            std::string pathString = GetPathIteratorAsString(iter);
            tempNode = node->getChild(pathString);

            if (!tempNode) {
                // attempt to add the child since it was not found
                // and reset the node to the new child node
                node = node->addChild(pathString);
            }
            else {
                // node found so reset node
                node = tempNode;
            }

            if (node) {
                // node was found or added, set the return node properly
                returnNode = node;
            }
            else {
                // node was not found and could not be added
                return 0;
            }
        }

        return returnNode;
    }

    ZipNode* ZipTree::getNode(const std::string& name) const {
        bfs::path filePath(name);
        ZipNode* returnNode = 0;
        ZipNode* tempNode = 0;
        ZipNode* node = getRootNode();
        for (bfs::path::iterator iter = filePath.begin(); iter != filePath.end(); ++iter) {
            std::string pathString = GetPathIteratorAsString(iter);

            if (pathString == ".." && (node != getRootNode())) {
                // handle ".." path case by setting the node back to the parent
                node = node->getParent();
            }
            else {
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

    ZipNode* ZipTree::getRootNode() const {
        return m_rootNode;
    }
}

std::ostream& operator<<(std::ostream& os, const FIFE::ZipTree& zipTree) {
    assert(zipTree.getRootNode() != 0);

    os << *(zipTree.getRootNode());

    return os;
}