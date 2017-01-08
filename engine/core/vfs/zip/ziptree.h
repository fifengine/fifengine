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
#ifndef FIFE_VFS_ZIP_ZIPTREE_H
#define FIFE_VFS_ZIP_ZIPTREE_H

// Standard C++ library includes
#include <string>
#include <vector>
#include <ostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder


namespace FIFE {

    class ZipNode;

    class ZipTree {
    public:
        /** constructor
         */
        ZipTree();

        /** destructor
         */
        ~ZipTree();

        /** adds a node to the proper place in the tree
         *  based on the node path name
         *  @param nodePath the node description
         *  @return ZipNode of the most derived path added
                    (value after the last path separator)
                    or NULL if the path could not be added
         */
         ZipNode* addNode(const std::string& nodePath);

        /** accessor for getting a node by name
         *  @param name the name to search for
         *  @ param ZipNode pointer, or NULL if not found
         */
        ZipNode* getNode(const std::string& name) const;

        /** accessor for the root node of the tree
         *  mostly used for debugging purposes
         *  @return ZipNode pointer to the root node
         */
        ZipNode* getRootNode() const;
    private:
        ZipNode* m_rootNode;
    };
}

/** debug stream for a zip tree
 *  useful for debugging
 *  @param os output stream
 *  @param zipTree the ZipTree to output
 *  @return os output stream to allow for chaining
 */
std::ostream& operator<<(std::ostream& os, const FIFE::ZipTree& zipTree);


#endif
