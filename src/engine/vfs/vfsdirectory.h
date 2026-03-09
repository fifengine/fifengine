// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_VFSHOSTSYSTEM_H
#define FIFE_VFS_VFSHOSTSYSTEM_H

// Standard C++ library includes
#include <set>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfssource.h"

namespace FIFE
{

    /**
     * The most basic VFSSource for "normal" filesystems.
     * For example, '/' or './tests/data'.
     */
    class VFSDirectory : public VFSSource
    {
    public:
        /** Constructor
         * Creates the given file system's VFS Source.
         */
        VFSDirectory(VFS* vfs, const std::string& root = "./");
        /** Destructor
         */
        virtual ~VFSDirectory();

        /** Tests whether a file can be opened.
         * @param filename The file to test.
         * @return True, if the file filename can be opened.
         */
        virtual bool fileExists(const std::string& filename) const;
        /** Opens a file.
         * @param filename The file to open.
         */
        virtual RawData* open(const std::string& filename) const;

        /** List files in a directory
         * @param path The directory to list the files in
         * @return A string list of filenames
         * Will return an empty list on \b any error
         */
        std::set<std::string> listFiles(const std::string& path) const;

        /** List directories in a directory
         * @param path The directory to list the directories in
         * @return A string list of directories
         * Will return an empty list on \b any error
         */
        std::set<std::string> listDirectories(const std::string& path) const;

    private:
        std::string m_root;

        std::set<std::string> list(const std::string& path, bool directorys) const;
    };

} // namespace FIFE

#endif
