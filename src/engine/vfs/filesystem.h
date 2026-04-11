// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_H_
#define FIFE_VFS_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <filesystem>
#include <string>

// Namespace alias for convenience
namespace fs = std::filesystem;

namespace FIFE
{

    /** Helper function to determine if a path object has a parent path
     * @param path filesystem path object
     * @return true - does have parent path
     *         false - does not have parent path
     */
    bool HasParentPath(const fs::path& path);

    /** Helper function to retrieve a parent path object from a path object
     * @param path filesystem path object
     * @return a path object with parent of passed in parameter
     */
    fs::path GetParentPath(const fs::path& path);

    /** Helper function to retrieve the filename from a filesystem path
     *   filename is just the name of the file and any extension without a path
     * @param path filesystem path object
     * @return filename string
     */
    std::string GetFilenameFromPath(const fs::path& path);

    /** Helper function to retrieve a filename string from a directory iterator
     * @param iter filesystem directory_iterator object
     * @return a filename string
     */
    std::string GetFilenameFromDirectoryIterator(const fs::directory_iterator& iter);

    /** Helper function to retrieve the path iterator in a string representation
     *  @param pathIter the iterator to get the string from
     *  @return a string representing the value in the path iterator
     */
    std::string GetPathIteratorAsString(const fs::path::iterator& pathIter);

    /** Helper function to retrieve an absolute path from a given relative path
     * @param path std::string object
     * @return a filesystem path object
     */
    fs::path GetAbsolutePath(const std::string& path);

    /** Helper function to retrieve an absolute path from a given relative path
     * @param path filesystem path object
     * @return a filesystem path object
     */
    fs::path GetAbsolutePath(const fs::path& path);

    /** Helper function to check if a filename has an extension
     * @param path filesystem path object
     * @return true if filename has extension, false if not
     */
    bool HasExtension(const std::string& path);

    /** Helper function to check if a filename has an extension
     * @param path filesystem path object
     * @return true if filename has extension, false if not
     */
    bool HasExtension(const fs::path& path);

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
    std::string GetExtension(const fs::path& path);

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
    std::string GetStem(const fs::path& path);
} // namespace FIFE

#endif