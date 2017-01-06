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
    * @param path boost filesystem path object
    * @return true - does have parent path
    *         false - does not have parent path
    */
    bool HasParentPath(const bfs::path& path);

    /** Helper function to retrieve a parent path object from a path object
    * @param path boost filesystem path object
    * @return a path object with parent of passed in parameter
    */
    bfs::path GetParentPath(const bfs::path& path);

    /** Helper function to retrieve the filename from a boost filesystem path
    *   filename is just the name of the file and any extension without a path
    * @param path boost filesystem path object
    * @return filename string
    */
    std::string GetFilenameFromPath(const bfs::path& path);

    /** Helper function to retrieve a filename string from a directory iterator
    * @param iter boost filesystem directory_iterator object
    * @return a filename string
    */
    std::string GetFilenameFromDirectoryIterator(const bfs::directory_iterator& iter);

    /** Helper function to retrieve the path iterator in a string representation
     *  @param pathIter the iterator to get the string from
     *  @return a string representing the value in the path iterator
     */
    std::string GetPathIteratorAsString(const bfs::path::iterator& pathIter);

    /** Helper function to retrieve an absolute path from a given relative path
    * @param path std::string object
    * @return a boost filesystem path object
    */
    bfs::path GetAbsolutePath(const std::string& path);

    /** Helper function to retrieve an absolute path from a given relative path
    * @param path boost filesystem path object
    * @return a boost filesystem path object
    */
    bfs::path GetAbsolutePath(const bfs::path& path);

    /** Helper function to check if a filename has an extension
    * @param path boost filesystem path object
    * @return true if filename has extension, false if not
    */
    bool HasExtension(const std::string& path);

    /** Helper function to check if a filename has an extension
     * @param path boost filesystem path object
     * @return true if filename has extension, false if not
     */
    bool HasExtension(const bfs::path& path);

    /** Helper function to retrieve the extension
     *  @note the extension includes the "."
     *  @param path the input path string
     *  @return the extension on the input path
     */
    std::string GetExtension(const std::string& path);

    /** Helper function to retrieve the extension
    *  @note the extension includes the "."
    *  @param path the input path object
    *  @return the extension on the input path
    */
    std::string GetExtension(const bfs::path& path);

    /** Helper function to retrieve the filename minus any extension
     *  @note the "." is NOT included
     *  @param path the input path string
     *  @return the filename minus any extension
     */
    std::string GetStem(const std::string& path);

    /** Helper function to retrieve the filename minus any extension
    *  @note the "." is NOT included
    *  @param path the input path object
    *  @return the filename minus any extension
    */
    std::string GetStem(const bfs::path& path);
}

#endif
