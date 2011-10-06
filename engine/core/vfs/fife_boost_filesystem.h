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
#ifndef FIFE_VFS_H_
#define FIFE_VFS_H_

// Standard C++ library includes

// 3rd party library includes
#include <boost/filesystem.hpp>
// #include <boost/filesystem/operations.hpp>
// #include <boost/filesystem/path.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace bfs = boost::filesystem;

namespace FIFE {

    /** Helper function to determine if a path object has a parent path
    * @param boost filesystem path object
    * @return true - does have parent path 
    *         false - does not have parent path
    */
    bool HasParentPath(const bfs::path& path);

    /** Helper function to retrieve a parent path object from a path object
    * @param boost filesystem path object
    * @return a path object with parent of passed in parameter
    */
    bfs::path GetParentPath(const bfs::path& path);

    /** Helper function to retrieve a filename string from a directory iterator
    * @param boost filesystem directory_iterator object
    * @return a filename string
    */
    std::string GetFilenameFromDirectoryIterator(const bfs::directory_iterator& iter);
}

#endif
